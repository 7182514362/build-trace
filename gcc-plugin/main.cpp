#include <fstream>
#include <string>
#include <vector>

#include <gcc-plugin.h>
#include <plugin.h>
#include <plugin-version.h>

#include <tree.h>
// #include <tree-core.h>

#include <cgraph.h>
#include <symtab.h>

// #include <tree-pass.h>
// #include <context.h>
#include "Global.h"
#include "DBUtil.h"
#include "Log.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "SymbolRefTable.h"
#include "FileManager.h"

#include <chrono>

using namespace std;

int plugin_is_GPL_compatible;
extern symbol_table *symtab;
static SymbolTable gSymbolTable;

static SymbolRefTable gSymRefTable;

static int gCmdId = -1;

static std::string gWorkDir;
static FileManager gFileMgr;

Symbol *CreateSymbol(symtab_node *node) {
    if (node == nullptr || node->decl == nullptr) {
        return nullptr;
    }
    Symbol *sym = new Symbol();
    if (sym == nullptr) {
        LOG_ERROR << "Failed to create symbol";
        return nullptr;
    }
    sym->isBuiltin = DECL_IS_BUILTIN(node->decl);
    if (!sym->isBuiltin) {
        const char *file = DECL_SOURCE_FILE(node->decl);
        if (file != nullptr) {
            sym->file_id = gFileMgr.Insert(file);
        }
        sym->line = DECL_SOURCE_LINE(node->decl);
    } else {
        sym->file_id = gFileMgr.Insert("<built-in>");
    }
    sym->asmname = node->asm_name();
    sym->name = Symbol::Demangle(sym->asmname);
    sym->id = DECL_UID(node->decl);
    sym->isDef = node->definition;
    sym->type = node->type;
    return sym;
}

void BeginAnalysis(void *event, void *user_data) {
    LOG_INFO << "BeginAnalysis";
    symtab_node *node;
    FOR_EACH_SYMBOL(node) {
        Symbol *sym = CreateSymbol(node);
        if (sym == nullptr) {
            continue;
        }
        gSymbolTable.Insert(sym);
        if (node->type == symtab_type::SYMTAB_FUNCTION) {
            cgraph_node *cn = dyn_cast<cgraph_node *>(node);
            if (cn == nullptr) {
                continue;
            }
            cgraph_edge *edge = cn->callees;
            for (cgraph_edge *edge = cn->callees; edge != nullptr; edge = edge->next_callee) {
                auto *caller = edge->caller;
                auto *callee = edge->callee;
                if (caller == nullptr || callee == nullptr) {
                    continue;
                }
                gSymRefTable.Insert(DECL_UID(caller->decl), DECL_UID(callee->decl));
            }
        }
    }
}

void EndAnalysis(void *event, void *user_data) {
    LOG_INFO << "EndAnalysis";
    std::string dbPath = Global::GetInstance().GetDBPath();
    dbPath += "/" + std::to_string(gCmdId) + "_" + std::to_string(getpid()) + ".sqlite";
    DBUtil db;
    bool success = db.Connect(dbPath);
    if (!success) {
        return;
    }

    if (!db.CreateSymbolTbl()) {
        return;
    }
    if (!db.CreateSymbolRefTbl()) {
        return;
    }
    if (!db.CreateFileTbl()) {
        return;
    }
    if (!db.InsertSymbol(gSymbolTable)) {
        return;
    }
    if (!db.InsertSymbolRef(gSymRefTable)) {
        return;
    }
    if (!db.InsertFile(gFileMgr.GetData())) {
        return;
    }
}

void FileIncluded(void *event, void *user_data) {
    if (event != nullptr) {
        std::string_view file = (char *)event;
        gFileMgr.Insert(file);
    }
}

bool Init(struct plugin_name_args *plugin_info) {
    const char *key = "cmdid";
    for (int i = 0; i < plugin_info->argc; ++i) {
        if (strncmp(plugin_info->argv[i].key, key, strlen(key)) == 0) {
            gCmdId = std::atoi(plugin_info->argv[i].value);
        }
    }
    auto &global = Global::GetInstance();
    if (!global.Init(gCmdId)) {
        return false;
    }
    Log::Config(global.GetLogPath());

    gFileMgr.Insert(global.GetCWD());
    return true;
}

int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version) {
    if (!plugin_default_version_check(version, &gcc_version)) {
        LOG_ERROR << "Incompatible gcc plugin versions";
        return 1;
    }

    if (!Init(plugin_info)) {
        return 0;
    }

    const char *const plugin_name = plugin_info->base_name;
    register_callback(plugin_name, PLUGIN_INCLUDE_FILE, FileIncluded, NULL);
    register_callback(plugin_name, PLUGIN_ALL_IPA_PASSES_START, BeginAnalysis, NULL);
    register_callback(plugin_name, PLUGIN_ALL_IPA_PASSES_END, EndAnalysis, NULL);

    return 0;
}