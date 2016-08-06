#include "Shape.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include <fstream>
#include <vector>

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

Shape::Shape(HINSTANCE hInstance)
:	D3DApplication(hInstance),
	mShapeVB(nullptr),
	mShapeIB(nullptr),
	mFX(nullptr),
	mTech(nullptr),
	mfxWorldViewProj(nullptr),
	mInputLayout(nullptr),
	mWireframeRS(nullptr),
	mTheta(1.5f*MathHelper::Pi),
	mPhi(0.1f*MathHelper::Pi),
	mRadius(15.0f)
{
	mMainWndCaption = L"Shape Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	XMMATRIX boxScale = XMMatrixScaling(2.0f, 1.0f, 2.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX centerSphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
	XMMATRIX centerSphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	XMStoreFloat4x4(&mCenterSphereWorld, XMMatrixMultiply(centerSphereScale, centerSphereOffset));

	for ( int i = 0; i < 5; i++ )
	{
		XMStoreFloat4x4(&mCylinderWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mCylinderWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f));

		XMStoreFloat4x4(&mSphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mSphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f));
	}
}

Shape::~Shape()
{
	ReleaseCOM(mShapeVB);
	ReleaseCOM(mShapeIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mWireframeRS);
}

bool Shape::Init()
{
	if ( !D3DApplication::Init() )
		return false;

	BuildGeometryBuffers();
	LoadFX();
	BuildVertexLayout();

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;
	HR(mD3DDevice->CreateRasterizerState(&wireframeDesc, &mWireframeRS));

	return true;
}

void Shape::OnResize()
{
	D3DApplication::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, D3DApplication::AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void Shape::UpdateScence(float dt)
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

void Shape::DrawScence()
{
	mD3DImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>( &Colors::Silver));
	mD3DImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mD3DImmediateContext->IASetInputLayout(mInputLayout);
	mD3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mD3DImmediateContext->RSSetState(mWireframeRS);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mD3DImmediateContext->IASetVertexBuffers(0, 1, &mShapeVB, &stride, &offset);
	mD3DImmediateContext->IASetIndexBuffer(mShapeIB, DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view*proj;

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for ( UINT p = 0; p < techDesc.Passes; ++p )
	{
		// Draw the grid.
		XMMATRIX world = XMLoadFloat4x4(&mGridWorld);
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
		mTech->GetPassByIndex(p)->Apply(0, mD3DImmediateContext);
		mD3DImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&mBoxWorld);
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
		mTech->GetPassByIndex(p)->Apply(0, mD3DImmediateContext);
		mD3DImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

		// Draw center sphere.
		world = XMLoadFloat4x4(&mCenterSphereWorld);
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
		mTech->GetPassByIndex(p)->Apply(0, mD3DImmediateContext);
		mD3DImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);

		// Draw the cylinders.
		for ( int i = 0; i < 10; ++i )
		{
			world = XMLoadFloat4x4(&mCylinderWorld[i]);
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
			mTech->GetPassByIndex(p)->Apply(0, mD3DImmediateContext);
			mD3DImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}

		// Draw the spheres.
		for ( int i = 0; i < 10; ++i )
		{
			world = XMLoadFloat4x4(&mSphereWorld[i]);
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
			mTech->GetPassByIndex(p)->Apply(0, mD3DImmediateContext);
			mD3DImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
		}
	}

	HR(mSwapChain->Present(0, 0));
}

void Shape::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void Shape::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Shape::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ( ( btnState & MK_LBUTTON ) != 0 )
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>( x - mLastMousePos.x ));
		float dy = XMConvertToRadians(0.25f*static_cast<float>( y - mLastMousePos.y ));

		mTheta += dx;
		mPhi += dy;
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ( ( btnState & MK_RBUTTON ) != 0 )
	{
		float dx = 0.01f*static_cast<float>( x - mLastMousePos.x );
		float dy = 0.01f*static_cast<float>( y - mLastMousePos.y );

		mRadius += dx - dy;
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 200.0f);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void Shape::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGeosphere(0.5f, 3, sphere);
	//geoGen.CreateSphere(1.0f, 30, 30, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 30, 30, cylinder);

	mGridVertexOffset		= 0;
	mBoxVertexOffset		= grid.Vertices.size();
	mSphereVertexOffset		= mBoxVertexOffset + box.Vertices.size();
	mCylinderVertexOffset	= mSphereVertexOffset + sphere.Vertices.size();

	mGridIndexCount			= grid.Indices.size();
	mBoxIndexCount			= box.Indices.size();
	mSphereIndexCount		= sphere.Indices.size();
	mCylinderIndexCount		= cylinder.Indices.size();

	mGridIndexOffset		= 0;
	mBoxIndexOffset			= mGridIndexCount;
	mSphereIndexOffset		= mBoxIndexOffset + mBoxIndexCount;
	mCylinderIndexOffset	= mSphereIndexOffset + mSphereIndexCount;

	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount =
		mBoxIndexCount +
		mGridIndexCount +
		mSphereIndexCount +
		mCylinderIndexCount;
#pragma region Create Vertices Buffer
	std::vector<Vertex> vertices(totalVertexCount);
	XMFLOAT4 color = *(XMFLOAT4*)&Colors::Red;

	UINT k = 0;
	for ( size_t i = 0; i < grid.Vertices.size(); ++i, ++k )
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = *(XMFLOAT4*)&Colors::Blue;
	}

	for ( size_t i = 0; i < box.Vertices.size(); ++i, ++k )
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = *(XMFLOAT4*)&Colors::Magenta;
	}

	for ( size_t i = 0; i < sphere.Vertices.size(); ++i, ++k )
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Color = *(XMFLOAT4*)&Colors::Yellow;
	}

	for ( size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k )
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = *(XMFLOAT4*)&Colors::Red;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex)*totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(mD3DDevice->CreateBuffer(&vbd, &vinitData, &mShapeVB));
#pragma endregion

#pragma region Create Indices Buffer
	std::vector<UINT> indices;
	indices.clear();
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)*totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(mD3DDevice->CreateBuffer(&ibd, &iinitData, &mShapeIB));
#pragma endregion
}

void Shape::LoadFX()
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

void Shape::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(mD3DDevice->CreateInputLayout(vertexDesc, 2,
									 passDesc.pIAInputSignature,
									 passDesc.IAInputSignatureSize, &mInputLayout));
}
