#pragma once
#include "stdafx.h"

namespace util {
    void generate_random_bytes(uint8_t* buffer, uint32_t length);
    int get_random_int(int from, int to);
    std::string convert_bytes_to_string(uint8_t* buffer, uint32_t length);
};