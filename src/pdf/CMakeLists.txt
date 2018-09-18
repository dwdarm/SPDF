file(GLOB SOURCES "*.cxx")
add_library(pdf ${SOURCES})

target_include_directories(pdf PUBLIC ${POPPLER_INCLUDEDIR} PUBLIC ${POPPLER_INCLUDE_DIRS})
target_compile_options(pdf PUBLIC ${POPPLER_CFLAGS})
target_link_libraries(pdf -L${POPPLER_LIBRARY_DIRS})