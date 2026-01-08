#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

class PhysicalMemoryAllocator;
class VirtualMemoryManager;
class CacheHierarchy;

struct SimulatorConfig {
    ll physical_memory_size = 0;
    ll page_size = 0;
    ll num_frames = 0;

    ll l1_size = 0;
    ll l1_block = 0;
    ll l1_assoc = 0;

    ll l2_size = 0;
    ll l2_block = 0;
    ll l2_assoc = 0;

    bool initialized = false;
};

class Simulator {
private:
    SimulatorConfig config;

    PhysicalMemoryAllocator* pm;
    VirtualMemoryManager* vm;
    CacheHierarchy* cache;

    void handleInit();

public:
    Simulator();
    ~Simulator();

    void run();
    bool isInitialized() const;
};

#endif
