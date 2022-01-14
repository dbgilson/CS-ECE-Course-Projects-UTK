##COSC 530: PA1, Memory Hierarchy Simulator
##By David Gilson
##Simple C++ Makefile

all: MemSim.cpp DC.cpp L2.cpp DTLB.cpp PT.cpp
	g++ -c -o DC.o DC.cpp; g++ -c -o L2.o L2.cpp; g++ -c -o DTLB.o DTLB.cpp; g++ -c -o PT.o PT.cpp; g++ -o MemSim MemSim.cpp DC.o L2.o DTLB.o PT.o

clean:
	rm MemSim
	rm DC.o
	rm L2.o
	rm DTLB.o
	rm PT.o
