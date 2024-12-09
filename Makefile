# Platform-specific settings
ifeq ($(OS),Windows_NT)
    CC = gcc
    CFLG = -O3 -Wall -DUSEGLEW -I/c/msys64/mingw64/include
    LIBS = -lfreeglut -lglew32 -lglu32 -lopengl32
    CLEAN = rm -f *.exe ./obj/*.o
else
    UNAME_S := $(shell uname)
    # macOS
    ifeq ($(UNAME_S),Darwin)
        CFLG = -O3 -Wall -Wno-deprecated-declarations -DRES=1
        LIBS = -framework GLUT -framework OpenGL
    else
        # Linux/Unix/Solaris
        CFLG = -O3 -Wall
        LIBS = -lglut -lGLU -lGL -lm
    endif
    CLEAN = rm -f final *.o
endif

# Directories
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Source and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Build target
final: $(OBJ_FILES)
	$(CC) $(CFLG) -o $@ $^ $(LIBS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLG) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up
clean:
	$(CLEAN)
