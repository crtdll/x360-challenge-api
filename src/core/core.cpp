#include "core.h"
#include "util/dirs.h"
#include "util/io/binary_reader.h"
#include <byteswap.h>

#include "challenges.h"

bool core::load() {
    g_dirs = std::make_unique<dirs>();

    if (!g_dirs->load()) {
        m_error_reason = g_dirs->m_error_reason;
        return false;
    }

    unsigned char rawData[51] = {
        0x1E, 0xA2, 0x01, 0xFD, 0xC7, 0x4B, 0x14, 0xE2, 0xCA, 0x0D, 0x44, 0xC8,
        0xEB, 0x66, 0x1F, 0xED, 0x07, 0x2D, 0xDD, 0xDB, 0x2E, 0xC8, 0x6C, 0xBD,
        0x19, 0x43, 0x60, 0x2B, 0x1C, 0xE7, 0x66, 0x10, 0xA9, 0x72, 0xEF, 0x19,
        0x88, 0x5F, 0x13, 0x21, 0xF8, 0x96, 0x3C, 0x5A, 0x97, 0xE8, 0xA5, 0x1C,
        0x00, 0x01, 0x00
    };

    challenges::hv_challenge challenge_params;
    memcpy(&challenge_params, rawData, sizeof(rawData));

    hv_challenge_buffer* output = new hv_challenge_buffer();
    if (!challenges::handle_hv_challenge(challenge_params, output))
        printf("err\n");

    uint8_t* yeet = (uint8_t*)output;

    for (int i = 0; i < sizeof(hv_challenge_buffer); i++)
        printf("%02x ", yeet[i]);

    delete[] output;
    return true;
}