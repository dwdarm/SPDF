subdirs(gtk pdf djvu)

include_directories(${SPDF_SOURCE_DIR}/gtk)
include_directories(${SPDF_SOURCE_DIR}/pdf)
include_directories(${SPDF_SOURCE_DIR}/djvu)

link_directories(${SPDF_SOURCE_DIR}/gtk)
link_directories(${SPDF_SOURCE_DIR}/pdf)
link_directories(${SPDF_SOURCE_DIR}/djvu)

file(GLOB SOURCES "*.cxx")
add_executable(SPDF ${SOURCES})

target_link_libraries(SPDF gtk)
target_link_libraries(SPDF pdf)
target_link_libraries(SPDF djvu)
target_link_libraries(SPDF ${CMAKE_THREAD_LIBS_INIT})


target_include_directories(SPDF PUBLIC ${POPPLER_INCLUDE_DIRS})
target_compile_options(SPDF PUBLIC ${POPPLER_CFLAGS_OTHER})
target_link_libraries(SPDF ${POPPLER_LIBRARIES})

target_include_directories(SPDF PUBLIC ${DDJVUAPI_INCLUDE_DIRS})
target_compile_options(SPDF PUBLIC ${DDJVUAPI_CFLAGS_OTHER})
target_link_libraries(SPDF ${DDJVUAPI_LIBRARIES})

target_include_directories(SPDF PUBLIC ${GTKMM_INCLUDE_DIRS})
target_compile_options(SPDF PUBLIC ${GTKMM_CFLAGS_OTHER})
target_link_libraries(SPDF ${GTKMM_LIBRARIES})
