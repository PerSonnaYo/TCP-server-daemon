all:
	gcc server/server.c main.c daemonize/daemonize.c -pthread -o TCPserver
	sudo cp TCPserver /usr/bin
	sudo systemctl start TCPserver
 	sudo systemctl status TCPserver