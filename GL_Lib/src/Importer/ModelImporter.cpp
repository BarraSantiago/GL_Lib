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

    ModelImporter::Mesh ModelImporter::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        Mesh result;

        // Process vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            // Position
            vertex.position.x = mesh->mVertices[i].x;
            vertex.position.y = mesh->mVertices[i].y;
            vertex.position.z = mesh->mVertices[i].z;

            // Normal
            if (mesh->HasNormals())
            {
                vertex.normal.x = mesh->mNormals[i].x;
                vertex.normal.y = mesh->mNormals[i].y;
                vertex.normal.z = mesh->mNormals[i].z;
            }

            // Texture coordinates
            if (mesh->mTextureCoords[0])
            {
                vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
                vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
            }
            else
            {
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            }

            result.vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                result.indices.push_back(face.mIndices[j]);
            }
        }

        // Process materials and textures
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // Get material color
            aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
            aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color);

            // Load textures for the material
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
            result.textures.insert(result.textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
            result.textures.insert(result.textures.end(), specularMaps.begin(), specularMaps.end());

            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "normal");
            result.textures.insert(result.textures.end(), normalMaps.begin(), normalMaps.end());
        }

        // Create the OpenGL buffers using the renderer
        std::vector<float> vertexData;
        for (const auto& v : result.vertices)
        {
            // Position
            vertexData.push_back(v.position.x);
            vertexData.push_back(v.position.y);
            vertexData.push_back(v.position.z);

            // Color (use 4 components: RGBA)
            vertexData.push_back(1.0f); // r
            vertexData.push_back(1.0f); // g
            vertexData.push_back(1.0f); // b
            vertexData.push_back(1.0f); // a

            // Texture Coordinates
            vertexData.push_back(v.texCoords.x);
            vertexData.push_back(v.texCoords.y);
        }

        // Convert to int array for EBO
        std::vector<int> indexData(result.indices.begin(), result.indices.end());

        // Create render data
        result.renderData = gllib::Renderer::createRenderData(
            vertexData.data(),
            static_cast<GLsizei>(vertexData.size()),
            indexData.data(),
            static_cast<GLsizei>(indexData.size())
        );

        return result;
    }

    std::vector<ModelImporter::Texture> ModelImporter::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
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
        filename = directory + '/' + filename;

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
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    size_t ModelImporter::getMeshCount() const
    {
        return meshes.size();
    }

    gllib::RenderData ModelImporter::getRenderData(size_t meshIndex) const
    {
        if (meshIndex < meshes.size())
        {
            return meshes[meshIndex].renderData;
        }
        return gllib::RenderData{0, 0, 0};
    }

    size_t ModelImporter::getIndicesCount(size_t meshIndex) const
    {
        if (meshIndex < meshes.size())
        {
            return meshes[meshIndex].indices.size();
        }
        return 0;
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

    void ModelImporter::cleanup()
    {
        // Clean up OpenGL resources
        for (auto& mesh : meshes)
        {
            gllib::Renderer::destroyRenderData(mesh.renderData);

            // Delete textures
            for (auto& texture : mesh.textures)
            {
                if (texture.id != 0)
                    glDeleteTextures(1, &texture.id);
            }
        }
        meshes.clear();
        textures_loaded.clear();
    }
}
