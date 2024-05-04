PROG=openlandmine

LIBS = -lglut -lGL -lGLU
SRCS=$(PROG).cpp themes.cpp
OBJS=$(SRCS:.cpp=.o)

$(PROG): $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS)

$(OBJS):
	g++ -c $*.cpp

clean:
	rm $(OBJS)