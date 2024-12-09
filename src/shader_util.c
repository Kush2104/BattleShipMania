#include "include/shader_util.h"

// Error handling
void ShaderErrorExit(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}

char* ReadShaderFile(const char* file) {
    printf("Opening file: %s\n", file);
    
    char* buffer;
    FILE* f = fopen(file, "rt");
    if (!f) {
        ShaderErrorExit("Cannot open shader file %s\n", file);
        return NULL;
    }

    printf("File opened successfully\n");
    
    fseek(f, 0, SEEK_END);
    int n = ftell(f);
    printf("File size: %d bytes\n", n);
    rewind(f);

    buffer = (char*)malloc(n + 1);
    if (!buffer) {
        ShaderErrorExit("Cannot allocate %d bytes for shader file %s\n", n + 1, file);
        fclose(f);
        return NULL;
    }

    printf("Buffer allocated successfully\n");
    
    size_t bytes_read = fread(buffer, 1, n, f);
    printf("Bytes read: %zu\n", bytes_read);
    
    if (bytes_read != n) {
        printf("Read failed. errno: %d\n", errno);
        printf("Buffer content: %s\n", buffer);
        ShaderErrorExit("Cannot read %d bytes from shader file %s\n", n, file);
        free(buffer);
        fclose(f);
        return NULL;
    }

    buffer[n] = 0;
    fclose(f);
    return buffer;
}

void PrintShaderLog(int obj, const char* file) {
    int len = 0;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &len);
    
    if (len > 1) {
        char* buffer = (char*)malloc(len);
        if (!buffer) {
            ShaderErrorExit("Cannot allocate %d bytes for shader log\n", len);
            return;
        }
        
        int n = 0;
        glGetShaderInfoLog(obj, len, &n, buffer);
        fprintf(stderr, "%s:\n%s\n", file, buffer);
        free(buffer);
    }
    
    glGetShaderiv(obj, GL_COMPILE_STATUS, &len);
    if (!len) {
        ShaderErrorExit("Error compiling shader %s\n", file);
    }
}

void PrintProgramLog(int obj) {
    int len = 0;
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &len);
    
    if (len > 1) {
        char* buffer = (char*)malloc(len);
        if (!buffer) {
            ShaderErrorExit("Cannot allocate %d bytes for program log\n", len);
            return;
        }
        
        int n = 0;
        glGetProgramInfoLog(obj, len, &n, buffer);
        fprintf(stderr, "%s\n", buffer);
        free(buffer);
    }
    
    glGetProgramiv(obj, GL_LINK_STATUS, &len);
    if (!len) {
        ShaderErrorExit("Error linking program\n");
    }
}

int CreateShader(int prog, const GLenum type, const char* file) {
    // Create the shader
    int shader = glCreateShader(type);
    
    // Load source code
    char* source = ReadShaderFile(file);
    glShaderSource(shader, 1, (const char**)&source, NULL);
    free(source);
    
    // Compile the shader
    glCompileShader(shader);
    
    // Check for errors
    PrintShaderLog(shader, file);
    
    // Attach to shader program
    glAttachShader(prog, shader);
    
    return shader;
}

int CreateShaderProg(const char* vert, const char* frag) {
    // Create program
    int prog = glCreateProgram();
    
    // Create and compile vertex shader if provided
    if (vert) CreateShader(prog, GL_VERTEX_SHADER, vert);
    
    // Create and compile fragment shader if provided
    if (frag) CreateShader(prog, GL_FRAGMENT_SHADER, frag);
    
    // Link program
    glLinkProgram(prog);
    
    // Check for errors
    PrintProgramLog(prog);
    
    return prog;
}

int CreateShaderProgCompute(const char* file) {
    // Create program
    int prog = glCreateProgram();
    
    // Create and compile compute shader
    CreateShader(prog, GL_COMPUTE_SHADER, file);
    
    // Link program
    glLinkProgram(prog);
    
    // Check for errors
    PrintProgramLog(prog);
    
    return prog;
}

int CreateShaderGeom(const char* VertFile, const char* GeomFile, const char* FragFile) {
    // Create program
    int prog = glCreateProgram();
    
    // Create and compile shaders
    if (VertFile) CreateShader(prog, GL_VERTEX_SHADER, VertFile);
    if (GeomFile) CreateShader(prog, GL_GEOMETRY_SHADER, GeomFile);
    if (FragFile) CreateShader(prog, GL_FRAGMENT_SHADER, FragFile);
    
    // Link program
    glLinkProgram(prog);
    
    // Check for errors
    PrintProgramLog(prog);
    
    return prog;
}