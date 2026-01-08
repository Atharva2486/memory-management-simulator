#include "physical/pm_allocator.h"
#include <iomanip>
#include <bits/stdc++.h>

using namespace std;
using ll = long long;

static bool isPowerOfTwo(ll x) {
    return x && !(x & (x - 1));
}

PhysicalMemoryAllocator::PhysicalMemoryAllocator(ll size)
    : total_size(size),
      next_block_id(1),
      allocator(AllocatorType::LINEAR),
      fit(FitStrategy::FIRST),
      total_requested(0),
      total_allocated(0),
      alloc_requests(0),
      alloc_failures(0)
{
    blocks.push_back({0, size, true, -1, 0});

    if (isPowerOfTwo(size)) {
        max_order = log2(size);
        free_lists.clear();
        free_lists.resize(max_order + 1);

        free_lists[max_order].push_back(0);
    }

}

void PhysicalMemoryAllocator::setAllocatorType(AllocatorType t) {
    allocator = t;
}

void PhysicalMemoryAllocator::setFitStrategy(FitStrategy f) {
    fit = f;
}

list<Block>::iterator
PhysicalMemoryAllocator::findBlock(ll size) {
    list<Block>::iterator chosen = blocks.end();

    if (fit == FitStrategy::FIRST) {
        for (auto it = blocks.begin(); it != blocks.end(); ++it)
            if (it->free && it->size >= size) return it;
    }
    else if (fit == FitStrategy::BEST) {
        ll best = LLONG_MAX;
        for (auto it = blocks.begin(); it != blocks.end(); ++it)
            if (it->free && it->size >= size && it->size < best)
                best = it->size, chosen = it;
    }
    else {
        ll worst = -1;
        for (auto it = blocks.begin(); it != blocks.end(); ++it)
            if (it->free && it->size >= size && it->size > worst)
                worst = it->size, chosen = it;
    }
    return chosen;
}

void PhysicalMemoryAllocator::coalesceLinear() {
    for (auto it = blocks.begin(); it != blocks.end(); ) {
        auto nxt = next(it);
        if (nxt != blocks.end() && it->free && nxt->free) {
            it->size += nxt->size;
            blocks.erase(nxt);
        } else ++it;
    }
}
long long PhysicalMemoryAllocator::nextPowerOfTwo(long long x) const {
    long long p = 1;
    while (p < x) {
        p <<= 1;
    }
    return p;
}

ll PhysicalMemoryAllocator::getOrder(ll size) const {
    return log2(size);
}

ll PhysicalMemoryAllocator::buddyOf(ll addr, ll order) const {
    return addr ^ (1LL << order);
}

ll PhysicalMemoryAllocator::pm_malloc(ll size) {
    alloc_requests++;
    total_requested += size;

    if (allocator == AllocatorType::LINEAR) {
        auto it = findBlock(size);
        if (it == blocks.end()) {
            alloc_failures++;
            return -1;
        }

        if (it->size > size) {
            blocks.insert(next(it),
                {it->start + size, it->size - size, true, -1,0});
            it->size = size;
        }

        it->free = false;
        it->id = next_block_id++;
        it->requested = size;
        total_allocated += it->size;
        return it->id;
    }

    if (!isPowerOfTwo(total_size)) {
        cout << "ERROR: Buddy allocator requires power-of-two memory\n";
        return -1;
    }

    ll req = nextPowerOfTwo(size);
    ll order = getOrder(req);
    ll cur = order;

    while (cur <= max_order && free_lists[cur].empty())
        cur++;

    if (cur > max_order) {
        alloc_failures++;
        return -1;
    }

    ll addr = free_lists[cur].front();
    free_lists[cur].pop_front();

    while (cur > order) {
        cur--;
        ll right = addr + (1LL << cur);
        free_lists[cur].push_back(right);
    }

    allocated[addr] = order;
    total_allocated += (1LL << order);

    requested_sizes[addr] = size;
    total_requested += size;


    return addr;

}

void PhysicalMemoryAllocator::pm_free(ll id_or_address) {
    if (allocator == AllocatorType::LINEAR) {
        for (auto& b : blocks) {
            if (!b.free && (b.id == id_or_address || b.start == id_or_address)) {
                b.free = true;
                b.id = -1;
                b.requested = 0; 
                coalesceLinear();
                return;
            }
        }
        return;
    }

    auto it = allocated.find(id_or_address);
    if (it == allocated.end())
        return; 

    ll addr  = it->first; 
    ll order = it->second;
    allocated.erase(it);

    while (order < max_order) {
        ll buddy = addr ^ (1LL << order);
        auto &fl = free_lists[order];

        auto bit = find(fl.begin(), fl.end(), buddy);
        if (bit == fl.end())break;
        fl.erase(bit);
        addr = min(addr, buddy);
        order++;
    }
    requested_sizes.erase(addr);
    free_lists[order].push_back(addr);
}

void PhysicalMemoryAllocator::dump() const {
    cout << "[Physical Memory Dump]\n";

    if (allocator == AllocatorType::LINEAR) {
        for (const auto& b : blocks) {
            cout << "["
                      << b.start << " - "
                      << (b.start + b.size - 1) << "] ";

            if (b.free) {
                cout << "FREE  (id=-)";
            } else {
                cout << "USED  (id=" << b.id << ")";
            }
            cout << "\n";
        }
    } 
    else { 
        cout << "Buddy Free Lists:\n";
        for (ll i = 0; i <= max_order; i++) {
            cout << "Order " << i
                      << " (size=" << (1LL << i) << "): ";
            for (auto addr : free_lists[i]) {
                cout << addr << " ";
            }
            cout << "\n";
        }
    }
}

void PhysicalMemoryAllocator::stats() const {
    ll internal_frag = 0;
    ll external_frag = 0;
    ll free_memory = 0;
    ll largest_free = 0;

    if (allocator == AllocatorType::LINEAR) {

        for (const auto &b : blocks) {
            if (b.free) {
                free_memory += b.size;
                largest_free = max(largest_free, b.size);
            } else {
                internal_frag += (b.size - b.requested);
            }
        }
    }

    else {
        for (const auto &p : allocated) {
            ll order = p.second;
            ll block_size = (1LL << order);

            auto rit = requested_sizes.find(p.first);
            if (rit != requested_sizes.end()) {
                internal_frag += (block_size - rit->second);
            }
        }

        for (ll i = 0; i <= max_order; i++) {
            ll block_size = (1LL << i);
            ll count = free_lists[i].size();

            free_memory += count * block_size;
            if (count > 0)
                largest_free = max(largest_free, block_size);
        }
    }

    if (free_memory == 0)
        largest_free = 0;

    external_frag = free_memory - largest_free;
    if (external_frag < 0)
        external_frag = 0;

    double internal_pct =
        (double)internal_frag * 100.0 / total_size;

    double external_pct =
        (double)external_frag * 100.0 / total_size;

    double util_pct =
        (double)total_requested * 100.0 / total_size;

    cout << "[Physical Memory Statistics]\n\n";

    cout << "Total Memory           : "
              << total_size << " bytes\n\n";

    cout << "Requested Memory       : "
              << total_requested << " bytes\n";

    cout << "Allocated Memory       : "
              << total_allocated << " bytes\n";

    cout << "Free Memory            : "
              << free_memory << " bytes\n\n";

    cout << "Internal Fragmentation :\n";
    cout << "  - " << internal_frag << " bytes ("
              << fixed << setprecision(2)
              << internal_pct << "%)\n\n";

    cout << "External Fragmentation :\n";
    cout << "  - " << external_frag << " bytes ("
              << fixed << setprecision(2)
              << external_pct << "%)\n\n";

    // cout << "Memory Utilization     :\n";
    // cout << "  - " << util_pct << "%\n";
              util_pct++;
}

