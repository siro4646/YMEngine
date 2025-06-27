#pragma once

namespace ym {
    class Object;

    namespace mesh {
        class Mesh;
        class MeshInstance;
		class Skeleton;
        class MeshLoader {
        public:
            struct LoadedMeshInfo {
                ym::mesh::Mesh mesh;
                bool           isSkinned;
                std::string    name;
            };

            //static std::shared_ptr<MeshInstance> LoadMesh(const std::string &filePath);
            static void LoadMeshHierarchy(std::shared_ptr<Object> parent, const std::string &filePath);
			static string GetFilePath() { return filePath_; }
        private:

            // NormalizeScale �� center/scale ��Ԃ��悤�ύX
            static void NormalizeScale(
                Mesh &mesh,
                DirectX::XMVECTOR &outCenter,   // ���_���ړ�����ۂ̒��S�iWorldSpace�j
                float &outScale     // �������K�p����X�P�[���W��
            );

            // Skeleton �� bind-pose �ɂ����� center/scale ��K�p����w���p�[
            static void AdjustSkeletonForNormalize(
                std::shared_ptr<Skeleton> skeleton,
                DirectX::XMVECTOR        center,
                float                    scale
            );
            static void LoadMesh(Mesh &dst, const struct aiMesh *src ,const aiScene* aiScene, bool inverseU, bool inverseV);
            static void NormalizeScale(Mesh &mesh);
            static std::shared_ptr<Skeleton> BuildSkeleton(const aiScene *scene);
            static void ProcessNode(const aiNode *node, const aiScene *scene, std::shared_ptr<Object> parent);
            static void BuildBoneHierarchyRecursive(std::shared_ptr<Object> parent,
                const aiNode *node,
                std::unordered_map<std::string,std::shared_ptr<Object>> &boneMap);
             private:
            static string filePath_;
			static float minX, minY, minZ;
			static float maxX, maxY, maxZ;
        };
    }
}