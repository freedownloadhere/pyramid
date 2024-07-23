#!/usr/bin/bash
echo building...
if g++ -g -o pyramid main.c include/glad.c include/stb_image.c -Iinclude -Llib -lglfw3 -Wall -O2; then
	echo built the pyramid of giza.
else
	echo did not builid the pyram.d.
fi
