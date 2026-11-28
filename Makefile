CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET = parking_system.exe
SRCS = main.cpp $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# 处理 src 目录下的 cpp
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 处理根目录的 main.cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q src\*.o *.o $(TARGET) 2>nul || rm -f src/*.o *.o $(TARGET)