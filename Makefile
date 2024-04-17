PROG	= openlandmine
CFLAGS	= -w -s -O2 -ansi -DSHM
XLIBS	= -lX11 -lXext -lXmu -lXext -lXmu -lXt -lXi -lSM -lICE
LIBS	= -lglut -lGLU -lGL
INCLS	= -I/usr/X11R/include -I/share/mesa/include
LIBDIR	= -L/usr/X11/lib -L/usr/X11R6/lib

#source codes
SRCS	= openlandmine.cpp
OBJS	= $(SRCS:.cpp=.o)

$(PROG): $(OBJS)
	g++ -o $@ $(OBJS) $(LIBDIR) $(LIBS) $(XLIBS)

$(OBJS):
	g++ -c $*.cpp $(INCLS)

clean:
	rm $(OBJS) $(PROG)