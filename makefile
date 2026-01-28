# Qt GUI build and run (combined)
.PHONY: gui

gui:
	cd gui/ChaoticPlants && mkdir -p build && cd build && rm -f CMakeCache.txt && rm -rf CMakeFiles && cmake .. && $(MAKE) && ./ChaoticPlants
.PHONY: cmake_test


# Makefile for COS214_Project_2025

CXX = g++
CXXFLAGS = -g -std=c++17 -Wall -Wextra $(QT_INCLUDES) $(QT_DEFINES)   # ← added Qt include paths and defines

# Dynamic Qt5 configuration using pkg-config (more portable)
QT_INCLUDES = $(shell pkg-config --cflags Qt5Core Qt5Gui Qt5Widgets 2>/dev/null || echo "-I/usr/include/qt5 -I/usr/include/qt5/QtCore -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtWidgets")
QT_DEFINES = -DQT_CORE_LIB -DQT_GUI_LIB -DQT_WIDGETS_LIB
QT_LIBS = $(shell pkg-config --libs Qt5Core Qt5Gui Qt5Widgets 2>/dev/null || echo "-lQt5Core -lQt5Gui -lQt5Widgets")
CXXFLAGS = -g -std=c++17 -Wall -Wextra $(QT_INCLUDES)
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
MOC = moc-qt5


# Find all source files in src subfolders
SRC_DIRS = src/Greenhouse src/Staff src/Customer src/Greenhouse/Nursery
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
# Exclude test files and GUI-specific files
SRCS := $(filter-out %_test.cpp %Test.cpp, $(SRCS))
SRCS := $(filter-out src/Greenhouse/Nursery/mainwindow.cpp, $(SRCS))  # ← EXCLUDE mainwindow.cpp
SRCS := $(filter-out src/Greenhouse/Nursery/main.cpp, $(SRCS))  # ← EXCLUDE GUI main.cpp

OBJS = $(SRCS:.cpp=.o)


# Main target for TestingMain.cpp
TARGET = TestingMain
DEMO_TARGET = DemoMain

all: $(TARGET)

# MOC files needed for Qt classes
MOC_SRCS = src/Customer/moc_Customer.cpp src/Customer/moc_Browse.cpp src/Greenhouse/Nursery/moc_Nursery.cpp src/Greenhouse/Nursery/moc_Clock.cpp \
           src/Greenhouse/Nursery/moc_CustomerClock.cpp src/Greenhouse/Nursery/moc_PlantClock.cpp src/Greenhouse/Nursery/moc_SeasonClock.cpp
MOC_OBJS = $(MOC_SRCS:.cpp=.o)

# For TestingMain, we need to run MOC on Qt classes first, then compile and link
$(TARGET): TestingMain.cpp $(OBJS) $(MOC_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(QT_LIBS)

# Generate MOC files for Qt classes
src/Customer/moc_Customer.cpp: src/Customer/Customer.h
	$(MOC) $(QT_INCLUDES) $< -o $@

src/Customer/moc_Browse.cpp: src/Customer/Browse.h
	moc-qt5 $(QT_INCLUDES) $< -o $@

src/Greenhouse/Nursery/moc_Nursery.cpp: src/Greenhouse/Nursery/Nursery.h
	$(MOC) $(QT_INCLUDES) $< -o $@

src/Greenhouse/Nursery/moc_Clock.cpp: src/Greenhouse/Nursery/Clock.h
	$(MOC) $(QT_INCLUDES) $< -o $@

src/Greenhouse/Nursery/moc_CustomerClock.cpp: src/Greenhouse/Nursery/CustomerClock.h
	$(MOC) $(QT_INCLUDES) $< -o $@

src/Greenhouse/Nursery/moc_PlantClock.cpp: src/Greenhouse/Nursery/PlantClock.h
	$(MOC) $(QT_INCLUDES) $< -o $@

src/Greenhouse/Nursery/moc_SeasonClock.cpp: src/Greenhouse/Nursery/SeasonClock.h
	$(MOC) $(QT_INCLUDES) $< -o $@


# Compile MOC files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


run: $(TARGET)
	./$(TARGET)

# Demo target for DemoMain.cpp
$(DEMO_TARGET): DemoMain.cpp $(OBJS) $(MOC_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(QT_LIBS)

demo: $(DEMO_TARGET)
	./$(DEMO_TARGET)

# Test target (if you add test files in tests/)
TEST_SRCS = $(wildcard tests/*.cpp)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
TEST_TARGET = run_tests

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Valgrind targets
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

valgrind_test: $(TEST_TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TEST_TARGET)

valgrind_demo:
	cd gui/ChaoticPlants && mkdir -p build && cd build && cmake .. && $(MAKE) && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=../../../qt5.supp ./ChaoticPlants

# GDB targets
gdb: $(TARGET)
	gdb ./$(TARGET)

gdb_test: $(TEST_TARGET)
	gdb ./$(TEST_TARGET)

# Coverage targets
coverage: clean_gcda CXXFLAGS += $(GCOV_FLAGS)
coverage: clean $(TARGET)
	./$(TARGET)

coverage_test: clean_gcda CXXFLAGS += $(GCOV_FLAGS)
coverage_test: clean $(TEST_TARGET)
	./$(TEST_TARGET)

report: coverage
	rm -rf gcovr-report
	mkdir gcovr-report
	gcovr --root . --html --html-details --output gcovr-report/coverage.html

clean_gcda:
	rm -f *.gcda *.gcno

clean:
	rm -f $(OBJS) $(MOC_OBJS) $(TEST_OBJS) $(TARGET) $(DEMO_TARGET) $(TEST_TARGET) *.o *.gcov *.gcda *.gcno *.gz *.html *.css output.txt coverage.txt $(MOC_SRCS)

.PHONY: all run demo clean test valgrind valgrind_test valgrind_demo gdb gdb_test coverage coverage_test report clean_gcda

# Run CMake and execute unit tests
unit_tests:
	mkdir -p build
	cd build && cmake .. && $(MAKE)
	cd build && ctest --output-on-failure