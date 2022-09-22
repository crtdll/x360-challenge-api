#include "dirs.h"
#include "core/core.h"
#include "util/io/binary_reader.h"

std::vector<std::string> dirs::get_files_in_directory(std::string path) {
    std::vector<std::string> files {};

    DIR* dir {nullptr};
    dirent* ent {nullptr};

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr)
            if (ent->d_type == DT_REG)
                if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
                    files.push_back(ent->d_name);

        closedir(dir);
    }

    return files;
}

std::vector<std::string> dirs::get_folders_in_directory(std::string path) {
    std::vector<std::string> files {};

    DIR* dir {nullptr};
    dirent* ent {nullptr};

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr)
            if (ent->d_type == DT_DIR)
                if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
                    files.push_back(ent->d_name);

        closedir(dir);
    }

    return files;
}

bool dirs::read_binary_contents(std::string filename, uint8_t** buffer, uint32_t* size, uint32_t max_size) {
    bool ret = false;

    FILE* file = fopen(filename.c_str(), "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        *size = (uint32_t)ftell(file);
        fseek(file, 0, SEEK_SET);

        if (max_size > 0 && *size > max_size)
            *size = max_size;

        *buffer = new uint8_t[*size];
        UNUSED_RETURN(fread(*buffer, 1, *size, file));
        fclose(file);
        return true;
    }

    return false;
}

bool dirs::load() {
    // get seed folders
    g_core->m_seeds = get_folders_in_directory("assets/seeds/");

    if (g_core->m_seeds.empty()) {
        m_error_reason = "No seeds found in assets folder";
        return false;
    }

    // load salts
    uint8_t* salts {nullptr};
    uint32_t salt_size {0};

    if (!read_binary_contents("assets/hv/hv_salts.bin", &salts, &salt_size)) {
        m_error_reason = "Couldn't read hv salts";
        return false;
    }

    for (uint32_t i = 0; i < salt_size; i += 0x10) {
        salt_cache cache(&salts[i]);
        g_core->m_salts[cache] = true; // just has a bool for now
    } delete[] salts;

    // load public rsa key
    uint8_t* rsa_key {nullptr};
    uint32_t rsa_key_size {0};

    if (!read_binary_contents("assets/hv/hv_public.bin", &rsa_key, &rsa_key_size)) {
        m_error_reason = "Couldn't read hv public rsa";
        return false;
    }

    // parse the rsa key
    io::reader rsa_reader(rsa_key, rsa_key_size, false);
    g_core->m_hv_public_rsa.m_rsa.m_modulus = rsa_reader.read_uint32();
    g_core->m_hv_public_rsa.m_rsa.m_exponent = rsa_reader.read_uint32();
    g_core->m_hv_public_rsa.m_rsa.m_reserved = rsa_reader.read_uint64();

    for (int i = 0; i < 0x10; i++)
        g_core->m_hv_public_rsa.m_sequence[i] = rsa_reader.read_uint64();
        
    rsa_reader.release();

    // load decrypted hv
    uint8_t* dec_hv {nullptr};
    uint32_t dec_hv_size {0};

    if (!read_binary_contents("assets/hv/hv_decrypted.bin", &dec_hv, &dec_hv_size)) {
        m_error_reason = "Couldn't read decrypted hv";
        return false;
    }

    for (uint32_t i = 0; i < dec_hv_size; i++)
        g_core->m_hv_decrypted.push_back(dec_hv[i]);

    return true;
}