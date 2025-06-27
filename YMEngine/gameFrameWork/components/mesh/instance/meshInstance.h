#pragma once
namespace ym
{
    namespace mesh
    {
        class MeshInstance {
        public:
            MeshInstance(Mesh source)
            {
                mesh_ = std::move(source);
            }

            inline const std::vector<Vertex3D> &GetVertices() const
            {
                return mesh_.Vertices;
            }
            inline  const std::vector<u32> &GetIndices() const
            {
                return mesh_.Indices;
            }
            inline const Mesh &GetMesh() const {
                return mesh_;
            }
			inline Mesh &GetRawMesh() {
				return mesh_;
			}

            // â¡çHÅEï“èWån
            //void ApplyCuttingPlane(...);
            //void ApplyDeformation(...);

        private:
            Mesh mesh_;
        };
    }
}