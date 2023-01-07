#pragma once
#include "Texture.h"

class Effect
{
public:

	Effect(ID3D11Device* pDeviceInput, const std::wstring& pathInput);
	virtual ~Effect();

	// -----------------------------------------------
	// Copy/move constructors and assignment operators
	// -----------------------------------------------
	Effect(const Effect& other)					= delete;
	Effect(Effect&& other) noexcept				= delete;
	Effect& operator=(const Effect& other)		= delete;
	Effect& operator=(Effect&& other) noexcept  = delete;

	//------------------------------------------------
	// Public member functions						
	//------------------------------------------------
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	ID3DX11EffectTechnique* GetTechniquePtr();

	void SetWorldViewProjectionMatrix(const dae::Matrix& worldViewProjectionMatrix);
	void SetDiffuseMap(dae::Texture* pDiffuseTexture);
	void ToggleSampleState();

private:

	//------------------------------------------------
	// Member variables						
	//------------------------------------------------

	const int NROFSAMPLESTATES{3};


	enum class sampleState
	{
		point,
		linear,
		anisotropic
	};
	sampleState m_SampleState = sampleState::point;

	ID3DX11Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pActiveTechnique{nullptr};

	ID3DX11EffectTechnique* m_pPointTechnique{ nullptr };
	ID3DX11EffectTechnique* m_pLinearTechnique{ nullptr };
	ID3DX11EffectTechnique* m_pAnisotropicTechnique{ nullptr };

	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };


	//------------------------------------------------
	// Private member functions						
	//------------------------------------------------

	void BindShaderTechniques();

};

