file(GLOB SOURCES "*.cxx")
add_library(djvu ${SOURCES})

target_compile_options(djvu PUBLIC ${DDJVUAPI_CFLAGS})