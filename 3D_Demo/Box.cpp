#include "Box.h"
#include "d3dx11Effect.h"
#include <fstream>
#include <vector>
using namespace std;
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

Box::Box(HINSTANCE hInstance)
:	D3DApplication(hInstance),
	mBoxVB(nullptr),
	mBoxIB(nullptr),
	mFX(nullptr),
	mTech(nullptr),
	mfxWorldViewProj(nullptr),
	mInputLayout(nullptr),
	mTheta(1.5f*MathHelper::Pi),
	mPhi(0.25f*MathHelper::Pi),
	mRadius(5.0f)
{
	mMainWndCaption = L"Box Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
}

Box::~Box()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
}

bool Box::Init()
{
	if (!D3DApplication::Init())
		return false;

	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();
	return true;
}

void Box::OnResize()
{
	D3DApplication::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, D3DApplication::AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void Box::UpdateScence(float dt)
{
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void Box::DrawScence()
{
	mD3DImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>( &Colors::Blue ));
	mD3DImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mD3DImmediateContext->IASetInputLayout(mInputLayout);
	mD3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mD3DImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	mD3DImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;
	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>( &worldViewProj ));

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, mD3DImmediateContext);
		mD3DImmediateContext->DrawIndexed(36, 0, 0);
	}

	HR(mSwapChain->Present(0, 0));
}

void Box::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void Box::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Box::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (( btnState & MK_LBUTTON ) != 0)
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>( x - mLastMousePos.x ));
		float dy = XMConvertToRadians(0.25f*static_cast<float>( y - mLastMousePos.y ));

		mTheta += dx;
		mPhi += dy;
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if (( btnState & MK_RBUTTON ) != 0)
	{
		float dx = 0.005f*static_cast<float>( x - mLastMousePos.x );
		float dy = 0.005f*static_cast<float>( y - mLastMousePos.y );

		mRadius += dx - dy;
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void Box::BuildGeometryBuffers()
{
#pragma region Create Vertices Buffer
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), *(XMFLOAT4*)( &Colors::White )},
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), *(XMFLOAT4*)( &Colors::Black )},
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), *(XMFLOAT4*)( &Colors::Red )},
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), *(XMFLOAT4*)( &Colors::Green )},
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), *(XMFLOAT4*)( &Colors::Blue )},
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), *(XMFLOAT4*)( &Colors::Yellow )},
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), *(XMFLOAT4*)( &Colors::Cyan )},
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), *(XMFLOAT4*)( &Colors::Magenta )}
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(mD3DDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));
#pragma endregion

#pragma region Create Indices Buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,
		// back face
		4, 6, 5,
		4, 7, 6,
		// left face
		4, 5, 1,
		4, 1, 0,
		// right face
		3, 2, 6,
		3, 6, 7,
		// top face
		1, 5, 6,
		1, 6, 2,
		// bottom face
		4, 0, 3,
		4, 3, 7
	};
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(mD3DDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
#pragma endregion
}

void Box::BuildFX()
{
	std::ifstream fin("fx/color.fxo", std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
									0, mD3DDevice, &mFX));

	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void Box::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(mD3DDevice->CreateInputLayout(vertexDesc, 2,
									 passDesc.pIAInputSignature,
									 passDesc.IAInputSignatureSize, &mInputLayout));
}
