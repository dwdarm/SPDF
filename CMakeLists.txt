cmake_minimum_required(VERSION 3.5)

project(SPDF)

set (CMAKE_CXX_STANDARD 11)

find_package (Threads)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
pkg_check_modules(POPPLER REQUIRED poppler)
pkg_check_modules(DDJVUAPI REQUIRED ddjvuapi)
pkg_check_modules(JSON-GLIB REQUIRED json-glib-1.0)

subdirs(src)
