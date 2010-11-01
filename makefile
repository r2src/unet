unet: main.o agent.o link.o
	g++ main.o agent.o link.o -lgsl -lgslcblas -o unet --static
main.o: main.h main.cpp
	g++ -ggdb --static -c -Wall main.cpp
agent.o: agent.h agent.cpp
	g++ -ggdb --static -c -Wall agent.cpp
link.o: link.h link.cpp
	g++ -ggdb --static -c -Wall link.cpp
clean:
	rm unet *.o
