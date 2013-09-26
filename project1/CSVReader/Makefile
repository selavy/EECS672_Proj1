CC = g++
CFLAGS = -Wall -Werror -pedantic -ansi -g
all: CSVReader.o
UnitTest: CSVReaderUnitTest
	./CSVReaderUnitTest
CSVReaderUnitTest: CSVReader.o
	$(CC) $(CFLAGS) CSVReader.o CSVReaderUnitTest.c++ -o CSVReaderUnitTest
CSVReader.o:
	$(CC) $(CFLAGS) -c CSVReader.c++
clean:
	rm -rf *.o *~ CSVReaderUnitTest
