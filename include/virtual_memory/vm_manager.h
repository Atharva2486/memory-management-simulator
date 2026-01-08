#ifndef VM_MANAGER_H
#define VM_MANAGER_H
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

using sim_pid_t = long long;

enum class PagePolicy {
    FIFO,
    LRU
};

struct PageTableEntry {
    bool resident;
    ll frame;
    ll last_used;   
};

struct Process {
    sim_pid_t pid;
    ll size;
    ll num_pages;
    vector<PageTableEntry> page_table;
};

struct Frame {
    bool occupied;
    sim_pid_t pid;
    ll page;
};

class VirtualMemoryManager {
private:
    ll page_size;
    ll num_frames;

    vector<Frame> frames;

    queue<ll> fifo_queue;

    ll time;

    PagePolicy policy;

    unordered_map<sim_pid_t, Process> processes;
    sim_pid_t next_pid;

    ll page_faults;
    ll page_hits;
    ll disk_reads;
    ll disk_writes;

    ll findFreeFrame();
    ll selectVictimFrame();

public:
    VirtualMemoryManager(ll page_size, ll num_frames);

    void setPagePolicy(PagePolicy p);

    sim_pid_t vm_create(ll process_size);
    ll vm_access(sim_pid_t pid, ll virtual_address);

    void dumpPageTable(sim_pid_t pid) const;
    void dumpFrames() const;
    void stats() const;
};

#endif
