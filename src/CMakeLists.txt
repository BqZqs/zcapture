add_executable(zcapture
    main.cpp
    mainwindow.cpp
    captureoverlay.cpp
)

target_link_libraries(zcapture PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_include_directories(zcapture PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_subdirectory(tools)
