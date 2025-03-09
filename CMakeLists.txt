cmake_minimum_required(VERSION 3.10)
project(libsdp VERSION 1.0.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(src SOURCE_SRC)

# 添加库目标
add_library(libsdp SHARED ${SOURCE_SRC})

add_executable(sdp_test test.cpp ${SOURCE_SRC})

# 设置库的公共头文件目录
target_include_directories(sdp_test
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(libsdp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# 设置库的版本号
set_target_properties(libsdp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# 安装库文件
install(TARGETS libsdp
    EXPORT libsdpTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装头文件
install(DIRECTORY include/
    DESTINATION include
)

# 导出库的 CMake 目标
install(EXPORT libsdpTargets
    FILE libsdpTargets.cmake
    NAMESPACE libsdp::
    DESTINATION lib/cmake/libsdp
)

# 生成版本文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libsdpConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 生成配置文件
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libsdpConfig.cmake"
    INSTALL_DESTINATION lib/cmake/libsdp
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# 安装配置文件
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libsdpConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/libsdpConfigVersion.cmake"
    DESTINATION lib/cmake/libsdp
)
