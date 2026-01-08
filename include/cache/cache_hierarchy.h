#ifndef CACHE_HIERARCHY_H
#define CACHE_HIERARCHY_H
#include <bits/stdc++.h>
using ll = long long;
using namespace std;
enum class CachePolicy {
    FIFO,
    LRU
};

struct CacheLine {
    bool valid;
    ll tag;
    ll last_used;   
};

struct CacheSet {
    vector<CacheLine> lines;
    ll fifo_ptr;
};

class CacheLevel {
private:
    ll cache_size;
    ll block_size;
    ll associativity;
    ll num_sets;

    vector<CacheSet> sets;

    CachePolicy policy;
    ll time;   

    ll hits;
    ll misses;

    ll getSetIndex(ll phys_addr) const;
    ll getTag(ll phys_addr) const;

public:
    CacheLevel(ll size, ll block, ll assoc);

    void setPolicy(CachePolicy p);
    bool access(ll phys_addr);
    void stats(const char* name) const;
};

class CacheHierarchy {
private:
    CacheLevel l1;
    CacheLevel l2;
    CachePolicy policy;

public:
    CacheHierarchy(
        ll l1_size, ll l1_block, ll l1_assoc,
        ll l2_size, ll l2_block, ll l2_assoc
    );

    void setPolicy(CachePolicy p);
    void access(ll phys_addr, bool verbose);
    void stats() const;
};

#endif
