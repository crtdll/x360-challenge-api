#include "stdafx.h"
#include "core/core.h"
#include "core/xbox.h"
#include <thread>

int main(int argc, char** argv) {
    g_core = std::make_unique<core>();

    if (!g_core->load()) {
        std::cout << "[Core] Failed: " << g_core->m_error_reason << std::endl;
        return 1;
    }

    // todo: obviously setup sockets, handle clients, etc

    // TESTING

    // todo: load static data and compare against C#

    // TESTING

    return 0;
}