#include "binary_writer.h"
#include <stack>

namespace io {
	uint8_t* writer::alloc(uint32_t size) {
		uint8_t* allocation = (uint8_t*)malloc(size);
		m_memory.push_back(allocation);

		memset(allocation, 0, size);
		return allocation;
	}

	bool writer::is_valid(uint32_t ReadSize) {
		return !((m_index + ReadSize) > m_size);
	}

	void writer::reverse(uint8_t* Buffer, uint32_t size) {
		std::stack<uint8_t> stack;

		for (uint32_t i = 0; i < size; i++)
			stack.push(Buffer[i]);

		int index = 0;
		while (!stack.empty()) {
			Buffer[index++] = stack.top();
			stack.pop();
		}
	}

	void writer::release() {
		for (uint8_t* allocation : m_memory) {
			if (allocation)
				free(allocation);
		}
	}

	void writer::write_double(double value) {
		if (is_valid(sizeof(double))) {
			memcpy((m_raw + m_index), &value, sizeof(double));
			m_index += (uint32_t)sizeof(double);
		}
	}

	void writer::write_int16(short value) {
		if (is_valid(sizeof(short))) {
			memcpy((m_raw + m_index), &value, sizeof(short));
			m_index += (uint32_t)sizeof(short);
		}
	}

	void writer::write_uint16(ushort value) {
		if (is_valid(sizeof(ushort))) {
			memcpy((m_raw + m_index), &value, sizeof(ushort));
			m_index += (uint32_t)sizeof(ushort);
		}
	}

	void writer::write_int32(int value) {
		if (is_valid(sizeof(int))) {
			memcpy((m_raw + m_index), &value, sizeof(int));
			m_index += (uint32_t)sizeof(int);
		}
	}

	void writer::write_uint32(uint32_t value) {
		if (is_valid(sizeof(uint32_t))) {
			memcpy((m_raw + m_index), &value, sizeof(uint32_t));
			m_index += (uint32_t)sizeof(uint32_t);
		}
	}

	void writer::write_int64(int64_t value) {
		if (is_valid(sizeof(int64_t))) {
			memcpy((m_raw + m_index), &value, sizeof(int64_t));
			m_index += (uint32_t)sizeof(int64_t);
		}
	}

	void writer::write_uint64(uint64_t value) {
		if (is_valid(sizeof(uint64_t))) {
			memcpy((m_raw + m_index), &value, sizeof(uint64_t));
			m_index += (uint32_t)sizeof(uint64_t);
		}
	}

	void writer::write_float(float value) {
		if (is_valid(sizeof(float))) {
			memcpy((m_raw + m_index), &value, sizeof(float));
			m_index += (uint32_t)sizeof(float);
		}
	}

	void writer::write_byte(uint8_t value) {
		if (is_valid(sizeof(uint8_t))) {
			*(uint8_t*)(m_raw + m_index) = value;
			m_index += (uint32_t)sizeof(uint8_t);
		}
	}

	void writer::write_bytes(uint8_t* value, uint32_t size) {
		if (is_valid(size)) {
			memcpy(m_raw + m_index, value, size);
			m_index += size;
		}
	}

	void writer::write_bytes_index(uint8_t* value, uint32_t size, uint32_t offset) {
		memcpy(m_raw + offset, value, size);
	}
}