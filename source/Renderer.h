#pragma once
#include "ColorRGB.h"
#include "DataTypes.h"
#include "Mesh.h"
#include "Camera.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

	private:
		SDL_Window* m_pWindow{};

		Camera* m_pCamera{};
		
		int m_Width{};
		int m_Height{};

		float m_AspectRatio{};
		
		bool m_IsInitialized{ false };

		std::vector<Vertex_PosCol> m_Vertices{};
		std::vector<uint32_t> m_Indices{};

		Mesh* m_pMesh{};

		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		IDXGISwapChain* m_pSwapChain{};

		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};

		ID3D11RenderTargetView* m_pRenderTargetView{};
		ID3D11Resource* m_pRenderTargetBuffer{};
		
		//DIRECTX
		HRESULT InitializeDirectX();
		//...

	};
}
