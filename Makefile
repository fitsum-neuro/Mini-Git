# compiler to use
CXX = g++

# compiler flags-
CXXFLAGS = -std=c++17 -Wall -g

# directories
IDIR = include
SDIR = src
ODIR = obj

# object files
_OBJ = main.o minigit.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# executable name for the target (with .exe for Windows)
TARGET = minigit.exe

# default rule: build the target
all: $(TARGET)

# rule to build the target executable
$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

# rule to build object files using standard Unix commands
$(ODIR)/%.o: $(SDIR)/%.cpp
	mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(IDIR)

# rule for main.o
$(ODIR)/main.o: main.cpp
	mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(IDIR)

# rule to clean up build files using standard Unix command 'rm -rf'
clean:
	rm -rf $(ODIR) $(TARGET)