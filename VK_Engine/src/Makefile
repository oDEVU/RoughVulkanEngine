CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VulkanApp: *.cpp include/*.hpp include/*.cpp include/external/*.h
	./compile_shaders.sh
	g++ $(CFLAGS) -o ../bin/VulkanApp *.cpp include/*.cpp $(LDFLAGS)

.PHONY: test clean

test: VulkanApp
	./../bin/VulkanApp

clean:
	rm -f ../bin/VulkanApp