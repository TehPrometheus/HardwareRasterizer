#pragma once
class Effect final
{
public:

	Effect(ID3D11Device* pDeviceInput, const std::wstring pathInput);
	~Effect();

	// -----------------------------------------------
	// Copy/move constructors and assignment operators
	// -----------------------------------------------
	Effect(const Effect& other)					= default;
	Effect(Effect&& other) noexcept				= default;
	Effect& operator=(const Effect& other)		= default;
	Effect& operator=(Effect&& other) noexcept  = default;

	//------------------------------------------------
	// Member functions						
	//------------------------------------------------
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring assetFile);
	ID3DX11EffectTechnique* GetTechniquePtr();
	ID3D11InputLayout* GetInputLayoutPtr();

private:

	//------------------------------------------------
	// Member variables						
	//------------------------------------------------
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};
	ID3D11InputLayout* m_pInputLayout{};


};

