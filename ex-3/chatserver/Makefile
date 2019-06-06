run: chatserver
	@./chatserver
chatserver: chatserver.c
	@rm -f chatserver
	@gcc -w -pthread -o chatserver chatserver.c
clean:
	@rm -f chatserver
