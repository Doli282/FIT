
file(GLOB monlibSources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(monlib ${monlibSources})

target_include_directories(monlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
