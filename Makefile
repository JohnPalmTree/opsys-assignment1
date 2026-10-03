all: build	

build: 
	@echo "Building"
	gcc jse94_assignment1.c -o shell

run: build
	./shell

clean:
	@echo "Cleaning"
	rm shell