#include "util.h"
#include "io/binary_reader.h"
#include <random>

namespace util {
    void generate_random_bytes(uint8_t* buffer, uint32_t length) {
		std::random_device r;
		std::seed_seq seed2 { r(), r(), r(), r(), r(), r(), r(), r() };
		std::mt19937 rng(seed2);
		std::uniform_int_distribution<int> val(0, 255);

		for (uint32_t i = 0; i < length; i++) {
			buffer[i] = (uint8_t)val(rng);
		}
	}

    int get_random_int(int from, int to) {
        std::random_device r;
		std::seed_seq seed2 { r(), r(), r(), r(), r(), r(), r(), r() };
		std::mt19937 rng(seed2);
		std::uniform_int_distribution<int> val(from, to);
        return val(rng);
    }

    std::string convert_bytes_to_string(uint8_t* buffer, uint32_t length) {
		std::stringstream str;
		str.setf(std::ios_base::hex, std::ios::basefield);
		str.setf(std::ios_base::uppercase);
		str.fill('0');

		for (size_t i = 0; i < length; ++i) {
			str << std::setw(2) << (unsigned short)buffer[i];
		}

		return str.str();
	}

    uint8_t* bswap(uint8_t* data, int size, int bit_count) {
        int count = bit_count / 8;
        uint8_t* tmp = new uint8_t[size];

        for (int i = 0; i < size; i += count) {
            uint8_t* num_array = new uint8_t[count];
            memcpy(num_array, &data[i], count);
            
            io::reader yeet(nullptr, 0);
            yeet.reverse(num_array, count);

            memcpy(&tmp[i], num_array, count);
            delete[] num_array;
        }

        memcpy(data, tmp, size);
        delete[] tmp;
    }
};