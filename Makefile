CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: walk

danzaldo.o: danzaldo.cpp
	g++ -c danzaldo.cpp -Wall
	
gjimenezroja.o: gjimenezroja.cpp
	g++ -c gjimenezroja.cpp -Wall

mlara2.o: mlara2.cpp
	g++ -c mlara2.cpp -Wall

msteiner.o: msteiner.cpp
	g++ -c msteiner.cpp -Wall

walk: walk.cpp
	g++ $(CFLAGS) walk.cpp libggfonts.a 
	danzaldo.o gjimenezroja.o mlara2.o msteiner.o 
	-Wall -Wextra $(LFLAGS) -owalk

clean:
	rm -f walk
	rm -f *.o
