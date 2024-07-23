@echo off
echo "building the pyramids bro trust me"
g++ -g -o pyramid main.c include/glad.c include/stb_image.c -Iinclude -Llib -lglfw3 -Wall -O2
echo "idk check if it built lol"
