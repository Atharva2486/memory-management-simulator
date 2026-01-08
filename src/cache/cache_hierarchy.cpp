#include "cache/cache_hierarchy.h"
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

CacheLevel::CacheLevel(ll size, ll block, ll assoc)
    : cache_size(size),
      block_size(block),
      associativity(assoc),
      policy(CachePolicy::FIFO),
      time(0),
      hits(0),
      misses(0)
{
    ll num_blocks = cache_size / block_size;

    if (associativity > num_blocks) {
        associativity = num_blocks;
    }

    num_sets = num_blocks / associativity;

    if (num_sets == 0) {
        num_sets = 1;
    }


    sets.resize(num_sets);
    for (auto &set : sets) {
        set.fifo_ptr = 0;
        set.lines.resize(associativity);
        for (auto &line : set.lines) {
            line.valid = false;
            line.tag = -1;
            line.last_used = 0;
        }
    }
}

void CacheLevel::setPolicy(CachePolicy p) {
    policy = p;
}

ll CacheLevel::getSetIndex(ll phys_addr) const {
    ll block_addr = phys_addr / block_size;
    return block_addr % num_sets;
}

ll CacheLevel::getTag(ll phys_addr) const {
    ll block_addr = phys_addr / block_size;
    return block_addr / num_sets;
}

bool CacheLevel::access(ll phys_addr) {
    time++;

    ll set_idx = getSetIndex(phys_addr);
    ll tag = getTag(phys_addr);

    CacheSet &set = sets[set_idx];

    for (auto &line : set.lines) {
        if (line.valid && line.tag == tag) {
            hits++;
            if (policy == CachePolicy::LRU)
                line.last_used = time;
            return true;
        }
    }

    misses++;

    ll victim = 0;

    if (policy == CachePolicy::FIFO) {
        victim = set.fifo_ptr;
        set.fifo_ptr = (set.fifo_ptr + 1) % associativity;
    }
    else { 
        ll oldest = LLONG_MAX;
        for (ll i = 0; i < associativity; i++) {
            if (!set.lines[i].valid) {
                victim = i;
                break;
            }
            if (set.lines[i].last_used < oldest) {
                oldest = set.lines[i].last_used;
                victim = i;
            }
        }
    }

    set.lines[victim].valid = true;
    set.lines[victim].tag = tag;
    set.lines[victim].last_used = time;

    return false;
}

void CacheLevel::stats(const char* name) const {
    ll total = hits + misses;
    double hit_ratio = total ? (double)hits / total : 0.0;

    cout << "[" << name << "]\n";
    cout << "Hits: " << hits << "\n";
    cout << "Misses: " << misses << "\n";
    cout << "Hit ratio: " << hit_ratio * 100 << "%\n";
}


CacheHierarchy::CacheHierarchy(
    ll l1_size, ll l1_block, ll l1_assoc,
    ll l2_size, ll l2_block, ll l2_assoc
)
    : l1(l1_size, l1_block, l1_assoc),
      l2(l2_size, l2_block, l2_assoc),
      policy(CachePolicy::FIFO)
{}

void CacheHierarchy::setPolicy(CachePolicy p) {
    policy = p;
    l1.setPolicy(p);
    l2.setPolicy(p);
}

void CacheHierarchy::access(ll phys_addr, bool verbose) {
    bool l1_hit = l1.access(phys_addr);

    if (l1_hit) {
        if (verbose)
            cout << "Cache: L1 HIT\n";
        return;
    }

    bool l2_hit = l2.access(phys_addr);

    if (l2_hit) {
        if (verbose)
            cout << "Cache: L1 MISS → L2 HIT\n";
        return;
    }

    if (verbose)
        cout << "Cache: L1 MISS → L2 MISS → MEMORY\n";
}

void CacheHierarchy::stats() const {
    l1.stats("L1 Cache");
    l2.stats("L2 Cache");
}
