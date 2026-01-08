#include "virtual_memory/vm_manager.h"
#include <bits/stdc++.h>

using namespace std;
using ll = long long; 

VirtualMemoryManager::VirtualMemoryManager(ll ps, ll nf)
    : page_size(ps),
      num_frames(nf),
      frames(nf),
      time(0),
      policy(PagePolicy::FIFO),
      next_pid(1),
      page_faults(0),
      page_hits(0),
      disk_reads(0),
      disk_writes(0)
{
    for (auto &f : frames)
        f.occupied = false;
}

void VirtualMemoryManager::setPagePolicy(PagePolicy p) {
    policy = p;
}

sim_pid_t VirtualMemoryManager::vm_create(ll process_size) {
    Process p;
    p.pid = next_pid++;
    p.size = process_size;
    p.num_pages = (process_size + page_size - 1) / page_size;

    p.page_table.resize(p.num_pages);
    for (auto &pte : p.page_table) {
        pte.resident = false;
        pte.frame = -1;
        pte.last_used = 0;
    }

    processes[p.pid] = p;

    cout << "Process created: PID=" << p.pid
              << " Pages=" << p.num_pages << "\n";
    return p.pid;
}

ll VirtualMemoryManager::findFreeFrame() {
    for (ll i = 0; i < num_frames; i++)
        if (!frames[i].occupied)
            return i;
    return -1;
}

ll VirtualMemoryManager::selectVictimFrame() {
    // FIFO
    if (policy == PagePolicy::FIFO) {
        ll f = fifo_queue.front();
        fifo_queue.pop();
        return f;
    }

    // LRU
    ll victim = -1;
    ll oldest = LLONG_MAX;

    for (ll i = 0; i < num_frames; i++) {
        if (frames[i].occupied) {
            Process &p = processes[frames[i].pid];
            PageTableEntry &pte =
                p.page_table[frames[i].page];

            if (pte.last_used < oldest) {
                oldest = pte.last_used;
                victim = i;
            }
        }
    }
    return victim;
}

ll VirtualMemoryManager::vm_access(sim_pid_t pid, ll vaddr) {
    if (processes.find(pid) == processes.end()) {
        cout << "SEGMENTATION FAULT: Invalid PID\n";
        return -1;
    }

    Process &p = processes[pid];

    if (vaddr < 0 || vaddr >= p.size) {
        cout << "SEGMENTATION FAULT: Invalid address\n";
        return -1;
    }

    time++;

    ll page = vaddr / page_size;
    ll offset = vaddr % page_size;
    PageTableEntry &pte = p.page_table[page];

    if (pte.resident) {
        page_hits++;
        pte.last_used = time;
        ll phys = pte.frame * page_size + offset;
        cout << "Page hit → PA=" << phys << "\n";
        return phys;
    }

    page_faults++;
    disk_reads++;

    ll frame = findFreeFrame();
    if (frame == -1) {
        frame = selectVictimFrame();
        Frame &vf = frames[frame];
        Process &vp = processes[vf.pid];

        vp.page_table[vf.page].resident = false;
        vp.page_table[vf.page].frame = -1;

        disk_writes++;
    }

    frames[frame].occupied = true;
    frames[frame].pid = pid;
    frames[frame].page = page;

    if (policy == PagePolicy::FIFO)
        fifo_queue.push(frame);

    pte.resident = true;
    pte.frame = frame;
    pte.last_used = time;

    ll phys = frame * page_size + offset;
    cout << "Page fault → Frame " << frame
              << " PA=" << phys << "\n";

    return phys;
}

void VirtualMemoryManager::dumpPageTable(sim_pid_t pid) const {
    auto it = processes.find(pid);
    if (it == processes.end()) {
        cout << "ERROR: Invalid PID\n";
        return;
    }

    const Process &p = it->second;

    cout << "[VM Page Table]\n";
    cout << "PID: " << pid << "\n";
    cout << "---------------------------------\n";
    cout << "Page | Resident | Frame\n";
    cout << "---------------------------------\n";

    for (ll i = 0; i < p.num_pages; i++) {
        const auto &pte = p.page_table[i];
        cout << i << "    | "
                  << (pte.resident ? "YES" : "NO ") << "      | ";
        if (pte.resident)
            cout << pte.frame;
        else
            cout << "-";
        cout << "\n";
    }
}


void VirtualMemoryManager::dumpFrames() const {
    cout << "[VM Frame Table]\n";
    cout << "---------------------------------\n";
    cout << "Frame | PID | Page\n";
    cout << "---------------------------------\n";

    for (ll i = 0; i < num_frames; i++) {
        cout << i << "     | ";
        if (frames[i].occupied) {
            cout << frames[i].pid << "   | "
                      << frames[i].page;
        } else {
            cout << "-   | -";
        }
        cout << "\n";
    }
}


void VirtualMemoryManager::stats() const {
    cout << "[VM Stats]\n";
    cout << "Policy: "
              << (policy == PagePolicy::FIFO ? "FIFO" : "LRU") << "\n";
    cout << "Page hits: " << page_hits << "\n";
    cout << "Page faults: " << page_faults << "\n";
    cout << "Disk reads: " << disk_reads << "\n";
    cout << "Disk writes: " << disk_writes << "\n";
    cout << "External fragmentation: 0\n";
}
