SOURCES=myShell.cpp  ReadCommand.cpp  ParseSetCommand.cpp  ParseCommand.cpp  BuiltInFunction.cpp
OBJS=$(patsubst %.cpp, %.o, $(SOURCES))
CPPFLAGS=-ggdb3 -Wall -Werror -pedantic -std=gnu++11
PROGRAM=myShell

$(PROGRAM): $(OBJS)
	g++ $(CPPFLAGS) -o myShell $(OBJS)
%.o: %.cpp ReadCommand.h  ParseSetCommand.h  ParseCommand.h  BuiltInFunction.h
	g++ $(CPPFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROGRAM) *~ *.o
