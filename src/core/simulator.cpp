#include "core/simulator.h"
#include "core/command_parser.h"
#include <bits/stdc++.h>

#include "physical/pm_allocator.h"
#include "virtual_memory/vm_manager.h"
#include "cache/cache_hierarchy.h"

using namespace std;
using ll = long long;

Simulator::Simulator() : pm(nullptr), vm(nullptr), cache(nullptr) {}

Simulator::~Simulator() {
    delete pm;
    delete vm;
    delete cache;
}

bool Simulator::isInitialized() const {
    return config.initialized;
}

void Simulator::handleInit() {
    if (config.initialized) {
        cout << "ERROR: Simulator already initialized\n";
        return;
    }

    cout << "Enter physical memory size (bytes): ";
    cin >> config.physical_memory_size;

    cout << "Enter page size (bytes): ";
    cin >> config.page_size;

    if (config.physical_memory_size % config.page_size != 0) {
        cout << "ERROR: Physical memory must be multiple of page size\n";
        return;
    }

    config.num_frames = config.physical_memory_size / config.page_size;

    cout << "Enter L1 cache size: ";
    cin >> config.l1_size;
    cout << "Enter L1 block size: ";
    cin >> config.l1_block;
    cout << "Enter L1 associativity: ";
    cin >> config.l1_assoc;

    cout << "Enter L2 cache size: ";
    cin >> config.l2_size;
    cout << "Enter L2 block size: ";
    cin >> config.l2_block;
    cout << "Enter L2 associativity: ";
    cin >> config.l2_assoc;

    pm = new PhysicalMemoryAllocator(config.physical_memory_size);
    vm = new VirtualMemoryManager(config.page_size, config.num_frames);
    cache = new CacheHierarchy(
        config.l1_size, config.l1_block, config.l1_assoc,
        config.l2_size, config.l2_block, config.l2_assoc
    );

    config.initialized = true;
    cout << "Simulator initialized successfully.\n";
}

void Simulator::run() {
    CommandParser parser;

    while (true) {
        cout << "~> ";
        string line;
        getline(cin >> ws, line);

        if (line == "exit") break;

        if (!config.initialized && line != "init") {
            cout << "ERROR: Simulator not initialized\n";
            continue;
        }

        if (line == "init") {
            handleInit();
            continue;
        }

        parser.dispatch(line, pm, vm, cache);
    }
}
