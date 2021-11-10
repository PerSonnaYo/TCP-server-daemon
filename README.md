# TCP-server-daemon
Just a simple TCP server that was daemonized with the Systemd standards.

# How to install?
	sudo apt-get install libsystemd-dev
	make

# How the daemon works?
	sudo systemctl daemon-reload
	sudo systemctl start TCPserver
	sudo systemctl status TCPserver

# To remove the daemon, you must run:
	sudo systemctl stop TCPserver
	
# To see the syslog:
	tail -f /var/log/syslog

# To run the client, you must run:
	bash client.sh
