set(T t)

add_executable(${T} main.c)
target_link_libraries(${T} c_gwr m)
add_custom_command(
        TARGET ${T}
        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${T}>/textures"

        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${T}>/shaders"

#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${CMAKE_CURRENT_SOURCE_DIR}/textures"
#        "$<TARGET_FILE_DIR:${T}>/textures"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:${T}>/shaders"
)
