#include "challenges.h"
#include "xbox.h"
#include "core.h"
#include "util/util.h"
#include "util/dirs.h"
#include "util/io/binary_reader.h"
#include "crypto/sha1.h"
#include <byteswap.h>

namespace challenges {
    uint32_t get_max_encrypted_hv_size() {
        uint32_t size = 0;

        for (int i = 0; i < NUMOF(g_hv_ecc_ranges); i++) {
            if (std::get<0>(g_hv_ecc_ranges[i]) == HV_ENC) {
                uint32_t total = std::get<1>(g_hv_ecc_ranges[i]) + std::get<2>(g_hv_ecc_ranges[i]);
                if (total > size)
                    size = total;
            }
        }

        return size;
    }

    bool calculate_ecc_digest(uint16_t salt_checksum, std::string seed, uint8_t* output) {
        uint8_t* cache {nullptr};
        uint32_t cache_size {0};

        if (!g_dirs->read_binary_contents("assets/seeds/" + seed + "/hv_cache.bin", (uint8_t**)&cache, &cache_size))
            return false;

        uint8_t* encrypted {nullptr};
        uint32_t encrypted_size {0};

        if (!g_dirs->read_binary_contents("assets/seeds/" + seed + "/hv_encrypted.bin", (uint8_t**)&encrypted, &encrypted_size, get_max_encrypted_hv_size())) {
            delete[] cache;
            return false;
        }

        sha1::sha1_t state;
        state.process((void*)&salt_checksum, sizeof(salt_checksum));

        for (int i = 0; i < NUMOF(g_hv_ecc_ranges); i++) {
            switch (std::get<0>(g_hv_ecc_ranges[i])) {
                case HV_ENC:
                    state.process(&encrypted[std::get<1>(g_hv_ecc_ranges[i])], std::get<2>(g_hv_ecc_ranges[i]));
                    break;

                case HV_DEC:
                    state.process(&g_core->m_hv_decrypted.data()[std::get<1>(g_hv_ecc_ranges[i])], std::get<2>(g_hv_ecc_ranges[i]));
                    break;

                case HV_CACHE:
                    state.process(&cache[std::get<1>(g_hv_ecc_ranges[i])], std::get<2>(g_hv_ecc_ranges[i]));
                    break;
            }
        }
        
        state.finish(output);

        delete[] encrypted;
        delete[] cache;
        return true;
    }

    void calculate_hv_digest(uint8_t* salt, uint8_t* output) {
        sha1::sha1_t state;
        state.process(salt, 0x10);

        for (int i = 0; i < NUMOF(g_hv_digest_ranges); i++)
            state.process(&g_core->m_hv_decrypted.data()[g_hv_digest_ranges[i].first], g_hv_digest_ranges[i].second);

        state.finish(output);
    }

    uint8_t* calculate_rsa_sha_salted_hash(uint8_t* salt, int salt_len, uint8_t* rsa, int rsa_len) {
        int increment = 0;
        for (int i = 0; i < rsa_len; i += 0x14) {
            uint8_t inc_block[] = { 0, 0, 0, (uint8_t)increment++ }; 
            uint8_t hash[0x14];

            sha1::sha1_t state;
            state.process(salt, salt_len);
            state.process(inc_block, sizeof(inc_block));
            state.finish(hash);

            for (int j = 0; j < ((i + 0x14 > rsa_len) ? rsa_len - i : 0x14); j++)
                rsa[i + j] ^= hash[j];
        }

        return rsa;
    }

    void calculate_rsa_memory_key(hv_encryption_keys* keys, uint8_t* output) {
        memset(output, 0, 0x80);

        uint8_t random[0x14];
        memset(random, 0, 0x14); random[0] = 1;
        // util::generate_random_bytes(&output[1], 0x14);

        uint8_t data[] = { 0xDA, 0x39, 0xA3, 0xEE, 0x5E, 0x6B, 0x4B, 0x0D, 0x32, 0x55, 0xBF, 0xEF, 0x95, 0x60, 0x18, 0x90, 0xAF, 0xD8, 0x07, 0x09 }; // 0x15FB0
        memcpy(&output[0x1], random, sizeof(random));
        memcpy(&output[0x15], data, sizeof(data));
        memcpy(&output[0x50], keys, sizeof(hv_encryption_keys));

        output[0x4F] = 1;

        uint8_t temp[0x6B];
        memcpy(temp, &output[0x15], sizeof(temp));
        memcpy(&output[0x15], calculate_rsa_sha_salted_hash(random, sizeof(random), temp, sizeof(temp)), sizeof(temp));
        
        memcpy(temp, &output[0x15], sizeof(temp));
        memcpy(&output[0x1], calculate_rsa_sha_salted_hash(temp, sizeof(temp), random, sizeof(random)), sizeof(random));
    }

    void calculate_rsa_digest(hv_encryption_keys* keys, uint8_t* out) {
        calculate_rsa_memory_key(keys, out);

        io::reader tmp(nullptr, 0);
        tmp.reverse(out, 0x80);

        util::bswap(out, 0x80, 0x40);
        // todo: rsa
        util::bswap(out, 0x80, 0x40);
    }

    bool handle_hv_challenge(hv_challenge params, hv_challenge_buffer* output) {
        // check that the console cpu is valid
        if (!xbox::is_valid_cpu(params.m_console_cpu)) {
            printf("invalid console cpu\n");
            return false;
        }

        // check that the kv cpu is valid
        if (!xbox::is_valid_cpu(params.m_kv_cpu)) {
            printf("invalid kv cpu\n");
            return false;
        }

        // check that the salt exists
        if (!g_core->m_salts[salt_cache(params.m_salt)]) {
            printf("invalid salt\n");
            return false;
        }

        // get a random seed and read it
        std::string seed_file = g_core->m_seeds[util::get_random_int(0, (int)g_core->m_seeds.size() - 1)];

        seed cur_seed {};
        seed* temp_seed_block {nullptr};
        uint32_t seed_block_size {0};

        if (!g_dirs->read_binary_contents("assets/seeds/" + seed_file + "/salts/0x" + util::convert_bytes_to_string(params.m_salt, sizeof(params.m_salt)) + ".bin", (uint8_t**)&temp_seed_block, &seed_block_size)) {
            printf("failed reading salt\n");
            return false;
        }

        memcpy(&cur_seed, temp_seed_block, sizeof(cur_seed));
        delete[] temp_seed_block;

        if (memcmp(cur_seed.m_salt.m_salt, params.m_salt, sizeof(params.m_salt)))
            return false;

        // setup the basic hv response data
        output->m_hv_magic = bswap_16(0x4E4E);
        output->m_hv_version = bswap_16(17559);
        output->m_bldr_flags = bswap_16(0xD83E);
        output->m_base_kernel_version = bswap_32(0x7600000);
        output->m_update_sequence = bswap_32(0x8988CC09);
        output->m_console_type_seq_allow = bswap_32(0x304000E);
        output->m_rtoc = bswap_64(0x200000000);
        output->m_hrmor = bswap_64(0x10000000000);
        output->m_console_type_seq_allow = bswap_32(0x0304000E); // usually checks for type one?
        output->m_hv_exec_address = cur_seed.m_hv_exec_address;
        output->m_hv_keys_status_flags = params.m_fcrt ? bswap_32(0x033289D3) : bswap_32(0x023289D3);
        if (params.m_crl) output->m_hv_keys_status_flags = params.m_fcrt ? bswap_32(0x033389D3) : bswap_32(0x023389D3);

        // hash the kv cpu
        sha1::sha1_t kv_cpu_state;
        kv_cpu_state.process(params.m_kv_cpu, sizeof(params.m_kv_cpu));
        kv_cpu_state.finish(output->m_cpu_key_digest);

        // calculate rsa
        hv_encryption_keys cur_keys {};
        hv_encryption_keys* temp_enc_keys {nullptr};
        uint32_t enc_key_size {0};

        if (!g_dirs->read_binary_contents("assets/seeds/" + seed_file + "/key.bin", (uint8_t**)&temp_enc_keys, &enc_key_size))
            return false;

        memcpy(&cur_keys, temp_enc_keys, sizeof(cur_keys));
        delete[] temp_enc_keys;

        // TODO: rsa

        // calculate the hv digest
        uint8_t hv_digest[0x14];
        calculate_hv_digest(params.m_salt, hv_digest);;

        memcpy(output->m_hv_digest, &hv_digest[0xE], sizeof(output->m_hv_digest));

        // calculate the ecc digest
        if (!calculate_ecc_digest(cur_seed.m_ecc_salt, seed_file, output->m_hv_ecc_digest))
            return false;

        return true;
    }

    bool handle_supervisor_challenge(supervisor_challenge params, supervisor_challenge_buffer* output) {

        return true;
    }
};