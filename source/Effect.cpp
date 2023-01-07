#include "pch.h"
#include "Effect.h"
#include <assert.h>

Effect::Effect(ID3D11Device* pDeviceInput, const std::wstring& pathInput)
{
	m_pEffect = LoadEffect(pDeviceInput, pathInput);

	BindShaderTechniques();


	// Bind WorldViewProjection Matrix
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"variable gWorldViewProj invalid\n";
	}
	// Bind Diffuse map
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable invalid\n";
	}

}

Effect::~Effect()
{
	m_pActiveTechnique->Release();
	m_pEffect->Release();
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);
	
	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
			{
				ss << pErrors[i];
			}

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;

		}
		else
		{
			std::wstringstream ss;
			ss << "Effectloader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechniquePtr()
{
	return m_pActiveTechnique;
}


void Effect::SetWorldViewProjectionMatrix(const dae::Matrix& worldViewProjectionMatrix)
{
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<const float*>(&worldViewProjectionMatrix));
}

void Effect::SetDiffuseMap(dae::Texture* pDiffuseTexture)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetResourceViewTexturePtr());
}

void Effect::ToggleSampleState()
{
	m_SampleState = static_cast<sampleState>((static_cast<int>(m_SampleState) + 1) % NROFSAMPLESTATES);

	std::cout << "Sample State = ";
	switch (m_SampleState)
	{
	case Effect::sampleState::point:
		m_pActiveTechnique = m_pPointTechnique;
		std::cout << "Point Sampling\n";
		break;
	case Effect::sampleState::linear:
		m_pActiveTechnique = m_pLinearTechnique;
		std::cout << "Linear Sampling\n";
		break;
	case Effect::sampleState::anisotropic:
		m_pActiveTechnique = m_pAnisotropicTechnique;
		std::cout << "Anisotropic Sampling\n";
		break;
	default:
		break;
	}

}

void Effect::BindShaderTechniques()
{
	m_pPointTechnique = m_pEffect->GetTechniqueByName("PointTechnique");
	if (!m_pPointTechnique->IsValid())
	{
		std::wcout << L"PointTechnique invalid\n";
	}

	m_pLinearTechnique = m_pEffect->GetTechniqueByName("LinearTechnique");
	if (!m_pLinearTechnique->IsValid())
	{
		std::wcout << L"Technique invalid\n";
	}

	m_pAnisotropicTechnique = m_pEffect->GetTechniqueByName("AnisotropicTechnique");
	if (!m_pAnisotropicTechnique->IsValid())
	{
		std::wcout << L"Technique invalid\n";
	}


	m_pActiveTechnique = m_pPointTechnique;
}
