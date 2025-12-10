SHELL=/bin/bash

compile:
	@g++ src/main.cc -o main $$(pkg-config --cflags --libs gtk4)

clean:
	@rm -f main
