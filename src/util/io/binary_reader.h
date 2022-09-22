#pragma once
#include "stdafx.h"
#include <vector>

namespace io {
    class reader {
    public:
        reader(uint8_t* buffer, uint32_t size, bool little = true)
            : m_raw(buffer), m_size(size), m_little(little)
        {}

        float read_float();
        double read_double();

        short read_int16();
        uint16_t read_uint16();

        int read_int24();
        int read_int32();
        uint32_t read_uint32();

        int64_t read_int64();
        uint64_t read_uint64();

        char read_byte();
        uint8_t read_ubyte();
        bool read_bool();

        const char* read_string(uint32_t size);
        uint8_t* read_bytes(uint32_t size);

        void reverse(uint8_t* buffer, uint32_t size);
        void copy(uint8_t* destination, uint32_t size);
        void release();

        void advance(uint32_t bytes);
        uint32_t get_size() { return m_size; }
        uint32_t get_index() { return m_index; }
        uint32_t get_remaining() { return m_size - m_index; }
    private:
        uint8_t* alloc(uint32_t size);
        bool is_valid(uint32_t size);

        bool m_little {true};
        uint8_t* m_raw = nullptr;
        uint32_t m_size = 0;
        uint32_t m_index = 0;
        std::vector<uint8_t*> m_memory = {};
    };
}