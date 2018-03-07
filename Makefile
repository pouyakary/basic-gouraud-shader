
name = "Kary's Gouraud Shader"
file = shader.cpp

build: $(file)
	g++ -std=c++11 -o $(name) $(file)  -framework OpenGl -framework Glut

run: build
	./$(name); rm ./$(name)

clean: ./$(name)
	rm ./$(name)
