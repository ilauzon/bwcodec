CPP_FILES	:= $(wildcard *.cpp)
OBJ       	:= $(CPP_FILES:.cpp=.o)
CXXFLAGS  	:= -std=c++23 -g $(shell pkg-config --cflags opencv4)
LDLIBS 		:= $(shell pkg-config --libs opencv4)
EXEC	  	:= bwcodec

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJ)
	$(CXX) $(LDLIBS) -o $@ $^

.PHONY: run
run: $(EXEC)
	./$(EXEC)

.PHONY: clean
clean:
	rm -f $(EXEC) $(OBJ) $(OBJ:.o=.d)
	rm -rf $(EXEC).dSYM
