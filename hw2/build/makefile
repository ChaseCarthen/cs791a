# This is a example build file that will be the ground truth for other applications
# Linux
#CC=g++
#LIBS= -lglut -lGLEW -lGL

# OSX
CC=clang++
LIBS=  -framework OpenGL -framework Cocoa `wx-config --libs --cxxflags --gl-libs` -L/usr/local/lib -lGLEW -lgdal -lCGAL -lmpfr -lgmp 

# Compiler flags
CXXFLAGS= -g -w -std=c++11 #-stdlib=libc++
INCLUDES= -I ../include -I /usr/local/include/
SOURCE= ../src/mainframe.cpp ../src/example2.cpp ../src/shader.cpp ../src/renderer.cpp ../src/terrain.cpp ../src/gishandler.cpp ../src/terrainmanager.cpp ../src/triangle.cpp ../src/shape.cpp
MAIN: ../src/example2.cpp
	$(CC) $(CXXFLAGS) $(SOURCE) -o gl $(LIBS) $(INCLUDES) 


#MAIN:	../src/example2.cpp ../include/example2.h
#	clang++ -std=c++11 ../src/example2.cpp -framework OpenGL -framework Cocoa `wx-config --libs --cxxflags --gl-libs` -I../include 
