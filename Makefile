SHELL=/bin/bash

compile:
	@g++ src/main.cc -o main $$(pkg-config --cflags --libs gtk4) -lsfml-window -lsfml-audio -lsfml-system

clean:
	@rm -f main
