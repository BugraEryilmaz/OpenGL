all:
	g++ -o hw3 -Wno-narrowing -std=c++11 main.cpp parser.cpp tinyxml2.cpp -lglew32s -lglfw3 -lopengl32 -lglu32 -lgdi32