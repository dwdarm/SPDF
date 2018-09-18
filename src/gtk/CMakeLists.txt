file(GLOB SOURCES "*.cxx")
add_library(gtk ${SOURCES})

target_compile_options(gtk PUBLIC ${GTKMM_CFLAGS})