add_library(hagl INTERFACE)

target_sources(hagl INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/hagl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_blit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_char.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_circle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_clip.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_ellipse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_hline.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_image.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_line.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_pixel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_polygon.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_rectangle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_triangle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_vline.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_bitmap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/fontx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hsl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rgb565.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rgb888.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tjpgd.c
    ${CMAKE_CURRENT_LIST_DIR}/fonts.cpp

    ${CMAKE_CURRENT_LIST_DIR}/mipi_display.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_hal_single.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_hal_double.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hagl_hal_triple.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Display.cpp
)

target_include_directories(hagl INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(hagl INTERFACE pico_stdlib hardware_spi hardware_gpio hardware_dma)
