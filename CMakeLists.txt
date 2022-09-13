cmake_minimum_required(VERSION 3.11)

project(RangeLineEdit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(RangeLineEdit
    line_part.h
    line_part.cc
    DoubleLineEdit.cpp
    LatitudeLineEdit.cpp
    LongitudeLineEdit.cpp
    MainWindow.cpp
    PhoneNumberLineEdit.cpp
    PositionalLineEdit.cpp
    Ranges.cpp
    TrianglePaintedButton.cpp
    main.cpp
)

target_link_libraries(RangeLineEdit
    PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)