cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS  TRUE)

if(COMMAND cmake_policy)
    cmake_policy(VERSION 2.6)
endif()

set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}) 
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT EXISTS ${SOURCE_DIR}/ceresConfig.cmake)
    message(FATAL_ERROR "Please provide ceresConfig.cmake")
endif()

include(ceresConfig.cmake)

include_directories(
    ${SOURCE_DIR}
    ${BUILD_DIR}
    ${CERES_COMMON_REQUIRED_INCLUDE_DIRS}
    ${CERES_REQUIRED_INCLUDE_DIRS}
    ${CERES_OPTIONAL_INCLUDE_DIRS})

# Sources {{{
set(CER_SRCS 
    ${SOURCE_DIR}/ceres.c
    ${SOURCE_DIR}/util.c
    ${SOURCE_DIR}/draw.c
    ${SOURCE_DIR}/xutil.c
    ${SOURCE_DIR}/color.c
    ${SOURCE_DIR}/window.c
    ${SOURCE_DIR}/client.c
    ${SOURCE_DIR}/atoms/atoms.c
    ${SOURCE_DIR}/ewmh.c
    ${SOURCE_DIR}/screen.c
    ${SOURCE_DIR}/mouse.c
    ${SOURCE_DIR}/root.c
    ${SOURCE_DIR}/key.c
    ${SOURCE_DIR}/layout.c
    ${SOURCE_DIR}/event.c)
# }}}

add_executable(${PROJECT_CER_NAME} 
    ${CER_SRCS})

set_target_properties(${PROJECT_AWE_NAME}
    PROPERTIES
    LINK_FLAGS -export-dynamic)

target_link_libraries(${PROJECT_CER_NAME}
    ${CERES_COMMON_REQUIRED_LIBRARIES}
    ${CERES_REQUIRED_LIBRARIES}
    ${CERES_OPTIONAL_LIBRARIES})

# Installation {{{
install(TARGETS ${PROJECT_CER_NAME} RUNTIME DESTINATION bin)
# }}}

# vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80:filetype=cmake
