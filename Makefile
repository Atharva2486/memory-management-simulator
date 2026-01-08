CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

INCLUDE = -Iinclude

SRC = main.cpp \
      src/core/simulator.cpp \
      src/core/command_parser.cpp \
      src/physical/pm_allocator.cpp \
      src/virtual_memory/vm_manager.cpp \
      src/cache/cache_hierarchy.cpp

TARGET = mmsim

all:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
