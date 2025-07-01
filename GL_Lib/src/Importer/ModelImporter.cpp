#include "ModelImporter.h"
#include <iostream>
#include <stb_image.h>

namespace gllib
{
    ModelImporter::ModelImporter()
    {
        // Initialize stb_image to flip loaded textures vertically
        stbi_set_flip_vertically_on_load(true);
    }

    ModelImporter::~ModelImporter()
    {
        cleanup();
    }

    bool ModelImporter::loadModel(const std::string& filePath)
    {
        // Clean up any existing data
        cleanup();

        // Clear the texture cache
        textures_loaded.clear();

        // Create an instance of the Importer class
        Assimp::Importer importer;

        // Import the model with common post-processing
        const aiScene* scene = importer.ReadFile(filePath,
                                                 aiProcess_Triangulate |
                                                 aiProcess_GenSmoothNormals |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace);

        // Check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return false;
        }

        // Store the directory path for loading textures
        directory = filePath.substr(0, filePath.find_last_of('/'));

        // Process all the nodes recursively
        processNode(scene->mRootNode, scene);

        return true;
    }

    void ModelImporter::processNode(aiNode* node, const aiScene* scene)
    {
        // Process each mesh in the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        // Process children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh ModelImporter::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // Process vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            // Position (use uppercase field names from Mesh.h)
            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;

            // Normal
            if (mesh->HasNormals())
            {
                vertex.Normal.x = mesh->mNormals[i].x;
                vertex.Normal.y = mesh->mNormals[i].y;
                vertex.Normal.z = mesh->mNormals[i].z;
            }

            // Texture coordinates
            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
                vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            // Initialize tangent and bitangent if available
            if (mesh->mTangents)
            {
                vertex.Tangent.x = mesh->mTangents[i].x;
                vertex.Tangent.y = mesh->mTangents[i].y;
                vertex.Tangent.z = mesh->mTangents[i].z;
            }

            if (mesh->mBitangents)
            {
                vertex.Bitangent.x = mesh->mBitangents[i].x;
                vertex.Bitangent.y = mesh->mBitangents[i].y;
                vertex.Bitangent.z = mesh->mBitangents[i].z;
            }

            vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Process materials and textures
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // Load textures for the material
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        }

        // Return the Mesh using its constructor (this will call setupMesh automatically)
        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> ModelImporter::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;

            // Check if texture was loaded before
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                // If texture hasn't been loaded already, load it
                Texture texture;
                texture.id = textureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture); // Store it in loaded textures
            }
        }
        return textures;
    }

    unsigned int ModelImporter::textureFromFile(const char* path, const std::string& directory, bool gamma)
    {
        std::string filename = std::string(path);

        // Handle different path separators
        if (!directory.empty())
        {
            if (directory.back() != '/' && directory.back() != '\\')
                filename = directory + "/" + filename;
            else
                filename = directory + filename;
        }

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            else
                format = GL_RGB;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
            std::cout << "Texture loaded successfully: " << filename << std::endl;
        }
        else
        {
            std::cout << "Texture failed to load at path: " << filename << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    size_t ModelImporter::getMeshCount() const
    {
        return meshes.size();
    }


    unsigned int ModelImporter::getTexture(size_t meshIndex) const
    {
        if (meshIndex < meshes.size() && !meshes[meshIndex].textures.empty())
        {
            // Find first diffuse texture
            for (const auto& texture : meshes[meshIndex].textures)
            {
                if (texture.type == "diffuse")
                    return texture.id;
            }

            // If no diffuse texture, return the first texture
            return meshes[meshIndex].textures[0].id;
        }
        return 0;
    }

    bool ModelImporter::hasTexture(size_t meshIndex) const
    {
        if (meshIndex < meshes.size())
        {
            return !meshes[meshIndex].textures.empty();
        }
        return false;
    }

    void ModelImporter::assignTexture(size_t meshIndex, const std::string& texturePath, const std::string& textureType)
    {
        if (meshIndex >= meshes.size())
            return;

        // Check if texture was already loaded
        for (const auto& tex : textures_loaded)
        {
            if (tex.path == texturePath)
            {
                Texture texture = tex;
                texture.type = textureType;
                meshes[meshIndex].textures.push_back(texture);
                return;
            }
        }

        // Load new texture
        Texture texture;
        texture.id = textureFromFile(texturePath.c_str(), directory);
        texture.type = textureType;
        texture.path = texturePath;
        meshes[meshIndex].textures.push_back(texture);
        textures_loaded.push_back(texture);
    }

    const Mesh& ModelImporter::getMesh(size_t meshIndex) const
    {
        if (meshIndex < meshes.size())
        {
            return meshes[meshIndex];
        }
        throw std::out_of_range("Mesh index out of range");
    }

    void ModelImporter::drawMesh(size_t meshIndex) const
    {
        if (meshIndex >= meshes.size())
        {
            return; // Guard against invalid mesh index
        }

        const auto& mesh = meshes[meshIndex];

        // Ensure VAO is valid before binding
        if (mesh.VAO == 0)
        {
            // VAO not initialized - this would cause the crash
            return;
        }

        // Bind the VAO before drawing
        glBindVertexArray(mesh.VAO);

        // Verify the mesh has indices to draw
        if (mesh.indices.empty())
        {
            glBindVertexArray(0);
            return;
        }

        // Draw the mesh
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);

        // Unbind VAO
        glBindVertexArray(0);
    }

    void ModelImporter::drawAllMeshes() const
    {
        for (size_t i = 0; i < meshes.size(); i++)
        {
             drawMesh(i);
        }
    }

    void ModelImporter::cleanup()
    {
        // The Mesh destructor will handle VAO/VBO/EBO cleanup automatically
        // Just need to clean up textures
        for (auto& texture : textures_loaded)
        {
            if (texture.id != 0)
                glDeleteTextures(1, &texture.id);
        }

        meshes.clear();
        textures_loaded.clear();
    }
}
