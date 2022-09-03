EXECUTE_PROCESS(
    COMMAND gcc-9 -print-file-name=plugin
    TIMEOUT 5
    OUTPUT_VARIABLE GCC_PLUGIN_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
MESSAGE(STATUS "GCC_PLUGIN_PATH: ${GCC_PLUGIN_PATH}")
set(TARGET ssagcc)
add_library(${TARGET} SHARED main.cpp Symbol.cpp SymbolTable.cpp SymbolRefTable.cpp FileManager.cpp DBUtil.cpp Log.cpp Global.cpp)
set_target_properties(${TARGET} PROPERTIES PREFIX "")

target_include_directories(${TARGET} PUBLIC "${GCC_PLUGIN_PATH}/include")
target_link_libraries(${TARGET} sqlite3)
redefine_file_macro(${TARGET})