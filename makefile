CC=gcc
MOSQUITTO=$(HOME)/mosquitto-1.1.3
INC=-I. -I$(MOSQUITTO)/lib -I$(MOSQUITTO)/src
CFLAGS=-Wall -fpic -DMQAP_DEBUG

LIBS=-lcurl
OBJ=mosquitto_auth_plugin_keystone.o adcmb.o
EXE=mosquitto_auth_plugin_keystone.so

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $<

all: 	plugin 

plugin: $(OBJ)
	$(CC) -shared -o $(EXE) $(OBJ) $(LIBS)


clean:
	rm -f *.so *.o
