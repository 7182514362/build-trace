#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <unordered_map>
#include <string_view>
#include <vector>

class FileManager {
public:
    int Insert(std::string_view file);

    std::string_view GetFile(int id) {
        if (m_data.size() <= id) {
            return "";
        }
        return m_data[id];
    }

    std::vector<std::string_view> &GetData() {
        return m_data;
    }

private:
    std::vector<std::string_view> m_data;
    std::unordered_map<std::string_view, int> m_fileIdMap;
};

#endif