opencv_path = `pkg-config opencv --cflags --libs`

image_stab: my_main.cpp
	g++  my_main.cpp my_videostab.cpp $(opencv_path)


