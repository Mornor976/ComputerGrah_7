#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"

class Model {
public:
    Model(const std::string& path) {
        loadModel(path);
    }

    enum MeshName {
        OBJ1,
        MAT4,
        OBJ3,
        OBJ4,
        OBJ5,
        MAT6,
        MAT5,
        UNKNOWN
    };

    MeshName GetMeshName(const std::string& name) {
        if (name == "obj1") return OBJ1;
        if (name == "mat4") return MAT4;
        if (name == "obj3") return OBJ3;
        if (name == "obj4") return OBJ4;
        if (name == "obj5") return OBJ5;
        if (name == "mat6") return MAT6;
        if (name == "mat5") return MAT5;
        return UNKNOWN;
    }

    void Draw(Shader& shader, glm::mat4 transformOZ, glm::mat4 transformXY,
        glm::mat4 transformXZ1, glm::mat4 transformXZ2) {

        for (unsigned int i = 0; i < meshes.size(); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            switch (GetMeshName(meshNames[i])) {
            case OBJ1:
                model = model * transformOZ;
                break;

            case MAT4:
            case OBJ3:
                model = model * transformOZ * transformXY;
                break;

            case OBJ5:
            case MAT6: {
                glm::vec3 pivotOBJ5(2.29485f, 0.0053f, 0.35437f);//2.29485f, 0.0f, -0.0053f
                model = glm::translate(model, pivotOBJ5);
                model = model * transformOZ * transformXY * transformXZ1;
                model = glm::translate(model, -pivotOBJ5);
                break;
            }

            case OBJ4:
            case MAT5: {
                glm::vec3 pivotPoint(2.29485f, -0.189765f, 0.35437f);//(2.29485f, 0.35437f, 0.189765f);
                model = glm::translate(model, pivotPoint);
                model = model * transformOZ * transformXY * transformXZ2;
                model = glm::translate(model, -pivotPoint);
                break;
            }

            default:
                break;
            }

            shader.SetUniform("model", glm::value_ptr(model));
            meshes[i].Draw(shader);
        }
    }



private:
    std::vector<Mesh> meshes;
    std::vector<std::string> meshNames; // Храним имена мешей
    std::string directory;

    void loadModel(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
            meshNames.push_back(mesh->mName.C_Str()); // Сохраняем имя меша
            std::cout << "Loaded mesh: " << mesh->mName.C_Str() << std::endl; // Для отладки
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }
    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            vertex.Position = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );
            vertex.Normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        return Mesh(vertices, indices);
    }
};

#endif