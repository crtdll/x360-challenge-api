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
    
    return true;
}