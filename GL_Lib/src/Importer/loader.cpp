#include "loader.h"

#include <iostream>
#include <fstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace gllib;
using namespace std;

bool Loader::fileExists(string filePath) {
    ifstream file(filePath);
    return file.good();
}

unsigned int Loader::loadTextureAdvanced(string filePath, GLint wrapping, GLint filtering, bool transparent) {
    cout << "Loading texture at " << filePath << "...\n";
    if (!fileExists(filePath)) {
        cerr << "No texture was found at the specified path!\n";
        return 0;
    }

    unsigned int texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        if (transparent) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);

        cout << "The texture (" << texture << ") " << filePath << " was loaded!\n";
    }
    else {
        cerr << "Failed to load texture!!\n";
        cerr << "Reason: " << stbi_failure_reason() << "\n";
        glDeleteTextures(1, &texture);
        return 0;
    }

    stbi_image_free(data);
    return texture;
}

unsigned int Loader::loadTexture(string filePath, bool transparent) {
    return loadTextureAdvanced(filePath, GL_REPEAT, GL_NEAREST, transparent);
}

const char* Loader::loadTextFile(string filePath) {
    ifstream file(filePath, ios::binary | ios::ate);
    if (!file.is_open()) {
        cout << "File not found: " << filePath << "\n";
        return "NULL";
    }

    streamsize file_size = file.tellg();
    if (file_size == -1) {
        cout << "Error determining file size: " << filePath << "\n";
        return "NULL";
    }

    file.seekg(0, ios::beg);
    if (!file.good()) {
        cout << "Error seeking to the beginning of the file: " << filePath << "\n";
        return "NULL";
    }

    // Removing BOMs in case of UTF-8 file
    char bom[3];
    file.read(bom, 3);
    if (!(bom[0] == static_cast<char>(0xEF) && bom[1] == static_cast<char>(0xBB) && bom[2] == static_cast<char>(0xBF))) {
        file.clear();
        file.seekg(0, ios::beg);
        if (!file.good()) {
            cout << "Error seeking to the beginning of the file: " << filePath << "\n";
            return "NULL";
        }
    }
    else {
        file_size -= 3;
    }

    char* text = new(nothrow) char[file_size + 1];
    if (text == nullptr) {
        cout << "Memory allocation error." << "\n";
        return "NULL";
    }

    file.read(text, file_size);
    if (!file) {
        cout << "Error reading file contents: " << filePath << "\n";
        delete[] text;
        return "NULL";
    }
    text[file_size] = '\0';

    file.close();

    return text;
}

void Loader::unloadTexture(unsigned int id) {
    glDeleteTextures(1, &id);
    cout << "Texture (" << id << ") was unloaded!\n";
}