#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H
#include <bits/stdc++.h>
using namespace std;
#include <string>

class PhysicalMemoryAllocator;
class VirtualMemoryManager;
class CacheHierarchy;

class CommandParser {
public:
    void dispatch(
        const string& line,
        PhysicalMemoryAllocator* pm,
        VirtualMemoryManager* vm,
        CacheHierarchy* cache
    );
};

#endif
