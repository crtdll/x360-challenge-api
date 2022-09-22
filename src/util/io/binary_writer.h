#pragma once
#include "stdafx.h"
#include <vector>

namespace io {
	class writer {
	public:
		writer(uint8_t* buffer, uint32_t size)
			: m_raw(buffer), m_size(size) {}

		void reverse(uint8_t* buffer, uint32_t size);
		void copy(uint8_t* destination, uint32_t size);
		void release();

		void write_double(double value);
		void write_int16(short value);
		void write_uint16(ushort value);
		void write_int32(int value);
		void write_uint32(uint32_t value);
		void write_int64(int64_t value);
		void write_uint64(uint64_t value);
		void write_float(float value);
		void write_byte(uint8_t value);
		void write_bytes(uint8_t* value, uint32_t size);
		void write_bytes_index(uint8_t* value, uint32_t size, uint32_t offset);

		uint32_t add_padding(uint32_t size) {
			uint32_t current_index = m_index;
			memset(m_raw + m_index, 0, size);
			m_index += size;
			return current_index;
		}

		uint8_t* get_buffer() { return m_raw; }
		uint32_t get_size() { return m_size; }
		uint32_t get_index() { return m_index; }
		uint32_t get_remaining() { return m_size - m_index; }
	private:
		uint8_t* alloc(uint32_t size);
		bool is_valid(uint32_t size);

		uint8_t* m_raw = nullptr;
		uint32_t m_size = 0;
		uint32_t m_index = 0;
		std::vector<uint8_t*> m_memory = {};
	};
}