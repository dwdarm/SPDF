subdirs(gtk pdf djvu)

file(GLOB SOURCES "*.cxx")
add_executable(SPDF ${SOURCES})

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
