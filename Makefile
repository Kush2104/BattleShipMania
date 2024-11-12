# Platform-specific settings
ifeq ($(OS),Windows_NT)
    CC = gcc
    CFLAGS = -Wall -g -O3 -DUSEGLEW -I/c/msys64/mingw64/include
    LDFLAGS = -lfreeglut -lglew32 -lglu32 -lopengl32
    TARGET = battleship_simulator.exe
    CLEAN = rm -rf *.exe obj/*.o
else
    UNAME_S := $(shell uname)
    ifeq ($(UNAME_S),Darwin)
        CC = gcc
        CFLAGS = -Wall -g -O3 -Wno-deprecated-declarations -DRES=1
        LDFLAGS = -framework GLUT -framework OpenGL
    else
        CC = gcc
        CFLAGS = -Wall -g -O3 -DUSEGLEW `pkg-config --cflags glew freeglut`
        LDFLAGS = `pkg-config --libs glew freeglut` -lGL -lGLU -lm
    endif
    TARGET = battleship_simulator
    CLEAN = rm -f $(TARGET) obj/*.o
endif

# Directories
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Source and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Build target
$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	$(CLEAN)
