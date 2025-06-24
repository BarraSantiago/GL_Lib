#include "ModelImporter.h"

namespace gllib
{
    ModelImporter::ModelImporter()
    {
    }

    ModelImporter::~ModelImporter()
    {
        cleanup();
    }

    bool ModelImporter::loadModel(const std::string& filePath)
    {
        // Clean up any existing data
        cleanup();

        // Create an instance of the Importer class
        Assimp::Importer importer;

        // Import the model with some common post-processing
        // triangulate: convert all geometry to triangles
        // GenNormals: create normals if model doesn't have them
        // FlipUVs: flip texture coords (needed for OpenGL)
        const aiScene* scene = importer.ReadFile(filePath,
                                                 aiProcess_Triangulate |
                                                 aiProcess_GenSmoothNormals |
                                                 aiProcess_FlipUVs);

        // Check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
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

        // Create the OpenGL buffers using the renderer
        std::vector<float> vertexData;
        for (const auto& v : result.vertices)
        {
            // Position
            vertexData.push_back(v.position.x);
            vertexData.push_back(v.position.y);
            vertexData.push_back(v.position.z);

            // Normal
            vertexData.push_back(v.normal.x);
            vertexData.push_back(v.normal.y);
            vertexData.push_back(v.normal.z);

            // Texture Coordinates
            vertexData.push_back(v.texCoords.x);
            vertexData.push_back(v.texCoords.y);
        }

        // Convert to int array for EBO
        std::vector<int> indexData(result.indices.begin(), result.indices.end());

        // Create render data
        result.renderData = gllib::Renderer::createRenderData(
            vertexData.data(),
            static_cast<GLsizei>(vertexData.size() * sizeof(float)),
            indexData.data(),
            static_cast<GLsizei>(indexData.size() * sizeof(int))
        );

        return result;
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
        return gllib::RenderData{0, 0, 0}; // Return empty render data if index is out of bounds
    }

    size_t ModelImporter::getIndicesCount(size_t meshIndex) const
    {
        if (meshIndex < meshes.size())
        {
            return meshes[meshIndex].indices.size();
        }
        return 0;
    }

    void ModelImporter::cleanup()
    {
        // Clean up OpenGL resources
        for (auto& mesh : meshes)
        {
            gllib::Renderer::destroyRenderData(mesh.renderData);
        }
        meshes.clear();
    }
}
