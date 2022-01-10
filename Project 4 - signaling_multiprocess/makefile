# Alex Sawicki, tul69540 - OS Project 4
# TA: Rachel Lazzaro

main: main.cpp
	g++ -std=c++11 main.cpp main.h -o main -Wall -Werror -lpthread
#	g++ -std=c++11 signalHandler.cpp -o signalHandler -Wall -Werror -lpthread


main.o: main.cpp
	g++ -c main.cpp
#	g++ -c signalHandler.cpp


clean:
	rm *.o main
#	rm *.o signalHandler