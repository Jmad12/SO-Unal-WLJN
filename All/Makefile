OBJS_SERVER = obj/server/main.o obj/server/clinicalHistory.o obj/server/hash.o obj/server/createPet.o obj/server/findPet.o obj/server/getPet.o obj/server/initProgram.o obj/server/makeLog.o obj/server/processRequest.o obj/server/removePet.o obj/server/save.o

OBJS_CLIENT = obj/client/main.o obj/client/clinicalHistory.o obj/client/createPet.o obj/client/deletePet.o obj/client/findPet.o obj/client/getPet.o obj/client/petsMethods.o

OBJS_GENERATOR = obj/generator/generatePet.o obj/generator/generateRandomDog.o obj/generator/hash.o obj/generator/initProgram.o obj/generator/main.o obj/generator/retrieve.o obj/generator/save.o

CC = gcc
CFLAGS = -Wall -lpthread

all: server client generator
	
server: $(OBJS_SERVER)
	$(CC) $? $(CFLAGS) -o bin/server
	
client: $(OBJS_CLIENT)
	$(CC) $? $(CFLAGS) -o bin/client
	
generator: $(OBJS_GENERATOR)
	$(CC) $? $(CFLAGS) -o bin/generator
	
obj/server/%.o: src/server/%.c src/server/server.h
	$(CC) -c $< -o $@
	
obj/client/%.o: src/client/%.c src/client/client.h
	$(CC) -c $< -o $@
	
obj/generator/%.o: src/generator/%.c src/generator/generator.h
	$(CC) -c $< -o $@

clean:
	rm -f bin/server bin/client bin/generator obj/server/*.o obj/client/*.o obj/generator/*.o
	
cleanAll:
	rm -f bin/server bin/client bin/generator obj/server/*.o obj/client/*.o obj/generator/*.o history/server/*.txt history/client/*.txt data/*.dat data/*.log

