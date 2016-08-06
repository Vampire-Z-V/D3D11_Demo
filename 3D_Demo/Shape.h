#pragma once
#include "D3DApplication.h"
#include "MathHelper.h"

class Shape :
	public D3DApplication
{
public:
	Shape(HINSTANCE hInstance);
	~Shape();

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
	ID3D11Buffer *mShapeVB;
	ID3D11Buffer *mShapeIB;

	ID3DX11Effect *mFX;
	ID3DX11EffectTechnique *mTech;
	ID3DX11EffectMatrixVariable *mfxWorldViewProj;

	ID3D11InputLayout *mInputLayout;

	ID3D11RasterizerState *mWireframeRS;

	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylinderWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mCenterSphereWorld;
	XMFLOAT4X4 mGridWorld;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	int mGridVertexOffset;
	int mBoxVertexOffset;
	int mSphereVertexOffset;
	int mCylinderVertexOffset;

	UINT mGridIndexOffset;
	UINT mBoxIndexOffset;
	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;

	UINT mGridIndexCount;
	UINT mBoxIndexCount;
	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

