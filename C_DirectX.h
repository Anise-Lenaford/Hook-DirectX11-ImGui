#pragma once
#include <d3d11.h>

class C_DirectX
{
public:
	static void Hook();

	inline static bool init{};

	inline static ID3D11Device* iDevice{};
	inline static ID3D11DeviceContext* iContext{};
	inline static ID3D11RenderTargetView* iView{};


};

