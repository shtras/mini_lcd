#include "TCP.h"
#include "Comm.h"
#include "Logger.h"

#include "Utils/tiny-json.h"

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <pico/multicore.h>

#include <iostream>
#include <sstream>
#include <iomanip>

namespace mini_lcd
{
TCPTest::TCPTest()
{
    if (cyw43_arch_init() != 0) {
        // std::cout << "cyw43 init failed!\n";
        return;
    }
    cyw43_arch_enable_sta_mode();
}

TCPTest::~TCPTest()
{
    // std::cout << "Closing TCP client\n";
    cyw43_arch_deinit();
}

bool TCPTest::Connect()
{
    if (cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        // std::cout << "failed to connect.\n";
        return false;
    }
    // std::cout << "Connected.\n";
    return true;
}

void TCPTest::GetMeasurements()
{
    if (reporting_) {
        return;
    }
    reporting_ = true;
    ip4addr_aton(SERVER_ADDR, &remote_addr);
    pcb = tcp_new_ip_type(IP_GET_TYPE(&remote_addr));
    if (!pcb) {
        // std::cout << "Failed to create pcb\n";
        return;
    }

    tcp_arg(pcb, this);

    tcp_poll(pcb, &TCPTest::tcp_client_poll, 20);
    tcp_sent(pcb, &TCPTest::tcp_client_sent);
    tcp_recv(pcb, &TCPTest::tcp_client_recv);
    tcp_err(pcb, &TCPTest::tcp_client_err);

    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(pcb, &remote_addr, 8700, tcp_client_connected);
    Logger::debug() << "Connecting to " << ip4addr_ntoa(&remote_addr) << " result: " << (int)err << "\n";
    cyw43_arch_lwip_end();
}

err_t TCPTest::poll(tcp_pcb* arg)
{
    // std::cout << "Poll\n";
    return close(-1);
}
err_t TCPTest::sent(tcp_pcb* tpcb, u16_t len)
{
    // std::cout << "Sent " << (int)len << "\n";
    return ERR_OK;
}
err_t TCPTest::recv(tcp_pcb* arg, pbuf* buf, err_t err)
{
    if (!buf) {
        // std::cout << "TCP: Received empty buffer\n";
        return close(-1);
    }
    Logger::debug() << "TCP received " << (int)buf->len << " " << (int)buf->tot_len << "\n";
    Logger::trace() << std::string_view{(char*)buf->payload, buf->len} << "\n";
    auto jsonStr = std::string((char*)buf->payload, buf->len);
    auto jsonStartPos = jsonStr.find('{');
    if (jsonStartPos == std::string::npos) {
        Logger::warn() << "No JSON object found in response\n";
        tcp_recved(pcb, buf->tot_len);
        pbuf_free(buf);
        return close(0);
    }
    jsonStr = jsonStr.substr(jsonStartPos);
    Logger::trace() << "JSON: " << jsonStr << "\n";
    json_t pool[128];
    auto parent = json_create((char*)jsonStr.c_str(), pool, sizeof pool / sizeof *pool);
    if (parent == nullptr) {
        Logger::error() << "Failed to parse JSON\n";
        tcp_recved(pcb, buf->tot_len);
        pbuf_free(buf);
        return close(0);
    }
    auto measurements = json_getProperty(parent, "measurements");
    if (!measurements || json_getType(measurements) != JSON_ARRAY) {
        Logger::warn() << "No measurements found in JSON\n";
        tcp_recved(pcb, buf->tot_len);
        pbuf_free(buf);
        return close(0);
    }

    Message msg;
    msg.type = Message::Type::Measurements;
    int idx = 0;

    auto parseAndSend = [&msg, &idx](const json_t* measurement, const char* value) {
        uint32_t val = 0;
        auto valStr = json_getPropertyValue(measurement, value);
        if (valStr) {
            val = std::stoi(valStr);
        }
        msg.data[idx++] = val;
    };

    for (auto measurement = json_getChild(measurements); measurement;
        measurement = json_getSibling(measurement)) {
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
    Sender::GetInstance().Send(msg);

    tcp_recved(pcb, buf->tot_len);
    pbuf_free(buf);
    Logger::debug() << "Measurements sent to core 1\n";
    return close(0);
}
void TCPTest::error(err_t err)
{
    Logger::error() << "TCP error " << (int)err << "\n";
    close(-1);
}
err_t TCPTest::conn(tcp_pcb* arg, err_t err)
{
    if (err != ERR_OK) {
        std::cout << "connect failed " << (int)err << "\n";
        return close(err);
    }

    std::stringstream ss;
    ss << "GET /measurements HTTP/1.1\r\n"
          "\r\n\r\n";

    auto res = tcp_write(pcb, ss.str().data(), ss.str().size(), TCP_WRITE_FLAG_COPY);
    Logger::debug() << "TCP write result: " << (int)res << "\n";
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
        Logger::error() << "close failed " << (int)res << ", calling abort\n";
        tcp_abort(pcb);
        res = ERR_ABRT;
    }
    reporting_ = false;
    return res;
}
} // namespace mini_lcd
