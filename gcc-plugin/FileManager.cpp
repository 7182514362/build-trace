#include "FileManager.h"

int FileManager::Insert(std::string_view file) {
    auto itor = m_fileIdMap.find(file);
    if (itor != m_fileIdMap.end()) {
        return itor->second;
    }
    m_data.push_back(file);
    int id = m_data.size() - 1;
    m_fileIdMap.insert({file, id});
    return id;
}