# Makefile
CPP=g++
CPPFLAGS=-std=c++11 -Wall -Wextra -pedantic -g
                                                                             
# Links files together to create executable                                                                                                                 
DataLoopTest: DataLoop.o DataLoopTest.o
	$(CPP) -o DataLoopTest DataLoop.o DataLoopTest.o

TDataLoopTest: TDataLoopTest.o
	$(CPP) -o TDataLoopTest TDataLoopTest.o

# Creates object files    
DataLoopTest.o: DataLoopTest.cpp DataLoop.cpp DataLoop.h
	$(CPP) $(CPPFLAGS) -c DataLoopTest.cpp DataLoop.cpp

DataLoop.o: DataLoop.cpp DataLoop.h
	$(CPP) $(CPPFLAGS) -c DataLoop.cpp

TDataLoopTest.o: TDataLoopTest.cpp TDataLoop.h
	$(CPP) $(CPPFLAGS) -c TDataLoopTest.cpp TDataLoop.h

# Removes all object files and the executables so we can start fresh                                                                                                                                                              
clean:
	rm -f *.o DataLoopTest TDataLoopTest