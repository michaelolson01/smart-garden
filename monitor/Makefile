# === Monitor Program Configuration ===
TARGET     = garden_monitor
SRC        = garden_monitor.cpp
CXX        = g++
CXXFLAGS   = -Wall -Wextra -pedantic -std=c++17
LDFLAGS    = -lncurses -ltinfo

.PHONY: all clean run

# Default target
all: $(TARGET)

# Build the monitor
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Run the monitor
run: $(TARGET)
	./$(TARGET)

# Remove built files
clean:
	rm -f $(TARGET)
