#include "stdafx.h"
#include "Renderer.h"

Renderer MainRenderer;
Renderer::Renderer() {
}


Renderer::~Renderer() {
}

IDirect3DDevice9 *Renderer::GetDevice() {
    return pDevice;
}

D3DSURFACE_DESC Renderer::GetRenderTargetDescription() {
	D3DSURFACE_DESC desc;
	LPDIRECT3DSURFACE9 pSurf;
	pDevice->GetRenderTarget(0, &pSurf);
	pSurf->GetDesc(&desc);
	pSurf->Release();
	return desc;
}

void Renderer::SetDevice(IDirect3DDevice9* device) {
	this->pDevice = device;
}
