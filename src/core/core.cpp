#include "core.h"
#include "util/dirs.h"

bool core::load() {
    g_dirs = std::make_unique<dirs>();

    if (!g_dirs->load()) {
        m_error_reason = g_dirs->m_error_reason;
        return false;
    }
    
    return true;
}