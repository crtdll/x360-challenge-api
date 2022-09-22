#pragma once
#include "stdafx.h"
#include <vector>
#include <unordered_map>

enum eHvType { HV_ENC, HV_DEC, HV_CACHE };

static std::pair<uint32_t, uint32_t> g_hv_digest_ranges[] = {
    { 0x34, 0x40 },
    { 0x78, 0xFF88 },
    { 0x100C0, 0x40 },
    { 0x10350, 0x5F70 },
    { 0x16EA0, 0x9160 },
    { 0x20000, 0xFFFF },
    { 0x30000, 0xFFFF }
};

static std::tuple<eHvType, uint32_t, uint32_t> g_hv_ecc_ranges[] = {
    { HV_DEC, 0x34, 0xC },
    { HV_ENC, 0x40, 0x30 },
    { HV_DEC, 0x70, 0x4 },
    { HV_DEC, 0x78, 0x8 },
    { HV_CACHE, 0x2, 0x3FE },
    { HV_ENC, 0x100C0, 0x40 },
    { HV_ENC, 0x10350, 0x30 },
    { HV_CACHE, 0x40E, 0x17C },
    { HV_ENC, 0x16280, 0x40 },
    { HV_ENC, 0x16EA0, 0x60 },
    { HV_CACHE, 0x5BC, 0x244 },
    { HV_CACHE, 0x800, 0x400 },
    { HV_CACHE, 0xC00, 0x400 }
};

struct salt_cache {
    uint8_t m_salt[0x10];

    salt_cache() {
        memset(m_salt, 0, sizeof(m_salt));
    };

    salt_cache(uint8_t* salt) {
        memcpy(m_salt, salt, sizeof(m_salt));  
    };

    bool operator==(const salt_cache& other) const {
        return !memcmp(m_salt, other.m_salt, sizeof(m_salt));
    };
};

struct salt_cache_hasher {
    std::size_t operator()(const salt_cache& cache) const {
        return std::hash<uint64_t>()(*(uint64_t*)&cache.m_salt[0])
            ^ (std::hash<uint64_t>()(*(uint64_t*)&cache.m_salt[0x8]) << 1);
    };
};

struct seed {
    salt_cache m_salt {};
    uint8_t _0x0010[0x10];
    uint16_t m_ecc_salt;
    char _0x0022[0xE];
    uint16_t m_hv_exec_address;
    char _0x32[0xE];
};

struct public_rsa {
    uint32_t m_modulus {0};
    uint32_t m_exponent {0};
    uint64_t m_reserved {0};
};

struct public_rsa_1024 {
    public_rsa m_rsa {};
    uint64_t m_sequence[0x10];
};

struct public_rsa_2048 {
    public_rsa m_rsa {};
    uint64_t m_sequence[0x20];
};

struct hv_encryption_keys {
    uint8_t m_whitening[0x10];
    uint8_t m_aes[0x10];
    uint8_t m_hashing[0x10];
};

struct hv_challenge_header {
    uint16_t m_magic {0};
    uint16_t m_build {0};
    uint16_t m_qfe {0};
    uint16_t m_flags {0};
    uint32_t m_entry {0};
    uint32_t m_size {0};
    uint8_t m_nonce[0x10];
};

struct hv_challenge_buffer {
    hv_challenge_header m_header {};
    uint8_t m_reserved[0x8];
    uint16_t m_hv_magic {0};
    uint16_t m_hv_version {0};
    uint16_t m_hv_qfe {0};
    uint16_t m_bldr_flags {0};
    uint32_t m_base_kernel_version {0};
    uint32_t m_update_sequence {0};
    uint32_t m_hv_keys_status_flags {0};
    uint32_t m_console_type_seq_allow {0};
    uint64_t m_rtoc {0};
    uint64_t m_hrmor {0};
    uint8_t m_hv_ecc_digest[0x14];
    uint8_t m_cpu_key_digest[0x14];
    uint64_t m_rsa_sequence[0x10];
    uint16_t m_hv_exec_address {0};
    uint8_t m_hv_digest[0x6];
};

struct supervisor_challenge_buffer {
    uint8_t m_buffer[0x2C8];
};

class core {
public:
    bool load();

public:
    std::vector<std::string> m_seeds {};
    std::vector<uint8_t> m_hv_decrypted {};
    std::unordered_map<salt_cache, bool, salt_cache_hasher> m_salts {};
    public_rsa_1024 m_hv_public_rsa {};
    std::string m_error_reason {""};
};

inline std::unique_ptr<core> g_core {};