
#include "rootSignature/rootSignature.h"
#include "shader/shader.h"
#include "bufferView/bufferView.h"
#include "textureView/textureView.h"
#include "sampler/sampler.h"


namespace ym
{
	class DescriptorSet;
	class Shader;

	/*************************************************//**
	 * @brief ���[�g�V�O�l�`�������L�q�q
	*****************************************************/
	struct RootSignatureCreateDesc
	{
		Shader *pVS = nullptr;
		Shader *pPS = nullptr;
		Shader *pGS = nullptr;
		Shader *pDS = nullptr;
		Shader *pHS = nullptr;
		Shader *pCS = nullptr;
	};	// struct RootSignatureCreateDesc

	/*************************************************//**
	 * @brief ���[�g�V�O�l�`���C���X�^���X
	*****************************************************/
	class RootSignatureInstance
	{
		friend class RootSignatureManager;
		friend class RootSignatureHandle;

		struct Slot
		{
			int		vs = -1;
			int		ps = -1;
			int		gs = -1;
			int		hs = -1;
			int		ds = -1;
			int		cs = -1;
		};	// struct Slot

	private:
		~RootSignatureInstance()
		{
			rootSig_.Destroy();
		}

	private:
		RootSignature					rootSig_;
		std::map<std::string, Slot>		slotMap_;
		std::atomic<int>				referenceCounter_ = 0;
		bool							isGraphics_ = true;
	};	// struct RootSignatureInstance

	/*************************************************//**
	 * @brief ���[�g�V�O�l�`���n���h��
	*****************************************************/
	class RootSignatureHandle
	{
		friend class RootSignatureManager;

	public:
		RootSignatureHandle()
			: pManager_(nullptr), crc_(0), pInstance_(nullptr)
		{}
		RootSignatureHandle(RootSignatureHandle &h)
			: pManager_(h.pManager_), crc_(h.crc_), pInstance_(h.pInstance_)
		{
			if (pInstance_)
			{
				pInstance_->referenceCounter_++;
			}
		}
		~RootSignatureHandle()
		{
			Invalid();
		}

		RootSignatureHandle &operator=(RootSignatureHandle &h)
		{
			pManager_ = h.pManager_;
			crc_ = h.crc_;
			pInstance_ = h.pInstance_;
			if (pInstance_)
			{
				pInstance_->referenceCounter_++;
			}
			return *this;
		}

		bool IsValid() const
		{
			return pInstance_ != nullptr;
		}

		void Invalid();

		bool SetDescriptor(DescriptorSet *pDescSet, const char *name, ConstantBufferView &cbv);
		bool SetDescriptor(DescriptorSet *pDescSet, const char *name, TextureView &srv);
		bool SetDescriptor(DescriptorSet *pDescSet, const char *name, BufferView &srv);
		bool SetDescriptor(DescriptorSet *pDescSet, const char *name, Sampler &sam);
		bool SetDescriptor(DescriptorSet *pDescSet, const char *name, UnorderedAccessView &uav);

		RootSignature *GetRootSignature()
		{
			assert(IsValid());
			return &pInstance_->rootSig_;
		}

	private:
		RootSignatureHandle(RootSignatureManager *man, u32 crc, RootSignatureInstance *ins)
			: pManager_(man), crc_(crc), pInstance_(ins)
		{
			if (pInstance_)
			{
				pInstance_->referenceCounter_++;
			}
		}

	private:
		RootSignatureManager *pManager_ = nullptr;
		u32							crc_ = 0;
		RootSignatureInstance *pInstance_ = nullptr;
	};	// class RootSignatureHandle

	/*************************************************//**
	 * @brief ���[�g�V�O�l�`���}�l�[�W��
	*****************************************************/
	class RootSignatureManager
	{

	public:
		RootSignatureManager()
		{}
		~RootSignatureManager()
		{
			Destroy();
		}

		bool Init(Device *pDev);
		void Destroy();

		/**
		 * @brief ���[�g�V�O�l�`���𐶐�����
		 *
		 * ���ɐ����ς݂̏ꍇ�͎Q�ƃJ�E���g���A�b�v���ăn���h����n��
		*/
		RootSignatureHandle CreateRootSignature(const RootSignatureCreateDesc &desc);

		/**
		 * @brief ���[�g�V�O�l�`�����������
		*/
		void ReleaseRootSignature(u32 crc, RootSignatureInstance *pInst);

	private:
		Device *pDevice_ = nullptr;
		std::map<u32, RootSignatureInstance *>	instanceMap_;
	};
}	// namespace ym