cmake_minimum_required(VERSION 3.29)
project(Homework1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "E:/Qt/6.8.0/mingw_64/lib/cmake")
set(ZLIB_LIBRARY "C:\\Users\\Admin\\.vcpkg-clion\\vcpkg\\packages\\zlib_x64-mingw-dynamic\\lib")
set(ZLIB_INCLUDE_DIR "C:\\Users\\Admin\\.vcpkg-clion\\vcpkg\\packages\\zlib_x64-mingw-dynamic\\include")
set(BZIP2_LIBRARIES "C:\\Users\\Admin\\.vcpkg-clion\\vcpkg\\packages\\bzip2_x64-mingw-dynamic\\lib")
set(BZIP2_INCLUDE_DIR "C:\\Users\\Admin\\.vcpkg-clion\\vcpkg\\packages\\bzip2_x64-mingw-dynamic\\include")
set(libzip_DIR "C:\\Users\\Admin\\.vcpkg-clion\\vcpkg\\packages\\libzip_x64-mingw-dynamic\\share\\libzip")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)
find_package(libzip CONFIG REQUIRED)

add_executable(Homework1 main.cpp
        CommandHandler.cpp
        CommandHandler.h
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)
target_link_libraries(Homework1
        Qt::Core
        Qt::Gui
        Qt::Widgets
        libzip::zip
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
