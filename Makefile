# Compiler to use
CXX = g++

# Compiler flags
# -std=c++17 is for modern C++ features like <filesystem>
# -Wall enables all warnings
# -g adds debugging information
CXXFLAGS = -std=c++17 -Wall -g

# directories
IDIR = include
SDIR = src

# object files will be put in an 'obj' directory
ODIR = obj
_OBJ = main.o minigit.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# executable name
TARGET = minigit

# default rule: build the target
all: $(TARGET)

# how to build the target executable
$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

# hule to build object files
$(ODIR)/%.o: $(SDIR)/%.cpp
	@mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(IDIR)

# how for main.o, which isn't in src/
$(ODIR)/main.o: main.cpp
	@mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(IDIR)

# hule to clean up build files
clean:
	rm -rf $(ODIR) $(TARGET)