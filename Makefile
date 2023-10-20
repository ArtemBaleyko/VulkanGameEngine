CFLAGS = -std=c++17 -g -Wall -Werror
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vertSources = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ./shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

TARGET = VulkanGameEngine.out
$(TARGET): $(vertObjFiles) $(fragObjFiles)

$(TARGET): src/*.cpp src/*.h
	g++ $(CFLAGS) -o $(TARGET) src/*.cpp $(LDFLAGS)

%.spv: %
	glslc $< -o $@

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET)
	rm -rf ./shaders/*.spv