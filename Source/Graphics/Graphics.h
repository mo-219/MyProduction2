#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>

#include "Graphics/Shader.h"
#include "Graphics/DebugRenderer.h"
#include "Graphics/LineRenderer.h"
#include "Graphics/ImGuiRenderer.h"

#define SCREEN_BLANK_X 100
#define SCREEN_BLANK_Y 100


enum class ModelShaderId
{
	Default,
	Phong,
	ShadowmapCaster,
	Cubic,


	Max
};

enum class SpriteShaderId
{
	Default,
	UVScroll,
	Mask,
	LuminanceExtraction,
	GaussianBlur,
	Finalpass,

	Max
};



// �O���t�B�b�N�X
class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();

	// �C���X�^���X�擾
	static Graphics& Instance() { return *instance; }

	// �f�o�C�X�擾
	ID3D11Device* GetDevice() const { return device.Get(); }

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

	// �X���b�v�`�F�[���擾
	IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	// �V�F�[�_�[�擾
	Shader* GetShader() const { return shader.get(); }


	// ���f���V�F�[�_�[�擾
	ModelShader* GetShader(ModelShaderId id) const { return modelShaders[static_cast<int>(id)].get(); }

	// �X�v���C�g�V�F�[�_�[�擾
	SpriteShader* GetShader(SpriteShaderId id) const { return spriteShaders[static_cast<int>(id)].get(); }


	// �X�N���[�����擾
	float GetScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	float GetScreenHeight() const { return screenHeight; }

	// �X�N���[���󔒕��擾
	float GetScreenBlankWidth() const { return screenBlankWidth; }

	// �X�N���[���󔒍����擾
	float GetScreenBlankHeight() const { return screenBlankHeight; }



	// �f�o�b�O�����_���擾
	DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

	// ���C�������_���擾
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	// ImGui�����_���擾
	ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }

	// �~���[�e�b�N�X�擾
	std::mutex& GetMutex() { return mutex; }

	void SetRenderPostion(DirectX::XMFLOAT2 pos) { renderPostion = pos; }
	DirectX::XMFLOAT2 GetRenderPosition() { return renderPostion; }

private:
	static Graphics*								instance;

	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

	std::unique_ptr<Shader>							shader;
	std::unique_ptr<ModelShader>					modelShaders[static_cast<int>(ModelShaderId::Max)];
	std::unique_ptr<SpriteShader>					spriteShaders[static_cast<int>(SpriteShaderId::Max)];


	std::unique_ptr<DebugRenderer>					debugRenderer;
	std::unique_ptr<LineRenderer>					lineRenderer;
	std::unique_ptr<ImGuiRenderer>					imguiRenderer;

	float	screenWidth;
	float	screenHeight;

	float	screenBlankWidth;
	float	screenBlankHeight;

	DirectX::XMFLOAT2 renderPostion = DirectX::XMFLOAT2(0, 0);

	std::mutex										mutex;
};

