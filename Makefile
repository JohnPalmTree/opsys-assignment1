all: build	

build: 
	@echo "Building"
	gcc -std=c11 -pedantic jse94_assignment1.c -o shell
	
run: build
	./shell

clean:
	@echo "Cleaning"
	rm shell