#include "meshLoader.h"
#include "gameFrameWork/components/mesh/instance/meshInstance.h"
#include "gameObject/gameObject.h"
#include "gameFrameWork/components/mesh/filter/meshFilter.h"
#include "gameFrameWork/components/mesh/renderer/meshRenderer.h"
#include "gameFrameWork/components/mesh/skin/skeleton.h"
#include "gameFrameWork/components/mesh/skin/skinnedMeshRenderer.h"

#include "gameFrameWork/material/material2.h"

#include "gameObject/gameObjectManager.h"

namespace ym::mesh {
	// �����o�ϐ��̏�����
	float MeshLoader::minX = FLT_MAX;
	float MeshLoader::minY = FLT_MAX;
	float MeshLoader::minZ = FLT_MAX;
	float MeshLoader::maxX = -FLT_MAX;
	float MeshLoader::maxY = -FLT_MAX;
	float MeshLoader::maxZ = -FLT_MAX;
	std::string MeshLoader::filePath_ = ""; // �t�@�C���p�X��ۑ�����ÓI�ϐ�
    std::string GetTexturePath(const aiMaterial *material, aiTextureType type)
    {
        if (!material) return "";

        if (material->GetTextureCount(type) > 0)
        {
            aiString path;
            if (material->GetTexture(type, 0, &path) == AI_SUCCESS)
            {
                return std::string(path.C_Str());
            }
        }
        return "";
    }
    static std::shared_ptr<ym::material::Material>
        CreateMaterialFromAi(aiMaterial *aiMat)
    {
        auto m = std::make_shared<ym::material::Material>();
        m->Init("pbrVS", "pbrPS");

        auto setTex = [&](aiTextureType t, const char *slot)
            {
                if (aiMat->GetTextureCount(t) == 0) return;
                aiString p;  if (aiMat->GetTexture(t, 0, &p) != AI_SUCCESS)
                {
					if (t == aiTextureType_DIFFUSE) {
						m->SetTexture(slot, "white"); // �f�t�H���g�̔��e�N�X�`����ݒ�
					}
                    else if (t == aiTextureType_OPACITY) {
						m->SetTexture(slot, "white"); // �}�X�N�e�N�X�`�������ɐݒ�
					}
                    else if (t == aiTextureType_SPECULAR) {
						m->SetTexture(slot, "black"); // �X�y�L�����e�N�X�`�������ɐݒ�
					}

                }
                else {
                    std::filesystem::path modelPath = MeshLoader::GetFilePath();
                    std::filesystem::path directory = modelPath.parent_path();
					p = ym::NormalizePath(p.C_Str()); // �p�X�𐳋K��
                    std::string textureName = p.C_Str();
                    // �e�N�X�`���̃p�X���f�B���N�g���ƌ���
                    std::filesystem::path fullTexturePath = directory / textureName;
                    ym::DebugLog("Type: %s  Texture Path: %s\n", slot, fullTexturePath.string().c_str());
                    if (!std::filesystem::exists(fullTexturePath))
                    {
                        ym::DebugLog("Missing Texture: %s\n", fullTexturePath.string().c_str());
                    }
                    m->SetTexture(slot, fullTexturePath.string());
                }
            };
        setTex(aiTextureType_DIFFUSE, "g_texture");
        setTex(aiTextureType_OPACITY, "g_maskTexture");
        setTex(aiTextureType_SPECULAR, "g_specularTexture");

        return m;
    }
    // ============================================================================
// 2) Assimp �� Mesh �ϊ��� SubMesh ��������
//    �iLoadMeshHierarchy ���� aiMesh ���Ɏ��s�j
// ============================================================================

    void BuildSubMeshes(const aiScene *scene, ym::mesh::Mesh &dstMesh)
    {
        dstMesh.SubMeshes.clear();

        // �� aiMesh �́u�}�e���A�����Ɓv�ɕ�������Ă��邱�Ƃ�����
        u32 runningIndexOffset = 0;

        for (u32 m = 0; m < scene->mNumMeshes; ++m)
        {
            const aiMesh *aMesh = scene->mMeshes[m];

            ym::mesh::SubMesh sub{};
            sub.indexOffset = runningIndexOffset;
            sub.indexCount = aMesh->mNumFaces * 3;
            sub.materialIndex = aMesh->mMaterialIndex;          // �� aiMaterial �̓Y����

            dstMesh.SubMeshes.push_back(sub);

            // ---------- ���_ & �C���f�b�N�X���R�s�[ ----------
            // ���_
            for (u32 v = 0; v < aMesh->mNumVertices; ++v)
            {
                Vertex3D vtx{};
                // �c Position / Normal / UV �̋l�ߍ��� �c
                dstMesh.Vertices.push_back(vtx);
            }
            // �C���f�b�N�X
            for (u32 f = 0; f < aMesh->mNumFaces; ++f)
            {
                const aiFace &face = aMesh->mFaces[f];
                dstMesh.Indices.push_back(face.mIndices[0]);
                dstMesh.Indices.push_back(face.mIndices[1]);
                dstMesh.Indices.push_back(face.mIndices[2]);
            }

            runningIndexOffset += sub.indexCount; // ���� SubMesh �̊J�n�ʒu��
        }
    }

    aiNode *FindNodeByName(aiNode *node, const std::string &targetName)
    {
        if (node->mName.C_Str() == targetName)
            return node;

        for (u32 i = 0; i < node->mNumChildren; ++i)
        {
            if (auto found = FindNodeByName(node->mChildren[i], targetName))
                return found;
        }

        return nullptr;
    }

    // �⏕: Bone���ɑΉ�����Node���ċA�I�ɒT���i������v�Ή��j
    aiNode *FindNodeByBoneName(aiNode *node, const std::string &boneName)
    {
        std::string nodeName = node->mName.C_Str();

        // ���S��v�܂��͕�����v�iAssimpFbx�g���΍�j
        if (nodeName == boneName || nodeName.find(boneName) != std::string::npos)
            return node;

        for (u32 i = 0; i < node->mNumChildren; ++i)
        {
            aiNode *result = FindNodeByBoneName(node->mChildren[i], boneName);
            if (result)
                return result;
        }

        return nullptr;
    }



   /* std::shared_ptr<MeshInstance> MeshLoader::LoadMesh(const std::string &filePath) {
        if (filePath.empty()) return nullptr;
        minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
        maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

        Mesh combinedMesh;
        Assimp::Importer importer;

        int flag = 0;
        flag |= aiProcess_Triangulate;
        flag |= aiProcess_PreTransformVertices;
        flag |= aiProcess_CalcTangentSpace;
        flag |= aiProcess_GenSmoothNormals;
        flag |= aiProcess_GenUVCoords;
        flag |= aiProcess_RemoveRedundantMaterials;
        flag |= aiProcess_OptimizeMeshes;

        const aiScene *scene = importer.ReadFile(filePath, flag);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) return nullptr;

        u32 indexOffset = 0;
        for (u32 i = 0; i < scene->mNumMeshes; ++i) {
            const aiMesh *assimpMesh = scene->mMeshes[i];
            Mesh tempMesh;
            LoadMesh(tempMesh, assimpMesh, false, false);

            if (!tempMesh.Vertices.empty()) {
                combinedMesh.Vertices.insert(combinedMesh.Vertices.end(),
                    tempMesh.Vertices.begin(), tempMesh.Vertices.end());

                for (auto idx : tempMesh.Indices) {
                    combinedMesh.Indices.push_back(idx + indexOffset);
                }

                indexOffset += static_cast<u32>(tempMesh.Vertices.size());
            }
        }

        NormalizeScale(combinedMesh);
        return std::make_shared<MeshInstance>(combinedMesh);
    }*/

    void MeshLoader::LoadMeshHierarchy(std::shared_ptr<Object> parent, const std::string &filePath)
    {
        if (!parent || filePath.empty()) return;
		filePath_ = filePath; // �O���[�o���ϐ��ɕۑ��i��Ŏg���j
        // AABB������
        minX = FLT_MAX; minY = FLT_MAX; minZ = FLT_MAX;
        maxX = -FLT_MAX; maxY = -FLT_MAX; maxZ = -FLT_MAX;

        Assimp::Importer importer;
        unsigned int flags =
            aiProcess_Triangulate |
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_GenUVCoords |
            aiProcess_RemoveRedundantMaterials |
            aiProcess_OptimizeMeshes;

        const aiScene *scene = importer.ReadFile(filePath, flags);
        if (!scene || !scene->mRootNode) return;

        for (u32 i = 0; i < scene->mNumMeshes; ++i)
        {
            const aiMesh *aMesh = scene->mMeshes[i];
            for (u32 v = 0; v < aMesh->mNumVertices; ++v)
            {
                const auto &pos = aMesh->mVertices[v];
                minX = std::min(minX, pos.x);
                minY = std::min(minY, pos.y);
                minZ = std::min(minZ, pos.z);
                maxX = std::max(maxX, pos.x);
                maxY = std::max(maxY, pos.y);
                maxZ = std::max(maxZ, pos.z);
            }
        }

        for (u32 i = 0; i < scene->mRootNode->mNumChildren; ++i)
        {
            ProcessNode(scene->mRootNode->mChildren[i], scene, parent);
        }
    }
    void MeshLoader::NormalizeScale(
        Mesh &mesh,
        XMVECTOR &outCenter,
        float &outScale)
    {
        // ���� 1) �o�E���f�B���O�{�b�N�X���璆�S�ƃX�P�[�����v�Z ����
        float sizeX = maxX - minX;
        float sizeY = maxY - minY;
        float sizeZ = maxZ - minZ;
        float maxDim = std::max({ sizeX, sizeY, sizeZ });
        outScale = 1.0f / maxDim;  // �ڕW�T�C�Y=1�Ɖ���

        float cx = (minX + maxX) * 0.5f;
        float cy = (minY + maxY) * 0.5f;
        float cz = (minZ + maxZ) * 0.5f;
        outCenter = XMVectorSet(cx, cy, cz, 0);

        // ���� 2) ���b�V�����_�� center�����_���X�P�[�����߂� ����
        XMMATRIX TtoOrigin = XMMatrixTranslationFromVector(-outCenter);
        XMMATRIX S = XMMatrixScaling(outScale, outScale, outScale);
        XMMATRIX adjust = TtoOrigin * S * XMMatrixTranslationFromVector(outCenter);

        for (auto &v : mesh.Vertices)
        {
            XMVECTOR p = XMLoadFloat3(&v.Position);
            p = XMVector3Transform(p, adjust);
            XMStoreFloat3(&v.Position, p);
        }
    }

    void MeshLoader::AdjustSkeletonForNormalize(
        std::shared_ptr<Skeleton> skeleton,
        XMVECTOR center,
        float scale)
    {
        // ���� 1) ���b�V���Ɠ����s����쐬 ����
        XMMATRIX TtoOrigin = XMMatrixTranslationFromVector(-center);
        XMMATRIX S = XMMatrixScaling(scale, scale, scale);
        XMMATRIX adjust = TtoOrigin * S * XMMatrixTranslationFromVector(center);

        // ���� 2) �e�W���C���g�� localBindPose �ɓK�p ����
        auto &joints = skeleton->joints;
        for (auto &joint : joints)
        {
            XMMATRIX local = XMLoadFloat4x4(&joint.localBindPose);
            local = adjust * local;
            XMStoreFloat4x4(&joint.localBindPose, local);
        }

        // ���� 3) globalBindPose, inverseGlobalPose ���č\�z ����
        size_t J = joints.size();
        std::vector<XMMATRIX> globalBind(J);
        for (size_t i = 0; i < J; ++i)
        {
            XMMATRIX local = XMLoadFloat4x4(&joints[i].localBindPose);
            int p = joints[i].parentIndex;
            globalBind[i] = (p >= 0)
                ? local * globalBind[p]
                : local;
        }
        for (size_t i = 0; i < J; ++i)
        {
            XMMATRIX inv = XMMatrixInverse(nullptr, globalBind[i]);
            XMStoreFloat4x4(&joints[i].inverseGlobalPose, inv);
        }
    }

    XMMATRIX ConvertMatrix(const aiMatrix4x4 &m)
    {
        XMMATRIX mat = XMMatrixSet(
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4
        );
        return XMMatrixTranspose(mat); // �� Assimp��D�� �� DirectX�s�D���
    }



    void MeshLoader::LoadMesh(Mesh &dst, const aiMesh *src,const aiScene *aiScene, bool inverseU, bool inverseV) {
        aiVector3D zero3D(0, 0, 0);
        aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

        dst.Vertices.resize(src->mNumVertices);
		dst.BoneWeights.resize(src->mNumVertices);

        for (auto i = 0u; i < src->mNumVertices; ++i)
        {
            auto position = &(src->mVertices[i]);
            auto normal = &(src->mNormals[i]);
            auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;
            auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
            auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;


            // ���]�I�v�V��������������UV�𔽓]������
            if (inverseU)
            {
                uv->x = 1 - uv->x;
            }
            if (inverseV)
            {
                uv->y = 1 - uv->y;
            }

            Vertex3D vertex = {};
            vertex.Position = DirectX::XMFLOAT3(position->x, position->y, position->z);
            vertex.Normal = DirectX::XMFLOAT3(normal->x, normal->y, normal->z);
            vertex.UV = DirectX::XMFLOAT2(uv->x, uv->y);
            vertex.Tangent = DirectX::XMFLOAT3(tangent->x, tangent->y, tangent->z);
            vertex.Color = DirectX::XMFLOAT4(color->r, color->g, color->b, color->a);

            minX = std::min(minX, vertex.Position.x);
            minY = std::min(minY, vertex.Position.y);
            minZ = std::min(minZ, vertex.Position.z);

            maxX = std::max(maxX, vertex.Position.x);
            maxY = std::max(maxY, vertex.Position.y);
            maxZ = std::max(maxZ, vertex.Position.z);

            dst.Vertices[i] = vertex;
        }

        dst.Indices.resize(src->mNumFaces * 3);

        for (auto i = 0u; i < src->mNumFaces; ++i)
        {
            const auto &face = src->mFaces[i];

            dst.Indices[i * 3 + 0] = face.mIndices[0];
            dst.Indices[i * 3 + 1] = face.mIndices[1];
            dst.Indices[i * 3 + 2] = face.mIndices[2];
        }
        if (src->HasBones())
        {
            ym::DebugLog("=== Skinned Mesh ===\n");
            ym::DebugLog("Bone Count: %d\n", src->mNumBones);
            for (u32 boneIdx = 0; boneIdx < src->mNumBones; ++boneIdx)
            {
                const aiBone *bone = src->mBones[boneIdx];

                // �{�[������ۑ�
                dst.BoneNames.push_back(bone->mName.C_Str());

                // �I�t�Z�b�g�s��i= bind pose �̋t�s��j
                aiMatrix4x4 offset = bone->mOffsetMatrix;
                XMMATRIX m = XMLoadFloat4x4((XMFLOAT4X4 *)&offset); // Assimp�s���float[4][4]
                XMFLOAT4X4 bindPoseInv;
                XMStoreFloat4x4(&bindPoseInv, m);
                dst.BindPoseInverse.push_back(bindPoseInv);

                // ���̃{�[���ɉe�����󂯂钸�_�Q
                for (u32 i = 0; i < bone->mNumWeights; ++i)
                {
                    const aiVertexWeight &vw = bone->mWeights[i];
                    u32 vertexId = vw.mVertexId;
                    float weight = vw.mWeight;

                    BoneWeight &bw = dst.BoneWeights[vertexId];

                    // 4�܂Œǉ�
                    for (int j = 0; j < 4; ++j)
                    {
                        if (bw.weights[j] == 0.0f)
                        {
                            bw.indices[j] = boneIdx;  // index �� BoneNames[] �̃C���f�b�N�X
                            bw.weights[j] = weight;
                            break;
                        }
                    }
                }
            }

        }

        /*for (unsigned int i = 0; i < src->mNumVertices; ++i) {
            auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;
            Vertex3D v{};
            v.Position = { src->mVertices[i].x, src->mVertices[i].y, src->mVertices[i].z };
            v.Normal = { src->mNormals[i].x, src->mNormals[i].y, src->mNormals[i].z };
            v.UV = { inverseU ? 1 - uv->x : uv->x, inverseV ? 1 - uv->y : uv->y };
            dst.Vertices.push_back(v);
        }
        for (unsigned int i = 0; i < src->mNumFaces; ++i) {
            const auto &f = src->mFaces[i];
            if (f.mNumIndices == 3) {
                dst.Indices.push_back(f.mIndices[0]);
                dst.Indices.push_back(f.mIndices[1]);
                dst.Indices.push_back(f.mIndices[2]);
            }
        }*/
        // BoneName �� SkeletonIndex �Ċ��蓖��
        std::shared_ptr<Skeleton> skeleton = BuildSkeleton(aiScene);
        if (skeleton)
        {
            std::unordered_map<std::string, int> nameToIndex;
            for (int i = 0; i < skeleton->joints.size(); ++i)
                nameToIndex[skeleton->joints[i].name] = i;

            for (u32 v = 0; v < dst.BoneWeights.size(); ++v)
            {
                for (int j = 0; j < 4; ++j)
                {
                    int oldIndex = dst.BoneWeights[v].indices[j];
                    if (oldIndex < dst.BoneNames.size())
                    {
                        const std::string &boneName = dst.BoneNames[oldIndex];
                        auto it = nameToIndex.find(boneName);
                        if (it != nameToIndex.end())
                        {
                            dst.BoneWeights[v].indices[j] = it->second;
                        }
                        else
                        {
                            dst.BoneWeights[v].indices[j] = 0; // ������Ȃ����0��
                        }
                    }
                }
            }
        }

    }

    void MeshLoader::NormalizeScale(Mesh &mesh) {

        float centerX;
        float centerY;
        float centerZ;
        float sizeX;
        float sizeY;
        float sizeZ;
        float maxDimension;
        float targetSize;
        float scaleFactor;

        centerX = (minX + maxX) / 2.0f;
        centerY = (minY + maxY) / 2.0f;
        centerZ = (minZ + maxZ) / 2.0f;
        sizeX = maxX - minX;
        sizeY = maxY - minY;
        sizeZ = maxZ - minZ;
        maxDimension = std::max({ sizeX, sizeY, sizeZ });
        targetSize = 1.0f; // �ڕW�̑傫��
        scaleFactor = targetSize / maxDimension;// �ڕW�̑傫���ɍ��킹�邽�߂̃X�P�[�� return;
        // �X�P�[����K�p

        for (auto &vertex : mesh.Vertices)
        {

            vertex.Position.x -= centerX;
            vertex.Position.y -= centerY;
            vertex.Position.z -= centerZ;
        }
        for (auto &vertex : mesh.Vertices) {

            vertex.Position.x *= scaleFactor;
            vertex.Position.y *= scaleFactor;
            vertex.Position.z *= scaleFactor;

        }


    }

    // meshLoader.cpp

    std::shared_ptr<Skeleton> MeshLoader::BuildSkeleton(const aiScene *scene) {
        auto skeleton = std::make_shared<Skeleton>();
        std::unordered_map<std::string, int> nameToIndex;

        // (A) �e aiBone �� offsetMatrix ���E���� joint �ɓo�^
        for (u32 mi = 0; mi < scene->mNumMeshes; ++mi) {
            const aiMesh *m = scene->mMeshes[mi];
            for (u32 bi = 0; bi < m->mNumBones; ++bi) {
                const aiBone *bone = m->mBones[bi];
                std::string bn = bone->mName.C_Str();
                if (!nameToIndex.count(bn)) {
                    int idx = skeleton->joints.size();
                    nameToIndex[bn] = idx;

                    SkeletonJoint joint;
                    joint.name = bn;
                    // ������ aiBone::mOffsetMatrix ���t�o�C���h�s��Ƃ��Ă��̂܂ܕۑ�
                    {
                        XMMATRIX off = ConvertMatrix(bone->mOffsetMatrix);
                        XMStoreFloat4x4(&joint.inverseBindPose, off);
                    }

                    skeleton->joints.push_back(joint);
                }
            }
        }

        // (B) �m�[�h�K�w�� DFS ���� localBindPose�^parentIndex ��ݒ�
        std::function<void(aiNode *, int, const XMMATRIX &)> dfs =
            [&](aiNode *node, int parentIdx, const XMMATRIX &parentG) {
            std::string nm = node->mName.C_Str();
            XMMATRIX localM = ConvertMatrix(node->mTransformation);
            XMMATRIX globalM = localM * parentG;
            auto it = nameToIndex.find(nm);
            if (it != nameToIndex.end()) {
                auto &joint = skeleton->joints[it->second];
                XMStoreFloat4x4(&joint.localBindPose, localM);
                joint.parentIndex = parentIdx;
            }
            for (u32 i = 0; i < node->mNumChildren; ++i)
                dfs(node->mChildren[i],
                    (it != nameToIndex.end() ? it->second : parentIdx),
                    globalM);
            };
        dfs(scene->mRootNode, -1, XMMatrixIdentity());

        // (C) NormalizeScale ��p�� globalBindPose�^inverseGlobalPose ���\�z
        //     �i�O�񂲈ē����� AdjustSkeletonForNormalize ���ĂԂȂǁj

        return skeleton;
    }

    bool IsAssimpFbxDummyNode(const std::string &name) {
        return name.find("_$AssimpFbx$_") != std::string::npos;
    }

    void MeshLoader::ProcessNode(const aiNode *node, const aiScene *scene, std::shared_ptr<Object> parent)
    {
         std::string name = node->mName.C_Str();
        if (IsAssimpFbxDummyNode(name))
        {
            for (u32 i = 0; i < node->mNumChildren; ++i)
                ProcessNode(node->mChildren[i], scene, parent);
            return;
        }

        auto obj = std::make_shared<Object>();
		parent->objectManager->AddGameObject(obj,parent.get());
        obj->name = name;        
        //parent->AddChild(obj);
        if (node->mNumMeshes > 0)
        {
            ym::mesh::Mesh combinedMesh;
            u32 runningIndexOffset = 0;

            std::vector<std::shared_ptr<ym::material::Material>> subMats;
            for (u32 i = 0; i < node->mNumMeshes; ++i)
            {
                u32 meshIndex = node->mMeshes[i];
                const aiMesh *aMesh = scene->mMeshes[meshIndex];
				aiMaterial *mat = scene->mMaterials[aMesh->mMaterialIndex];
				subMats.push_back(CreateMaterialFromAi(mat));
				std::string diffuseTex = GetTexturePath(mat, aiTextureType_DIFFUSE);
				std::string maskTex = GetTexturePath(mat, aiTextureType_OPACITY);
				std::string specularTex = GetTexturePath(mat, aiTextureType_SPECULAR);
               // matInstance->SetTexture("g_texture", diffuseTex);
                //matInstance->SetTexture("g_maskTexture", maskTex);
               // matInstance->SetTexture("g_specularTexture", specularTex);

                
                ym::mesh::Mesh tempMesh;
                LoadMesh(tempMesh, aMesh, scene, false, false);

                ym::mesh::SubMesh sub;
                sub.indexOffset = static_cast<u32>(combinedMesh.Indices.size());
                sub.indexCount = static_cast<u32>(tempMesh.Indices.size());
                sub.materialIndex = aMesh->mMaterialIndex;
                combinedMesh.SubMeshes.push_back(sub);

                combinedMesh.Vertices.insert(combinedMesh.Vertices.end(), tempMesh.Vertices.begin(), tempMesh.Vertices.end());
                combinedMesh.BoneWeights.insert(combinedMesh.BoneWeights.end(), tempMesh.BoneWeights.begin(), tempMesh.BoneWeights.end());
                combinedMesh.BindPoseInverse.insert(combinedMesh.BindPoseInverse.end(), tempMesh.BindPoseInverse.begin(), tempMesh.BindPoseInverse.end());
                combinedMesh.BoneNames.insert(combinedMesh.BoneNames.end(), tempMesh.BoneNames.begin(), tempMesh.BoneNames.end());
                for (auto idx : tempMesh.Indices)
                    combinedMesh.Indices.push_back(idx + runningIndexOffset);

                runningIndexOffset += static_cast<u32>(tempMesh.Vertices.size());
            }
            XMVECTOR center;
            float     scale;
            NormalizeScale(combinedMesh);
            auto instance = std::make_shared<MeshInstance>(combinedMesh);
            auto mf = obj->AddComponent<MeshFilter>();
            mf->SetMeshInstance(instance);

            bool isSkinned = !combinedMesh.BoneWeights.empty()
                && combinedMesh.BoneWeights.size() == combinedMesh.Vertices.size()
                && !combinedMesh.BindPoseInverse.empty();

            if (isSkinned)
            {
                //ym::DebugLog("SkinnedMeshRenderer ��ǉ�: %s\n", obj->name.c_str());
                auto smr = obj->AddComponent<SkinnedMeshRenderer>();
                smr->SetMeshFilter(mf);
                for (auto &m : subMats)smr->PushMaterial(m);
                //smr->SetMaterialCount(static_cast<u32>(combinedMesh.SubMeshes.size()));

                // Skeleton ���\�z
                //auto skeleton = BuildSkeleton(scene);

                // �� �����ŃX�P�[�����O�ɍ��킹�� Skeleton �𒲐��I
                //AdjustSkeletonForNormalize(skeleton, center, scale);

                //smr->SetSkeleton(skeleton);
            }
            else
            {
               // ym::DebugLog("MeshRenderer ��ǉ�: %s\n", obj->name.c_str());
                auto mr = obj->AddComponent<MeshRenderer>();
                for(auto &m:subMats) mr->PushMaterial(m);
               // mr->SetMaterialCount(static_cast<u32>(combinedMesh.SubMeshes.size()));
                //for (auto &m : subMats)   mr->PushMaterial(m.get());
            }
        }

        for (u32 i = 0; i < node->mNumChildren; ++i)
        {
            ProcessNode(node->mChildren[i], scene, obj);
        }
    }

}
