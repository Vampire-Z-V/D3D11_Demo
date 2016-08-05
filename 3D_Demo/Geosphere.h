#pragma once
#include "D3DApplication.h"
#include "MathHelper.h"

class Geosphere :
	public D3DApplication
{
public:
	Geosphere(HINSTANCE hInstance);
	~Geosphere();

	bool Init();
	void OnResize();
	void UpdateScence(float dt);
	void DrawScence();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();
	void LoadFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer *mGeosphereVB;
	ID3D11Buffer *mGeosphereIB;

	ID3DX11Effect *mFX;
	ID3DX11EffectTechnique *mTech;
	ID3DX11EffectMatrixVariable *mfxWorldViewProj;

	ID3D11InputLayout *mInputLayout;

	ID3D11RasterizerState *mWireframeRS;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	int mGeosphereIndexCount;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

