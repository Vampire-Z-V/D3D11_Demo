#pragma once
#include "D3DApplication.h"
#include "MathHelper.h"

class Box :
	public D3DApplication
{
public:
	Box(HINSTANCE hInstance);
	~Box();

	bool Init();
	void OnResize();
	void UpdateScence(float dt);
	void DrawScence();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer *mBoxVB;
	ID3D11Buffer *mBoxIB;

	ID3DX11Effect *mFX;
	ID3DX11EffectTechnique *mTech;
	ID3DX11EffectMatrixVariable *mfxWorldViewProj;

	ID3D11InputLayout *mInputLayout;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

