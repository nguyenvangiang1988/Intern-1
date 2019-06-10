run: chatclient
	@./chatclient
chatclient: chatclient.c lib.c
	@rm -f chatserver
	@gcc -pthread -w -o chatclient chatclient.c
clean:
	@rm -f chatclient
