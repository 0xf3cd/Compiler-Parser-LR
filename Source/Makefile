OSNAME = $(shell uname)
OBJS = FileReader.o \
	   Symbol.o \
	   Production.o \
	   Item.o \
	   Tokenizer.o \
	   Grammar.o \
	   LR.o \
	   LR_Wrap.o

ifeq ($(OSNAME), Darwin)
	CXX = clang++
	DYLIB = LR.dylib
	TARGET = test
endif

ifeq ($(OS), Windows_NT)
	CXX = g++ #64bits
	DYLIB = LR.dll
	TARGET = test.exe
endif

$(TARGET) : $(DYLIB) main.o
	$(CXX) -o $(TARGET) $(DYLIB) main.o

main.o : main.cpp
	$(CXX) -c -o main.o main.cpp

$(DYLIB) : $(OBJS)
	$(CXX) -o $(DYLIB) -shared $(OBJS)

%.o : %.cpp
	$(CXX) -c -o $@ $<

clean :
	-rm -f $(OBJS)
	-rm -f main.o
	-rm -f $(TARGET)
	-rm -f $(DYLIB)