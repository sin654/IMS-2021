.PHONY: all run
CC = g++
FILE = main.cpp
SIMLIB = -lsimlib -lm
SW = -g -O2 -o
OUTPUT = bread
DAT = bread.dat

all: main.cpp
	$(CC) $(SW) $(OUTPUT) $(FILE) $(SIMLIB)


run: all
	./$(OUTPUT) -o exp1_1.dat
	./$(OUTPUT) -a 20 -t 1 -m 1 -b 3 -d 23 -o exp1_2.dat
	./$(OUTPUT) -a 60 -t 2 -m 1 -b 3 -d 23 -o exp1_3.dat
	./$(OUTPUT) -a 80 -t 3 -m 1 -b 3 -d 23 -o exp1_4.dat

#-b baker
#-t trasher
#-a acres of wheat
#-m mills
#-s shopkeepers

#-o output file
#-d simulation time [months]
exp-all: exp1 exp2

exp1: all #experimenting with acres
	./$(OUTPUT) -o exp1.dat


exp2: all #experimenting with bakers
	./$(OUTPUT) -a 40 -t 2 -m 1 -b 3 -d 23 -o exp2_1.dat
	./$(OUTPUT) -a 35 -t 2 -m 1 -b 3 -d 23 -o exp2_2.dat
	./$(OUTPUT) -a 30 -t 2 -m 1 -b 3 -d 23 -o exp2_3.dat
	./$(OUTPUT) -a 25 -t 2 -m 1 -b 3 -d 23 -o exp2_4.dat
# ./$(OUTPUT) -a 20 -t 1 -m 1 -b 6 -d 23 -o exp2_1.dat
# ./$(OUTPUT) -a 20 -t 1 -m 1 -b 5 -d 23 -o exp2_2.dat
# ./$(OUTPUT) -a 20 -t 1 -m 1 -b 4 -d 23 -o exp2_3.dat
# ./$(OUTPUT) -a 20 -t 1 -m 1 -b 3 -d 23 -o exp2_4.dat
# ./$(OUTPUT) -a 10 -t 1 -m 1 -b 6 -d 23 -o exp2_1.dat
# ./$(OUTPUT) -a 10 -t 1 -m 1 -b 5 -d 23 -o exp2_2.dat
# ./$(OUTPUT) -a 10 -t 1 -m 1 -b 4 -d 23 -o exp2_3.dat
# ./$(OUTPUT) -a 10 -t 1 -m 1 -b 3 -d 23 -o exp2_3.dat



clean: 
	rm $(OUTPUT) $(DAT) exp1_1.dat exp1_2.dat exp1_3.dat exp1_4.dat
