add_executable(pixelshot
    main.cpp
    mainwindow.cpp
    captureoverlay.cpp
)

target_link_libraries(pixelshot PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_include_directories(pixelshot PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_subdirectory(tools)
add_subdirectory(widgets)
