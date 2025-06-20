# compiler to use
CXX = g++

# compiler flags-
CXXFLAGS = -std=c++17 -Wall -g

# directories
IDIR = include
SDIR = src
ODIR = obj

# object files
_OBJ = main.o minigit.o log.o branch.o checkout.o merge.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# executable name for the target
TARGET = minigit

# default rule: build the target
all: $(TARGET)

# rule to build the target executable
$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) -lstdc++fs

# rule to build object files using standard Unix commands
$(ODIR)/%.o: $(SDIR)/%.cpp
	mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(IDIR)

$(ODIR)/log.o: src/log.cpp
	mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(IDIR)

$(ODIR)/branch.o: src/branch.cpp
	mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(IDIR)

$(ODIR)/checkout.o: src/checkout.cpp
	mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(IDIR)

$(ODIR)/merge.o: src/merge.cpp
	mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(IDIR)

# rule for main.o
$(ODIR)/main.o: main.cpp
	mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(IDIR)

# rule to clean up build files using standard Unix command 'rm -rf'
clean:
	rm -rf $(ODIR) $(TARGET)