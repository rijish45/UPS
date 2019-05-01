CFLAGS=-std=c++14 -pedantic -Wall -O3
SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SRCS))

server: $(OBJS)
	g++ -o $@ $(OBJS) -lpq -lpthread  -lpqxx -lprotobuf


%.o:%.cpp
	g++ $(CFLAGS) -c $<

.PHONY: clean depend
clean:
	rm -f server *.o *~ *\#

depend:
	makedepend $(SRCS)
        # DO NOT DELETE

# DO NOT DELETE
