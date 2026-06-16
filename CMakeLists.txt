cmake_minimum_required(VERSION 3.22)
project(zcapture VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(zcapture src/main.cpp)

if(WIN32)
    set_property(TARGET zcapture PROPERTY WIN32_EXECUTABLE true)
endif()

target_link_libraries(zcapture PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_include_directories(zcapture PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)

# Windows portable packaging
if(WIN32)
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-win64-portable")
    set(CPACK_GENERATOR ZIP)
    include(CPack)
endif()
