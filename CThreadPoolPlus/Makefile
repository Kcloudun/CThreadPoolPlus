.SUFFIXES: .cxx .o

OBJS = CThreadPoolPlus.o ThreadPoolImpl.o
INC_DIR = -I.
LIBS = -lpthread
CC = g++ -fPIC -fpermissive
MKSO = g++ -g -shared -fpermissive

all: lib

lib: $(OBJS)
	$(MKSO) -o ./libCThreadPoolPlus.so $(OBJS) $(LIBS)

.cxx.o:
	$(CC) -g -c $(INC_DIR) -o $@ $<

clean:
	rm -f *.o libCThreadPoolPlus.so

.PHONY: clean
