#include "sha1.h"
#include <byteswap.h>

namespace sha1 {
	inline unsigned int cyclic_left_rotate(unsigned int data, unsigned int shift_bits) {
        return (data << shift_bits) | (data >> (32 - shift_bits));
    }

    // Save a 32-bit unsigned integer to memory, in big-endian order
    inline void make_big_endian_uint32( unsigned char* byte, unsigned int num ) {
        byte[0] = (unsigned char)(num >> 24);
        byte[1] = (unsigned char)(num >> 16);
        byte[2] = (unsigned char)(num >> 8);
        byte[3] = (unsigned char)num;
    }

    const char* HEX_STRING = "0123456789abcdef";    /* to convert to hex */

    sha1_t::sha1_t() {
        initialise();
    }

    sha1_t::sha1_t(const void* input, const unsigned int input_length, void* signature_) {
        initialise();
        process(input, input_length);
        finish(signature);
    }

    void sha1_t::process(const void* input, int input_length) {
        if (!finished) {
            unsigned int processed = 0;

            if (stored_size and input_length + stored_size >= BLOCK_SIZE) {
                unsigned char block[BLOCK_SIZE];
                memcpy(block, stored, stored_size);
                memcpy(block + stored_size, input, BLOCK_SIZE - stored_size);
                processed = BLOCK_SIZE - stored_size;
                stored_size = 0;
                process_block(block);
            }

            while (processed + BLOCK_SIZE <= input_length) {
                process_block((unsigned char*)input + processed);
                processed += BLOCK_SIZE;
            }

            if (processed < input_length) {
                memcpy(stored + stored_size, (char*)input + processed, input_length - processed);
                stored_size += input_length - processed;
            } else {
                stored_size = 0;
            }
        }
    }

    void sha1_t::finish(void* signature_) {
        if (!finished) {
            if (message_length[0] + stored_size < message_length[0])
                message_length[1]++;
            message_length[0] += stored_size;

            int pad = BLOCK_SIZE - (sizeof(unsigned int) * 2) - stored_size;
            if (pad <= 0)
                pad += BLOCK_SIZE;

            if (pad > 0) {
                stored[stored_size] = 0x80;
                if (pad > 1)
                    memset(stored + stored_size + 1, 0, pad - 1);
                stored_size += pad;
            }

            unsigned int totalBitsH = (message_length[1] << 3) | ((message_length[0] & 0xE0000000) >> 29);
            make_big_endian_uint32(stored + stored_size, totalBitsH);
            stored_size += sizeof(unsigned int);

            unsigned int totalBitsL = ((message_length[0] & 0x1FFFFFFF) << 3);
            make_big_endian_uint32(stored + stored_size, totalBitsL);
            stored_size += sizeof(unsigned int);

            process_block(stored);
            if (stored_size > BLOCK_SIZE)
                process_block(stored + BLOCK_SIZE);

            make_big_endian_uint32(signature,      H0);
            make_big_endian_uint32(signature + 4,  H1);
            make_big_endian_uint32(signature + 8,  H2);
            make_big_endian_uint32(signature + 12, H3);
            make_big_endian_uint32(signature + 16, H4);

            if (signature_ != NULL) {
                memcpy(signature_, signature, SHA1_SIZE);
            }

            sig_to_string(signature, str);

            finished = true;
        }
    }

    void sha1_t::get_sig(void* signature_) {
        if (finished) {
            memcpy(signature_, signature, SHA1_SIZE);
        }
    }

    void sha1_t::get_string(void* str_) {
        if (finished) {
            memcpy(str_, str, SHA1_STRING_SIZE);
        }
    }

    void sha1_t::initialise() {
        H0 = 0x67452301;
        H1 = 0xefcdab89;
        H2 = 0x98badcfe;
        H3 = 0x10325476;
        H4 = 0xc3d2e1f0;

        message_length[0] = 0;
        message_length[1] = 0;
        stored_size = 0;

        finished = false;
    }

    void sha1_t::process_block(const unsigned char* block) {
        if (message_length[0] + BLOCK_SIZE < message_length[0])
            message_length[1]++;
        message_length[0] += BLOCK_SIZE;

        int t;
        unsigned int a, b, c, d, e, K, f, W[80];

        #if SHA1_DEBUG
            std::cout << "BEFORE"
                      << "\nH0: " << H0
                      << "\nH1: " << H1
                      << "\nH2: " << H2
                      << "\nH3: " << H3
                      << "\nH4: " << H4 << std::endl;
        #endif // SHA1_DEBUG

        // starting values
        a = H0;
        b = H1;
        c = H2;
        d = H3;
        e = H4;

        // copy and expand the message block
        for (t = 0; t < 16; t++)
            W[t] = (block[t * 4]     << 24)
                 + (block[t * 4 + 1] << 16)
                 + (block[t * 4 + 2] << 8)
                 +  block[t * 4 + 3];

        for (; t < 80; t++)
            W[t] = cyclic_left_rotate(W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16], 1);

        /* main loop */
        unsigned int temp;
        for (t = 0; t < 80; t++) {
            if (t < 20) {
                K = 0x5a827999;
                f = (b & c) | ((b ^ 0xFFFFFFFF) & d); //TODO: try using ~
            } else if (t < 40) {
                K = 0x6ed9eba1;
                f = b ^ c ^ d;
            } else if (t < 60) {
                K = 0x8f1bbcdc;
                f = (b & c) | (b & d) | (c & d);
            } else {
                K = 0xca62c1d6;
                f = b ^ c ^ d;
            }

            temp = cyclic_left_rotate(a, 5) + f + e + W[t] + K;
            e = d;
            d = c;
            c = cyclic_left_rotate(b, 30);
            b = a;
            a = temp;
        }

        /* add variables */
        H0 += a;
        H1 += b;
        H2 += c;
        H3 += d;
        H4 += e;

        #if SHA1_DEBUG
            std::cout << "AFTER"
                      << "\nH0: " << H0
                      << "\nH1: " << H1
                      << "\nH2: " << H2
                      << "\nH3: " << H3
                      << "\nH4: " << H4 << std::endl;
        #endif // SHA1_DEBUG
    }

    void sig_to_string(const void* signature_, char* str_) {
        unsigned char* sig_p;
        char* str_p;
        char* max_p;
        unsigned int high, low;

        str_p = str_;
        max_p = str_ + SHA1_STRING_SIZE;

        for (sig_p = (unsigned char*)signature_; sig_p < (unsigned char*)signature_ + SHA1_SIZE; sig_p++) {
            high = *sig_p / 16;
            low = *sig_p % 16;
            /* account for 2 chars */
            if (str_p + 1 >= max_p) {
                break;
            }
            *str_p++ = HEX_STRING[high];
            *str_p++ = HEX_STRING[low];
        }
        /* account for 2 chars */
        if (str_p < max_p) {
            *str_p++ = '\0';
        }
    }

    void sig_from_string(void* signature_, const char* str_) {
        unsigned char *sig_p;
        const char *str_p;
        char* hex;
        unsigned int high, low, val;

        hex = (char*)HEX_STRING;
        sig_p = static_cast<unsigned char*>(signature_);

        for (str_p = str_; str_p < str_ + SHA1_SIZE * 2; str_p += 2) {
            high = strchr(hex, *str_p) - hex;
            low = strchr(hex, *(str_p + 1)) - hex;
            val = high * 16 + low;
            *sig_p++ = val;
        }
    }
} // namespace sha1