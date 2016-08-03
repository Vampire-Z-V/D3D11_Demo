#ifndef D3DUTILITY_H
#define D3DUTILITY_H

#include "d3dx11Effect.h"
#include "DDSTextureLoader.h"
#include "dxerr.h"
#include "MathHelper.h"
#include <cassert>

// Simple error checker for debugging. 
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)													\
		{                                                           \
			HRESULT hr = (x);                                       \
			if(FAILED(hr))                                          \
			{                                                       \
				DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true); \
			}                                                       \
		}
	#endif // HR
	
	#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 

// Convenience macro for releasing COM objects. 
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

// Convenience macro for deleting objects.
#define SafeDelete(x) { delete x; x = 0; }

namespace Colors
{
	XMGLOBALCONST XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

	XMGLOBALCONST XMVECTORF32 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
	XMGLOBALCONST XMVECTORF32 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}
#endif // D3DUTILITY_H
