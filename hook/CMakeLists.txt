set(CMAKE_BUILD_TYPE Debug)
add_library(hook SHARED main.c trace.c db_util.c util.c log.c)
target_link_libraries(hook dl sqlite3)
redefine_file_macro(hook)

# add_link_options("--version-script /home/daniel/Workspace/playground/build-trace/hook/symbol.version")
# SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--version-script,/home/daniel/Workspace/playground/build-trace/hook/symbol.version")