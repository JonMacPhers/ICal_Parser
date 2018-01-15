CC = gcc

CFLAGS = -g -Wall -std=c11 -Iinclude -shared



LINKED = LinkedListAPI

PARSER = CalendarParser

A2 = A2main

TEST = MainFunctionTestHarness



all: ./bin/libllist.so ./bin/libcparse.so

A2main: ./bin/$(A2)

list: ./bin/libllist.so

parser: ./bin/libcparse.so

testHarness: ./bin/$(TEST)



#./bin/$(A2): ./bin/$(A2).o

#    $(CC) $(CFLAGS) -o ./bin/$(A2) ./bin/$(A2).o



#./bin/$(A2).o: ./src/$(A2).c ./bin/$(PARSER).so ./bin/$(LINKED).so

#    $(CC) $(CFLAGS) -c -fPIC ./src/$(A2).c -o ./bin/$(A2).o



./bin/$(TEST): ./src/$(TEST).c ./bin/libcparse.so ./bin/libllist.so
	$(CC) $(CFLAGS) -L./bin/ -o ./bin/$(TEST) ./src/$(TEST).c -lcparse -lllist



#./bin/$(TEST).o: ./src/$(TEST).c

#    $(CC) $(CFLAGS) -c ./src/$(TEST).c -o ./bin/$(TEST).o



./bin/libcparse.so: ./src/$(PARSER).c ./include/$(PARSER).h ./include/Calendar_Helper.h ./bin/libllist.so
	$(CC) $(CFLAGS) -o ./bin/libcparse.so -fPIC ./src/$(PARSER).c ./bin/libllist.so -Wl,-rpath=./bin/



#./bin/libcparse.a: ./bin/$(PARSER).o

#    ar cr ./bin/libcparse.a ./bin/$(PARSER).o



#./bin/$(PARSER).o: ./src/$(PARSER).c ./include/$(PARSER).h ./include/CalendarHelper.h ./bin/libllist.a

#    $(CC) $(CFLAGS) -c ./src/$(PARSER).c -o ./bin/$(PARSER).o -lllist -L./bin/



./bin/libllist.so: ./src/$(LINKED).c ./include/$(LINKED).h
	$(CC) $(CFLAGS) -o ./bin/libllist.so -fPIC ./src/$(LINKED).c



#./bin/libllist.a: ./bin/$(LINKED).o

#    ar cr ./bin/libllist.a ./bin/$(LINKED).o



#./bin/$(LINKED).o: ./src/$(LINKED).c ./include/$(LINKED).h

#    $(CC) $(CFLAGS) -c -fPIC ./src/$(LINKED).c -o ./bin/$(LINKED).o



#Create linked list so library





clean:
	$(RM) ./bin/$(A2) ./bin/*.o ./bin/*.a ./bin/*.so