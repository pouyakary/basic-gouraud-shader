build: shader.cpp
	g++ -std=c++11 -o shader shader.cpp  -framework OpenGl -framework Glut

run: build
	./shader; rm ./shader

clean: ./shader
	rm ./shader
