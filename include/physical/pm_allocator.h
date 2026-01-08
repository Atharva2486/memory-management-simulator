#ifndef PM_ALLOCATOR_H
#define PM_ALLOCATOR_H
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

enum class AllocatorType {
    LINEAR,
    BUDDY
};

enum class FitStrategy {
    FIRST,
    BEST,
    WORST
};

struct Block {
    ll start;
    ll size;
    bool free;
    ll id;
    ll requested;
};

class PhysicalMemoryAllocator {
private:
    
    ll total_size;
    ll next_block_id;
    AllocatorType allocator;

    list<Block> blocks;
    FitStrategy fit;

    ll max_order;
    vector<list<ll>> free_lists;
    map<ll, ll> allocated; 

    ll total_requested;
    ll total_allocated;
    ll alloc_requests;
    ll alloc_failures;
    unordered_map<ll, ll> requested_sizes;

    list<Block>::iterator findBlock(ll size);
    void coalesceLinear();

    ll nextPowerOfTwo(ll x) const;
    ll getOrder(ll size) const;
    ll buddyOf(ll addr, ll order) const;

public:
    explicit PhysicalMemoryAllocator(ll size);

    void setAllocatorType(AllocatorType t);
    void setFitStrategy(FitStrategy f);

    ll pm_malloc(ll size);
    void pm_free(ll id_or_address);

    void dump() const;
    void stats() const;
};

#endif
