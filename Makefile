all:
	g++ -o hw3 -std=c++11 main.cpp parser.cpp tinyxml2.cpp -lglew32 -lglfw3 -lopengl32 -lglu32 -lgdi32