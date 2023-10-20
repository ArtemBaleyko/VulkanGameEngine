CFLAGS = -std=c++17 -g -Wall -Werror
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VulkanGameEngine: src/*.cpp src/*.h
	g++ $(CFLAGS) -o VulkanGameEngine.out src/*.cpp $(LDFLAGS)

.PHONY: test clean

test: VulkanGameEngine
	./VulkanGameEngine.out

clean:
	rm -rf VulkanGameEngine.out