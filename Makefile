IRgpio_objects = IRgpio.o  

all : IRgpio 

IRgpio: $(IRgpio_objects)
	g++ -o IRgpio $(IRgpio_objects) -lwiringPi -lrt -llirc_client

IRgpio:IRgpio.cpp

clean:
	rm -f *.o IRgpio 