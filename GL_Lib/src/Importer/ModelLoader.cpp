#include "ModelLoader.h"

#include <stb_image.h>
#include "Assimp/matrix4x4.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
namespace gllib
{
    std::vector<Texture> ModelLoader::textures_loaded;
    std::string ModelLoader::directory = "";

    void ModelLoader::loadModel(std::string const& path, std::vector<Mesh>& meshes, bool gamma, Transform* rootTransform)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
    
        glm::vec3 minAABB(FLT_MAX);
        glm::vec3 maxAABB(-FLT_MAX);
    
        // Process the root node - if it has only one child and no meshes, use the child as the actual root
        aiNode* actualRoot = scene->mRootNode;
        if (scene->mRootNode->mNumMeshes == 0 && scene->mRootNode->mNumChildren == 1)
        {
            actualRoot = scene->mRootNode->mChildren[0];
        }
    
        processNode(actualRoot, scene, meshes, gamma, minAABB, maxAABB, rootTransform, nullptr);
    }
    
    void ModelLoader::processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, bool gamma,
                                 glm::vec3& minAABB, glm::vec3& maxAABB,
                                 Transform* rootTransform, Transform* parentTransform)
    {
        Transform* currentTransform;
        
        // If this is the root node call and parentTransform is null, use the provided rootTransform
        if (parentTransform == nullptr)
        {
            currentTransform = rootTransform;
        }
        else
        {
            // Create a new child transform
            currentTransform = new Transform();
            parentTransform->addChild(currentTransform);
        }
    
        // Convert Assimp matrix to glm and extract transform components
        aiMatrix4x4 aiMat = node->mTransformation;
        glm::mat4 mat(
            aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
            aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
            aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
            aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
        );
    
        // Extract translation, rotation, and scale from the matrix
        glm::vec3 translation, scale, skew;
        glm::vec4 perspective;
        glm::quat rotation;
        glm::decompose(mat, scale, rotation, translation, skew, perspective);
    
        // Set transform properties
        currentTransform->setPosition(translation);
        currentTransform->setScale(scale);
    
        // Convert glm::quat to custom Quaternion
        gllib::Quaternion customQuat;
        customQuat.w = rotation.w;
        customQuat.x = rotation.x;
        customQuat.y = rotation.y;
        customQuat.z = rotation.z;
        currentTransform->setRotation(customQuat);
    
        // Initialize AABB for this node
        glm::vec3 nodeMinAABB(FLT_MAX);
        glm::vec3 nodeMaxAABB(-FLT_MAX);
        bool hasGeometry = false;
    
        // Process meshes for this node and associate them with the current transform
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Mesh processedMesh = processMesh(mesh, scene, meshes, gamma);
            
            // Associate this mesh with the current transform
            processedMesh.associatedTransform = currentTransform;
    
            // Update node AABB
            if (processedMesh.minAABB != processedMesh.maxAABB)
            {
                if (!hasGeometry)
                {
                    nodeMinAABB = processedMesh.minAABB;
                    nodeMaxAABB = processedMesh.maxAABB;
                    hasGeometry = true;
                }
                else
                {
                    nodeMinAABB = glm::min(nodeMinAABB, processedMesh.minAABB);
                    nodeMaxAABB = glm::max(nodeMaxAABB, processedMesh.maxAABB);
                }
            }
    
            // Update global AABB
            minAABB = glm::min(minAABB, processedMesh.minAABB);
            maxAABB = glm::max(maxAABB, processedMesh.maxAABB);
    
            meshes.push_back(processedMesh);
        }
    
        // Set local AABB for this transform
        if (hasGeometry)
        {
            currentTransform->localAABBMin = nodeMinAABB;
            currentTransform->localAABBMax = nodeMaxAABB;
        }
        else
        {
            // No geometry in this node, set minimal AABB
            currentTransform->localAABBMin = glm::vec3(-0.1f);
            currentTransform->localAABBMax = glm::vec3(0.1f);
        }
    
        // Recursively process children
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, meshes, gamma, minAABB, maxAABB,
                       rootTransform, currentTransform);
        }
    }

    Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, std::vector<Mesh>& meshes, bool gamma)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        glm::vec3 minAABB(FLT_MAX);
        glm::vec3 maxAABB(-FLT_MAX);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // Update AABB
            minAABB = glm::min(minAABB, vector);
            maxAABB = glm::max(maxAABB, vector);

            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse",
                                                                gamma);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular",
                                                                 gamma);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", gamma);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        std::vector<Texture> heightMaps =
            loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", gamma);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        Mesh result = Mesh(vertices, indices, textures);
        result.minAABB = minAABB;
        result.maxAABB = maxAABB;
        return result;
    }


    std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName,
                                                           bool gamma)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
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
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), directory, gamma);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    unsigned TextureFromFile(const char* path, const std::string& directory, bool gamma)
    {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);
        stbi_set_flip_vertically_on_load(gamma);

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
}
