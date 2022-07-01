EXECUTE_PROCESS(
    COMMAND gcc-9 -print-file-name=plugin
    TIMEOUT 5
    OUTPUT_VARIABLE GCC_PLUGIN_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
MESSAGE(STATUS "GCC_PLUGIN_PATH: ${GCC_PLUGIN_PATH}")
add_library(ssa SHARED main.cpp Symbol.cpp SymbolTable.cpp SymbolRefTable.cpp FileManager.cpp DBUtil.cpp Log.cpp Global.cpp)

target_include_directories(ssa PUBLIC "${GCC_PLUGIN_PATH}/include")
target_link_libraries(ssa sqlite3)
redefine_file_macro(ssa)