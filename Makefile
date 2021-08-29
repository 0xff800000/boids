CC = g++
CFLAGS = -W -Wall -O3 -std=c++20 -g
LDFLAGS = -LGLEW -lGL -lglut -lGLU -lstdc++
EXEC = boid

SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) -o $(EXEC) $^ $(CFLAGS) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm *.o
	rm $(EXEC)
