NAME = TCPserver
FLAGS =  -Wall -Werror -Wextra -lsystemd
SRC =	server/server.c daemonize/daemonize.c daemonize/utils.c main.c
DIR = /etc/TCPserver
OBJ = $(SRC:.c=.o)
$(NAME): $(OBJ)
	gcc $(FLAGS)  $^ -o $(NAME)
	sudo systemctl stop TCPserver
	sudo cp TCPserver /usr/bin
	@if [ ! -d $(DIR) ] ; then echo "creating $(DIR)" ; mkdir /etc/TCPserver; fi
	sudo cp TCPserver.conf /etc/TCPserver
	sudo cp TCPserver.service /etc/systemd/system
	sudo systemctl daemon-reload
	sudo systemctl start TCPserver

%.o : %.c
	gcc $(FLAGS) -o $@ -c $<
all: $(NAME)

clean:
	rm -rf $(OBJ)
fclean: clean
	rm -f $(NAME)
re: fclean all