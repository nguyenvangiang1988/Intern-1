run: chatserver
	@./chatserver
chatserver: chatserver.c lib.c
	@rm -f chatserver
	@gcc -w -pthread -o chatserver chatserver.c -Ilib.c
clean:
	@rm -f chatserver
