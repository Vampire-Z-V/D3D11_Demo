#pragma once

#include "D3DUtility.h"

class GeometryGenerator
{
public:
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 Tangent;
		XMFLOAT2 Texture;

		Vertex(){}
		Vertex(
			const XMFLOAT3 &p,
			const XMFLOAT3 &n,
			const XMFLOAT3 &t,
			const XMFLOAT2 &uv
		):	Position(p), Normal(n), 
			Tangent(t), Texture(uv){}
		Vertex(
			float p_x, float p_y, float p_z,
			float n_x, float n_y, float n_z,
			float t_x, float t_y, float t_z,
			float u, float v
		):	Position(p_x, p_y, p_z), Normal(n_x, n_y, n_z), 
			Tangent(t_x, t_y, t_z), Texture(u, v){}
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};


};

