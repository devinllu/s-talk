all:
	gcc -Wall -Werror -pthread -o s-talk receiver.c sender.c main.c list.c output.c input.c network.c

clean:
	rm s-talk
