add_library(hagl_hal INTERFACE)

target_sources(hagl_hal INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/mipi_display.cpp
  ${CMAKE_CURRENT_LIST_DIR}/hagl_hal_single.cpp
  ${CMAKE_CURRENT_LIST_DIR}/hagl_hal_double.cpp
  ${CMAKE_CURRENT_LIST_DIR}/hagl_hal_triple.cpp
  ${CMAKE_CURRENT_LIST_DIR}/times.c
)

target_include_directories(hagl_hal INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(hagl_hal INTERFACE pico_stdlib hardware_spi hardware_gpio hardware_dma hagl)
