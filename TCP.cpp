#include "TCP.h"
#include "Comm.h"

#include "tiny-json.h"

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <pico/multicore.h>

#include <plog/Log.h>

#include <iostream>
#include <sstream>
#include <iomanip>

namespace mini_lcd
{
TCPTest::TCPTest()
{
    if (cyw43_arch_init() != 0) {
        PLOG_ERROR << "cyw43 init failed!";
        return;
    }
    cyw43_arch_enable_sta_mode();
}

TCPTest::~TCPTest()
{
    PLOG_INFO << "Closing TCP client";
    cyw43_arch_deinit();
}

bool TCPTest::connect()
{
    if (cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        PLOG_ERROR << "failed to connect.";
        return false;
    }
    PLOG_INFO << "Connected.";
    return true;
}

void TCPTest::getMeasurements()
{
    if (reporting_) {
        return;
    }
    reporting_ = true;
    ip4addr_aton(SERVER_ADDR, &remote_addr);
    pcb = tcp_new_ip_type(IP_GET_TYPE(&remote_addr));
    if (!pcb) {
        PLOG_ERROR << "Failed to create pcb";
        return;
    }

    tcp_arg(pcb, this);

    tcp_poll(pcb, &TCPTest::tcp_client_poll, 20);
    tcp_sent(pcb, &TCPTest::tcp_client_sent);
    tcp_recv(pcb, &TCPTest::tcp_client_recv);
    tcp_err(pcb, &TCPTest::tcp_client_err);

    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(pcb, &remote_addr, 8700, tcp_client_connected);
    PLOG_DEBUG << "Connecting to " << ip4addr_ntoa(&remote_addr) << " result: " << (int)err << "";
    cyw43_arch_lwip_end();
}

err_t TCPTest::poll(tcp_pcb* arg)
{
    PLOG_INFO << "TCP timeout...";
    return close(-1);
}
err_t TCPTest::sent(tcp_pcb* tpcb, u16_t len)
{
    PLOG_DEBUG << "Sent " << (int)len;
    return ERR_OK;
}
err_t TCPTest::recv(tcp_pcb* arg, pbuf* buf, err_t err)
{
    if (!buf) {
        PLOG_WARNING << "TCP: Received empty buffer";
        return close(-1);
    }
    PLOG_DEBUG << "TCP received " << (int)buf->len << " " << (int)buf->tot_len;
    PLOG_VERBOSE << std::string_view{(char*)buf->payload, buf->len};
    auto jsonStr = std::string((char*)buf->payload, buf->len);
    auto jsonStartPos = jsonStr.find('{');
    if (jsonStartPos == std::string::npos) {
        PLOG_WARNING << "No JSON object found in response";
        tcp_recved(pcb, buf->tot_len);
        pbuf_free(buf);
        return close(0);
    }
    jsonStr = jsonStr.substr(jsonStartPos);
    PLOG_VERBOSE << "JSON: " << jsonStr;
    json_t pool[128];
    auto parent = json_create((char*)jsonStr.c_str(), pool, sizeof pool / sizeof *pool);
    if (parent == nullptr) {
        PLOG_WARNING << "Failed to parse JSON";
        tcp_recved(pcb, buf->tot_len);
        pbuf_free(buf);
        return close(0);
    }
    auto measurements = json_getProperty(parent, "measurements");
    if (!measurements || json_getType(measurements) != JSON_ARRAY) {
        PLOG_WARNING << "No measurements found in JSON";
        tcp_recved(pcb, buf->tot_len);
        pbuf_free(buf);
        return close(0);
    }

    auto parseAndSend = [](const json_t* measurement, const char* value) {
        uint32_t val = 0;
        auto valStr = json_getPropertyValue(measurement, value);
        if (valStr) {
            val = std::stoi(valStr);
        }
        multicore_fifo_push_blocking(val);
    };

    for (auto measurement = json_getChild(measurements); measurement;
        measurement = json_getSibling(measurement)) {
        multicore_fifo_push_blocking(static_cast<uint32_t>(mini_lcd::Message::Type::Measurements));
        if (json_getType(measurement) == JSON_OBJ) {
            auto time = json_getPropertyValue(measurement, "time");
            for (int i = 0; i < 16; ++i) {
                std::string cpuName = "CPU" + std::to_string(i);
                parseAndSend(measurement, cpuName.c_str());
            }
            parseAndSend(measurement, "RAM");
            parseAndSend(measurement, "GPU");
            parseAndSend(measurement, "GPUVD");
            parseAndSend(measurement, "GPUVE");
            parseAndSend(measurement, "GPUMEM");
        }
    }

    tcp_recved(pcb, buf->tot_len);
    pbuf_free(buf);
    PLOG_DEBUG << "Measurements sent to core 1";
    return close(0);
}
void TCPTest::error(err_t err)
{
    PLOG_ERROR << "TCP error " << (int)err;
    close(-1);
}
err_t TCPTest::conn(tcp_pcb* arg, err_t err)
{
    if (err != ERR_OK) {
        PLOG_ERROR << "connect failed " << (int)err;
        return close(err);
    }

    std::stringstream ss;
    ss << "GET /measurements HTTP/1.1\r\n"
          "\r\n\r\n";

    auto res = tcp_write(pcb, ss.str().data(), ss.str().size(), TCP_WRITE_FLAG_COPY);
    PLOG_DEBUG << "TCP write result: " << (int)res;
    return ERR_OK;
}

err_t TCPTest::close(int status)
{
    tcp_arg(pcb, nullptr);
    tcp_poll(pcb, nullptr, 0);
    tcp_sent(pcb, nullptr);
    tcp_recv(pcb, nullptr);
    tcp_err(pcb, nullptr);
    auto res = tcp_close(pcb);
    if (res != ERR_OK) {
        PLOG_ERROR << "close failed " << (int)res << ", calling abort";
        tcp_abort(pcb);
        res = ERR_ABRT;
    }
    reporting_ = false;
    return res;
}
} // namespace mini_lcd
