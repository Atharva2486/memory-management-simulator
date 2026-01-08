#include "core/command_parser.h"
#include <sstream>
#include <bits/stdc++.h>
#include "physical/pm_allocator.h"
#include "virtual_memory/vm_manager.h"
#include "cache/cache_hierarchy.h"
using namespace std;
using ll = long long;

void CommandParser::dispatch(
    const string& line,
    PhysicalMemoryAllocator* pm,
    VirtualMemoryManager* vm,
    CacheHierarchy* cache
) {
    stringstream ss(line);
    string cmd;
    ss >> cmd;

    if (cmd == "pm") {
        string sub;
        ss >> sub;

        if (sub == "alloc") {
            long long bytes;
            ss >> bytes;
            long long res = pm->pm_malloc(bytes);
            if (res == -1) {
                cout << "PM allocation failed\n";
            } else {
                cout << "Allocated block\n";
                cout << "  ID / Address: " << res << "\n";
                cout << "  Size: " << bytes << " bytes\n";
            }
        }
        else if (sub == "free") {
            string type;
            ll val;
            ss >> type >> val;

            if (type == "id" || type == "addr") {
                pm->pm_free(val);
                cout << "PM free requested\n";
            } else {
                cout << "ERROR: use pm free id <id> | pm free addr <addr>\n";
            }
        }
        else if (sub == "dump") {
            pm->dump();
        }
        else if (sub == "stats") {
            pm->stats();
        }
        else if (sub == "set") {
            string what, how;
            ss >> what >> how;

            if (what == "allocator") {
                if (how == "linear") {
                    pm->setAllocatorType(AllocatorType::LINEAR);
                    cout << "PM allocator set to LINEAR\n";
                } else if (how == "buddy") {
                    pm->setAllocatorType(AllocatorType::BUDDY);
                    cout << "PM allocator set to BUDDY\n";
                } else {
                    cout << "ERROR: allocator must be linear|buddy\n";
                }
            }
            else if (what == "fit") {
                if (how == "first") {
                    pm->setFitStrategy(FitStrategy::FIRST);
                    cout << "PM fit strategy set to FIRST\n";
                } else if (how == "best") {
                    pm->setFitStrategy(FitStrategy::BEST);
                    cout << "PM fit strategy set to BEST\n";
                } else if (how == "worst") {
                    pm->setFitStrategy(FitStrategy::WORST);
                    cout << "PM fit strategy set to WORST\n";
                } else {
                    cout << "ERROR: fit must be first|best|worst\n";
                }
            }
            
            else {
                cout << "ERROR: Invalid pm set command\n";
            }
        }else if (sub == "help") {
            cout << "PM Commands:\n"
                      << "  pm alloc <bytes>\n"
                      << "  pm free id <id>\n"
                      << "  pm free addr <address>\n"
                      << "  pm dump\n"
                      << "  pm stats\n"
                      << "  pm set allocator linear|buddy\n"
                      << "  pm set fit first|best|worst\n";
        }
        else {
            cout << "ERROR: Unknown pm command\n";
        }
    }
    else if (cmd == "vm") {
        string sub;
        ss >> sub;

        if (sub == "create") {
            long long sz;
            ss >> sz;
            vm->vm_create(sz);
        }
        else if (sub == "access") {
            long long pid;
            string addr;
            ss >> pid >> addr;
            long long vaddr = stoll(addr, nullptr, 16);
            long long pa = vm->vm_access(pid, vaddr);
            if (pa != -1)
                cache->access(pa,true);
        }
        else if (sub == "dump") {
            string what;
            ss >> what;
            if (what == "pagetable") {
                long long pid;
                ss >> pid;
                vm->dumpPageTable(pid);
            } else if (what == "frames") {
                vm->dumpFrames();
            }
        }
        else if (sub == "stats") {
            vm->stats();
        }
        else if (sub == "set") {
            string what, how;
            ss >> what >> how;
            if (what == "page_policy") {
                if (how == "fifo") {
                    vm->setPagePolicy(PagePolicy::FIFO);
                    cout << "VM page policy set to FIFO\n";
                }
                else if (how == "lru") {
                    vm->setPagePolicy(PagePolicy::LRU);
                    cout << "VM page policy set to LRU\n";
                }
                else {
                    cout << "ERROR: page_policy must be fifo|lru\n";
                }
            }
        }else if (sub=="help"){
            cout << "VM Commands:\n"
                      << "  vm create <bytes>\n"
                      << "  vm access <pid> <hex_addr>\n"
                      << "  vm dump pagetable <pid>\n"
                      << "  vm dump frames\n"
                      << "  vm set page_policy fifo|lru\n"
                      << "  vm stats\n";
            
        }
        else {
            cout << "ERROR: Unknown vm command\n";
        }
    }
    else if (cmd == "cache_stats") {
        cache->stats();
    }
    else if (cmd == "help") {
        cout << "Global Commands:\n"
                  << "  pm help\n"
                  << "  vm help\n"
                  << "  vm_create <bytes>\n"
                  << "  cache help\n"
     << "  cache set policy fifo|lru\n"
                  << "  vm_access <pid> <hex_addr>\n"
                  << "  vm_dump_pagetable <pid>\n"
                  << "  vm_dump_frames\n"
                  << "  vm_stats\n"
                  << "  cache_stats\n"
                  << "  exit\n";
    }
    else if (cmd == "cache") {
    string sub;
    ss >> sub;

    if (sub == "set") {
        string what, how;
        ss >> what >> how;

        if (what == "policy") {
            if (how == "fifo") {
                cache->setPolicy(CachePolicy::FIFO);
                cout << "Cache policy set to FIFO\n";
            }
            else if (how == "lru") {
                cache->setPolicy(CachePolicy::LRU);
                cout << "Cache policy set to LRU\n";
            }
            else {
                cout << "ERROR: cache policy must be fifo|lru\n";
            }
        }
        else {
            cout << "ERROR: Unknown cache set command\n";
        }
    }
    else if (sub == "help") {
        cout << "Cache Commands:\n"
             << "  cache set policy fifo|lru\n"
             << "  cache_stats\n";
    }
    else {
        cout << "ERROR: Unknown cache command\n";
    }
}
    else {
        cout << "ERROR: Unknown command\n";
    }
}
