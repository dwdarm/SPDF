subdirs(gtk pdf djvu)

set (SPDF_VERSION_MAJOR 0)
set (SPDF_VERSION_MINOR 1)

configure_file (
    "config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

include_directories(${PROJECT_BINARY_DIR})

file(GLOB SOURCES "*.cxx")
add_executable(SPDF ${SOURCES})

target_compile_options(SPDF PUBLIC ${JSON-GLIB_CFLAGS})
target_include_directories(SPDF PUBLIC gtk)
target_include_directories(SPDF PUBLIC pdf)
target_include_directories(SPDF PUBLIC djvu)

target_link_libraries(SPDF gtk)
target_link_libraries(SPDF pdf)
target_link_libraries(SPDF djvu)
target_link_libraries(SPDF ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(SPDF ${POPPLER_LIBRARIES})
target_link_libraries(SPDF ${DDJVUAPI_LIBRARIES})
target_link_libraries(SPDF ${GTKMM_LIBRARIES})
target_link_libraries(SPDF ${JSON-GLIB_LIBRARIES})
