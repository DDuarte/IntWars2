#pragma once
class Renderer
{
public:
	Renderer();
	~Renderer();
	IDirect3DDevice9* GetDevice();
	void SetDevice(IDirect3DDevice9* device);
	D3DSURFACE_DESC GetRenderTargetDescription();
private:
	IDirect3DDevice9* pDevice;
};
extern Renderer MainRenderer;