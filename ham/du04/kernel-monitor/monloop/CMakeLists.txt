# Find kernel headers
find_package(KernelHeaders REQUIRED)

include_directories(${KERNELHEADERS_INCLUDE_DIRS})

file(GLOB monLoopSources src/*.c)
get_target_property(monlibSources monlib SOURCES)
get_target_property(monlibIncludes monlib INCLUDE_DIRECTORIES)
list(APPEND monLoopSources ${monlibSources})

foreach(src ${monLoopSources})
    get_filename_component(src_name "${src}" NAME)
    list(APPEND monlibSourcesName ${src_name})
endforeach()

# Construct the compiler string for the include directories.
foreach(dir ${monlibIncludes})
    string(APPEND module_includes_string "-I${dir} ")
endforeach()

string(REPLACE ";" " " module_sources_string "${monlibSourcesName}")
configure_file(Kbuild.in Kbuild @ONLY)

file(GLOB monLoopHeaders src/*.h)
list(APPEND monLoopSources ${monLoopHeaders})
foreach(src ${monLoopSources})
    get_filename_component(src_name "${src}" NAME)
    configure_file("${src}" "${src_name}" COPYONLY)
endforeach()

set(module_cmd ${CMAKE_MAKE_PROGRAM} -C ${KERNELHEADERS_DIR} M=${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(OUTPUT monloop.ko
  COMMAND ${module_cmd} modules
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${module_sources} ${CMAKE_CURRENT_BINARY_DIR}/Kbuild
  VERBATIM)
add_custom_target(module DEPENDS monloop.ko)
add_custom_target(module-clean COMMAND ${module_cmd} clean)


if(DUMMY_MODULE_EXEC MATCHES ON)
    add_executable(DUMMY ${monLoopSources})
    target_link_libraries(DUMMY PRIVATE monlib)
    target_compile_definitions(DUMMY PUBLIC __KERNEL__)
    target_compile_definitions(DUMMY PUBLIC KBUILD_MODNAME="")
endif()

