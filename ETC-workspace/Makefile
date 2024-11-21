CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
SRC = ETC_controller.cpp main.cpp
TEST_SRC = ETC-test.cpp
OUT = ETC
TEST_OUT = ETC_TEST

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $^ -lgtest -lgtest_main -pthread -o $(TEST_OUT)
	./$(TEST_OUT)

clean:
	rm -f $(OUT) $(TEST_OUT)
