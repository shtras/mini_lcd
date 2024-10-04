add_library(hagl INTERFACE)

target_sources(hagl INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_blit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_char.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_circle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_clip.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_ellipse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_hline.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_image.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_line.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_pixel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_polygon.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_rectangle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_triangle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_vline.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hagl_bitmap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/fontx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/hsl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/rgb565.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/rgb888.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/tjpgd.c
)

target_include_directories(hagl INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(hagl INTERFACE hagl_hal)
