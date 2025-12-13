#include "shader.h"

#include "loader.h"

#include <iostream>
#include <vector>

using namespace gllib;
using namespace std;

// Private
unsigned int Shader::shapeShaderProgram = 0;
unsigned int Shader::textureShaderProgram = 0;

string Shader::getShaderType(unsigned int type) {
    switch (type) {
    case GL_VERTEX_SHADER:
        return "VERTEX SHADER";
    case GL_FRAGMENT_SHADER:
        return "FRAGMENT SHADER";
    default:
        return "OTHER SHADER";
    }
}

unsigned int Shader::compileShader(unsigned int type, string source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            vector<char> message(length);
            glGetShaderInfoLog(id, length, &length, message.data());
            cout << "(Failed to compile " << getShaderType(type) << " Shader) " << message.data() << endl;
        }
        else {
            cout << "(Failed to compile " << getShaderType(type) << " Shader) Unknown error" << endl;
        }
        glDeleteShader(id);
        return 0;
    }
    else {
        cout << "Compiled " << getShaderType(type) << " successfully!" << endl;
    }

    return id;
}

// Public

unsigned int Shader::createShader(const char* vertexShader, const char* fragmentShader) {
    cout << "Creating Shader Program..." << endl;
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    if (vs == 0 || fs == 0) {
        cout << "Failed to create shader program due to shader compilation error." << endl;
        if (vs != 0) glDeleteShader(vs);
        if (fs != 0) glDeleteShader(fs);
        glDeleteProgram(program);
        return 0;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int isLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (!isLinked) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            vector<char> message(length);
            glGetProgramInfoLog(program, length, &length, message.data());
            cout << "Failed to link shader program: " << message.data() << endl;
        }
        else {
            cout << "Failed to link shader program: Unknown error" << endl;
        }
        glDeleteProgram(program);
        return 0;
    }

    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    cout << "(" << program << ") Shader program created!" << endl;
    return program;
}

void Shader::destroyShader(unsigned int program) {
    cout << "(" << program << ") Unloading shader..." << endl;
    glDeleteProgram(program);
    cout << "Shader unloaded!" << endl;
}

const char* Shader::loadShader(string filePath) {
    return Loader::loadTextFile(filePath);
}

void Shader::useShaderProgram(unsigned int shaderProgram) {
    glUseProgram(shaderProgram);
}

void Shader::setShaderProgram(unsigned int shaderProgram) {
    glUseProgram(shaderProgram);
}
