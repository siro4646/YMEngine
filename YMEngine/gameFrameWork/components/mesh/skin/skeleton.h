
#pragma once
namespace ym
{
	namespace mesh
	{
            // �P�ʍs��萔
        static const DirectX::XMFLOAT4X4 IDENTITY_MAT = {
            1, 0, 0, 0,   // row0
            0, 1, 0, 0,   // row1
            0, 0, 1, 0,   // row2
            0, 0, 0, 1    // row3
        };
        struct SkeletonJoint
        {

            std::string name;
            int parentIndex = -1; // -1 �� root
            //������
            DirectX::XMFLOAT4X4 localBindPose = IDENTITY_MAT; // �� �����l��P�ʍs���
            DirectX::XMFLOAT4X4 inverseGlobalPose = IDENTITY_MAT;
            DirectX::XMFLOAT4X4 inverseBindPose = IDENTITY_MAT;
            // ���s���ɕK�v�ł���΁c
            XMFLOAT4X4 currentPoseMatrix;
        };

        class Skeleton
        {
        public:
            std::vector<SkeletonJoint> joints;

            int FindJointIndexByName(const std::string &name) const;
        };

	}

}