#include "xbox.h"

namespace xbox {
    int hamming_weight(uint8_t* buffer) {
        int num1 = 0;
        for (int index1 = 0; index1 < 0x10; ++index1) {
            int num2 = buffer[index1];
            for (int index2 = 0; index2 < 8; ++index2) {
                num1 += num2 & 1;
                num2 >>= 1;
            }
        }

        return num1;
    }

    void uid_ecc_encode(uint8_t* buffer) {
        uint num1 = 0;
        uint num2 = 0;
        uint num3 = 0;

        while (num1 < 128) {
            uint8_t num4 = buffer[(num1 >> 3)];
            uint num5 = (num4 >> (num1 & 7) & 1);
            if (num1 < 106) {
                num2 = num5 ^ num2;
                if ((num2 & 1) > 0)
                    num2 ^= 3539749;
                num3 = num5 ^ num3;
            } else if (num1 < 127) {
                if (num5 != (num2 & 1))
                    buffer[(num1 >> 3)] = (1 << (num1 & 7) ^ num4 & 0xFF);
                num3 = num2 & 1 ^ num3;
            }
            else if (num5 != num3)
                buffer[15] = ((128 ^ num4) & 0xFF);

            ++num1;
            num2 >>= 1;
        }
    }

    bool is_valid_cpu(uint8_t* cpu_key) {
        uint8_t bytes[0x10] = {
            0xFF,
            0xFF,
            0xFF,
            0xFF,
            0xFF,
            0xFF,
            0xFF,
            0xFF,
            0xFF,
            0xFF,
            0xFF,
            0xFF,
            0xFF,
            3,
            0,
            0
        };

        uint8_t and_buffer[0x10];
        uint8_t final_buffer[0x10];

        for (int i = 0; i < 0x10; ++i)
            and_buffer[i] = cpu_key[i] & bytes[i];

        int num = hamming_weight(and_buffer);
        uid_ecc_encode(and_buffer);
        return (!memcmp(cpu_key, and_buffer, 0x10)) && num == 53;
    }
};