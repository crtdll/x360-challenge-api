#pragma once
#include "stdafx.h"
#include "core.h"

namespace challenges {
    struct hv_challenge {
        uint8_t m_salt[0x10];
        uint8_t m_kv_cpu[0x10];
        uint8_t m_console_cpu[0x10];
        bool m_type_one;
        bool m_fcrt;
        bool m_crl;
    };

    struct supervisor_challenge {
        int64_t m_hv_protected_flags;
        uint8_t m_motherboard_index;
        uint8_t m_xeika_certificate_inquiry_data[0x24];
        uint8_t m_console_serial_number[0xC];
        uint8_t m_console_certificate_abdata[0x5];
        uint8_t m_cpu_key_digest[0x10];
        uint16_t m_odd_features;
        uint32_t m_kv_policy_flash_size;
        uint32_t m_title_id;
        uint32_t m_media_id;
        bool m_type_one;
        bool m_fcrt;
        uint8_t m_buffer[0x2C8];
    };

    bool handle_hv_challenge(hv_challenge params, hv_challenge_buffer* output);
    bool handle_supervisor_challenge(supervisor_challenge params, supervisor_challenge_buffer* output);

    // testing
    bool calculate_ecc_digest(uint16_t salt_checksum, std::string seed, uint8_t** output);
    bool calculate_hv_digest(uint8_t* salt, uint8_t** output);
};