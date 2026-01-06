CPP_FILES	:= $(wildcard *.cpp)
OBJ       	:= $(CPP_FILES:.cpp=.o)
CXXFLAGS  	:= -std=c++20 -g
EXEC	  	:= bwcodec

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

-include $(OBJ:.o=.d)

.PHONY: run
run: $(EXEC)
	./$(EXEC)

.PHONY: clean
clean:
	rm -f $(EXEC) $(OBJ) $(OBJ:.o=.d)
	rm -rf $(EXEC).dSYM
