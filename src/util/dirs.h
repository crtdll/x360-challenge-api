#pragma once
#include "stdafx.h"
#include <vector>

class dirs {
public:
    bool load();
    bool read_binary_contents(std::string file, uint8_t** buffer, uint32_t* size, uint32_t max_size = 0);
   
    std::vector<std::string> get_files_in_directory(std::string path);
    std::vector<std::string> get_folders_in_directory(std::string path);

public:
    std::string m_error_reason {""};
};

inline std::unique_ptr<dirs> g_dirs {};