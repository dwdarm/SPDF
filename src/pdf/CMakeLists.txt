file(GLOB SOURCES "*.cxx")
add_library(pdf ${SOURCES})

target_compile_options(pdf PUBLIC ${POPPLER_CFLAGS})
