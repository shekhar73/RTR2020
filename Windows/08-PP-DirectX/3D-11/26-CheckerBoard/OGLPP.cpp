#include <windows.h>
#include <stdio.h>
//#include <math.h>


#include <d3d11.h>
#include <d3dcompiler.h>

#pragma warning(disable: 4838)
#include "XNAMath_204\xnamath.h"
#include "WICTextureLoader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")	// Directx Graphics Infrastructure it is analogus with wgi

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define CHECKIMAGEWIDTH 64
#define CHECKIMAGEHEIGHT 64


// Prototype of WndProc() delclared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declarations

DWORD dwStyle;
HWND ghwnd					= 	NULL;
HDC ghdc					=	NULL;
HGLRC ghrc					= 	NULL;
FILE* gpVendorInfoFile		= 	NULL;
FILE *gpFile				=	NULL;
char gszLogFileName[]		= 	"Log.txt";
WINDOWPLACEMENT wpPrev		= { sizeof(WINDOWPLACEMENT) };
bool gbActiveWindow			= false;
bool gbEscapeKeyIsPressed	= false;
bool gbFullScreen			= false;

float gClearColor[4];	// RGBA
IDXGISwapChain *gpIDXGISwapChain;
ID3D11Device *gpID3D11Device 						= NULL;
ID3D11DeviceContext *gpID3D11DeviceContext 			= NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView 	= NULL;

ID3D11VertexShader *gpID3D11VertexShader 			= NULL;
ID3D11PixelShader *gpID3D11PixelShader 				= NULL;
ID3D11InputLayout *gpID3D11InputLayout 				= NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer 		= NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Position_Q 	= NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Texture_Q 	= NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Position_R 	= NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Texture_R 	= NULL;

ID3D11DepthStencilView *gpID3D11DepthStencilView 				= NULL;
ID3D11RasterizerState *gpID3D11RasterizerState 					= NULL;
ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture 	= NULL;
ID3D11SamplerState *gpID3D11SamplerState_Texture 				= NULL;
ID3D11Texture2D *gpID3D11Texture2D 								= NULL;

UCHAR checkImage[CHECKIMAGEWIDTH][CHECKIMAGEHEIGHT][4];
UCHAR checkImage_1D[CHECKIMAGEWIDTH * CHECKIMAGEHEIGHT * 4];

XMMATRIX gPerspectiveProjectionMatrix;


// Entry Point function i.e. main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Prototype
	HRESULT Initialize(void);
	void UnInitialize(void);
	void Display(void);

	// Variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("Direct3D11");
	bool bDone = false;

	// code
	
	if (fopen_s(&gpFile, "LogFile.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExitting..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened.\n");
		fclose(gpFile);
	}

	wndclass.cbSize			= sizeof(WNDCLASSEX);
	wndclass.style 			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance 		= hInstance;
	wndclass.hbrBackground 	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon 			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor 		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm 		= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc 	= WndProc;
	wndclass.lpszClassName	= szClassName;
	wndclass.lpszMenuName 	= NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("CHECKDER BOARD"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		((GetSystemMetrics(SM_CXSCREEN) / 2) - (WIN_WIDTH / 2)),
		((GetSystemMetrics(SM_CYSCREEN) / 2) - (WIN_HEIGHT / 2)),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// initialize D3D
	HRESULT hr;
	hr = Initialize();
	if(FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "Initialize() FAILED. Exitting now...\n");
		fclose(gpFile);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "Initialize() Succeded.\n");
		fclose(gpFile);
	}

	while(bDone == false)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			Display();

			if(gbActiveWindow == true)
			{
				if(gbEscapeKeyIsPressed == true)
				{
					bDone = true;
				}
			}
		}
	}
	UnInitialize();

	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration
	void ToggleFullScreen(void);
	HRESULT Resize(int, int);
	void UnInitialize();

	// local variable
	HRESULT hr;

	// code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if(HIWORD(wParam) == 0)
		{
			gbActiveWindow = true;
		}
		else
		{
			gbActiveWindow = false;
		}
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_SIZE:
		if(gpID3D11DeviceContext)
		{
			hr = Resize(LOWORD(lParam), HIWORD(lParam));
			if(FAILED(hr))
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "Initialize() FAILED. Exitting now...\n");
				fclose(gpFile);
				DestroyWindow(hwnd);
				hwnd = NULL;
			}
			else
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "Initialize() Succeded.\n");
				fclose(gpFile);
			}
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
			case VK_ESCAPE:
				if (gbEscapeKeyIsPressed == false)
				{
					gbEscapeKeyIsPressed = true;
				}
				break;

			case 0x46:
			case 0x66:
				if (gbFullScreen == false)
				{
					ToggleFullScreen();
					gbFullScreen = true;
				}
				else
				{
					ToggleFullScreen();
					gbFullScreen = false;
				}
				break;

			default:
				break;
		}
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_CLOSE:
		UnInitialize();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen()
{
	// local variable
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code
	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && (GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP,
					mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}

		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOZORDER);
		ShowCursor(TRUE);
	}
}

HRESULT Initialize(void)
{
	// Function prototypes
	HRESULT Resize(int, int);
	void UnInitialize();
	HRESULT LoadD3DTexture(ID3D11ShaderResourceView **);
	//void printD3DInfo();
	
	// variable declarations
	HRESULT hr;
	//printD3DInfo();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void *)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount 							= 1;
	dxgiSwapChainDesc.BufferDesc.Width 						= WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height 					= WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format 					= DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator 		= 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator 	= 1;
	dxgiSwapChainDesc.BufferUsage 							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow 							= ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count 						= 1;
	dxgiSwapChainDesc.SampleDesc.Quality 					= 0;
	dxgiSwapChainDesc.Windowed 								= TRUE;

	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] = { 	D3D_DRIVER_TYPE_HARDWARE,
										 	D3D_DRIVER_TYPE_WARP,
										 	D3D_DRIVER_TYPE_REFERENCE,
										};
	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;	// default. lowest
	UINT createDeviceFlags = 0;
	UINT numDriverTypes = 0;
	UINT numFeatureLevels = 1;	// based upon d3dFeatureLevel_required

	//code
	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);	// calculating size of array

	for(UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; numDriverTypes++)
	{
		d3dDriverType = d3dDriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL, 							// Adapter
			d3dDriverType,					// Driver Type
			NULL, 							// Software
			createDeviceFlags,				// Flags
			&d3dFeatureLevel_required,		// Feature Levels
			numFeatureLevels,				// Num Feature Levels
			D3D11_SDK_VERSION,				// SDK Version
			&dxgiSwapChainDesc,				// Swap Chain
			&gpIDXGISwapChain,				// Swap Chain
			&gpID3D11Device,					// Device
			&d3dFeatureLevel_acquired,		// Feature Level
			&gpID3D11DeviceContext);		// Device Context

		if(SUCCEEDED(hr))
			break;
	}

	if(FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3D11CreateDeviceAndSwapChain() FAILED. Exitting now...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3D11CreateDeviceAndSwapChain() Succeded.\n");
		fprintf_s(gpFile, "The Chosen Driver Is Of ");
		if(d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpFile, "Hardware Type.\n");
		}
		else if(d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpFile, "Warp Type.\n");
		}
		else if(d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpFile, "Reference Type.\n");
		}
		else
		{
			fprintf_s(gpFile, "Unkown Type.\n");
		}

		fprintf_s(gpFile, "The Supported Highest Feature Level Is ");
		if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf_s(gpFile, "11.0\n");
		}
		else if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf_s(gpFile, "10.0f\n");
		}
		else
		{
			fprintf_s(gpFile, "Unkown.\n");
		}
		fclose(gpFile);
	}

	//-----------------------------------------------------------------------------------------------------
	// initialize shader, input layouts, constant buffers etc.
		const char *vertexShaderSourceCode =
	{
		"cbuffer ConstantBuffer {																			\n" \
		"	float4x4 worldViewProjectionMatrix;																\n" \
		"};																									\n" \
		"																									\n" \
		"struct vertex_output {																				\n" \
		"	float4 position: SV_POSITION;																	\n" \
		"	float2 texcoord: TEXCOORD;																		\n" \
		"};																									\n" \
		"																									\n" \
		"vertex_output main(float4 pos:POSITION, float2 texcoord:TEXCOORD) {								\n" \
		"	vertex_output output;																			\n" \
		"																									\n" \
		"	output.position = mul(worldViewProjectionMatrix, pos);											\n" \
		"	output.texcoord = texcoord;																		\n" \
		"																									\n" \
		"	return (output);																				\n" \
		"}																									\n"
	};

	ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode,				// Shader source code
		lstrlenA(vertexShaderSourceCode) + 1,			// Source code string length
		"VS",											// Shader's type source code string
		NULL,											// struct D3D_SHADER_MACRO *, if there are any macros used in the shader
		D3D_COMPILE_STANDARD_FILE_INCLUDE,				// If the shader any '#inlcude', specify it here, we tell DX to include standard files.
		"main",											// Name of entry point function
		"vs_5_0",										// Shader version
		0,												// How the shader compiler should compile our code, 0, default- Debug
		0,												// Effect constant, if any
		&pID3DBlob_VertexShaderCode,					// Compiled code
		&pID3DBlob_Error);								// Errors

		if(FAILED(hr))
		{
			if(pID3DBlob_Error != NULL)
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "D3DCompile() FAILED For Vertex Shader. %s.\n", (char *)pID3DBlob_Error->GetBufferPointer());
				fclose(gpFile);
				pID3DBlob_Error->Release();
				pID3DBlob_Error = NULL;
				return(hr);	
			}
			else
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "Failed due Some COM Error.\n");
				fclose(gpFile);
			}
		}
		else
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Succeded For Vertex Shader.\n");
			fclose(gpFile);
		}

		hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader);

		if(FAILED(hr))
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateVertexShader() Failed.\n");
			fclose(gpFile);
			return(hr);			
		}
		else
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreatePixelShader() Succeded.\n");
			fclose(gpFile);
		}

		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);


	//----------------------------------------------------------------------------------------------------------

	const char *pixelShaderSourceCode =
	{
		"Texture2D myTexture2D;																			\n" \
		"SamplerState mySamplerState;																	\n" \
		"																								\n" \
		"float4 main(float4 pos: SV_POSITION, float2 texcoord: TEXCOORD): SV_TARGET {					\n" \
		"	float4 color = myTexture2D.Sample(mySamplerState, texcoord);								\n" \
		"	return (color);																				\n" \
		"}																								\n"
	};

		ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
		pID3DBlob_Error = NULL;

		hr = D3DCompile(pixelShaderSourceCode,
			lstrlenA(pixelShaderSourceCode) + 1,
			"PS",
			NULL,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"ps_5_0",
			0,
			0,
			&pID3DBlob_PixelShaderCode,
			&pID3DBlob_Error);

		if(FAILED(hr))
		{
			if(pID3DBlob_Error != NULL)
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "D3DCompile() FAILED For Pixel Shader. %s.\n", (char *)pID3DBlob_Error->GetBufferPointer());
				fclose(gpFile);
				pID3DBlob_Error->Release();
				pID3DBlob_Error = NULL;
				return(hr);	
			}
			else
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "Failed due Some COM Error.\n");
				fclose(gpFile);
			}		
		}
		else
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Succeded For Pixel Shader.\n");
			fclose(gpFile);
		}


		// create Pixel Shader
		hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader);

		if(FAILED(hr))
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreatePixelShader() Failed.\n");
			fclose(gpFile);
			return(hr);			
		}
		else
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreatePixelShader() Succeded.\n");
			fclose(gpFile);
		}

		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, 0, 0);

		// Release PixelShaderCode
		pID3DBlob_PixelShaderCode->Release();
		pID3DBlob_PixelShaderCode = NULL;


		// initialize create and set input layout structure
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
	
		ZeroMemory(inputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC) * _ARRAYSIZE(inputElementDesc));

		inputElementDesc.SemanticName			= "POSITION";
		inputElementDesc.SemanticIndex			= 0;		// if we are going to send multiple geometry in the same semanticName then by seperated by indices and this is the first geometry hence it is 0
		inputElementDesc.Format					= DXGI_FORMAT_R32G32B32_FLOAT; // vertexAttribPointer cha 2nd paratmeter in OpenGL
		inputElementDesc.InputSlot				= 0; // Like enum of POSITON, COLOR, NORMAL, TEXTURE0 in OpenGL
		inputElementDesc.AlignedByteOffset		= 0;
		inputElementDesc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;	// draw vertex by using vertex data
		inputElementDesc.InstanceDataStepRate	= 0;	// gap between 2 instnce data

		inputElementDesc[1].SemanticName 		= "TEXCOORD";
		inputElementDesc[1].SemanticIndex 		= 0;
		inputElementDesc[1].Format 				= DXGI_FORMAT_R32G32_FLOAT;
		inputElementDesc[1].InputSlot 			= 1;
		inputElementDesc[1].AlignedByteOffset 	= D3D11_APPEND_ALIGNED_ELEMENT;
		inputElementDesc[1].InputSlotClass 		= D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc[1].InstanceDataStepRate = 0;

		hr = gpID3D11Device->CreateInputLayout(
			inputElementDesc, 
			_ARRAYSIZE(inputElementDesc), 
			pID3DBlob_VertexShaderCode->GetBufferPointer(),
			pID3DBlob_VertexShaderCode->GetBufferSize(), 
			&gpID3D11InputLayout);

		if(FAILED(hr))
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateInputLayout() Failed.\n");
			fclose(gpFile);

			pID3DBlob_VertexShaderCode->Release();
			pID3DBlob_VertexShaderCode = NULL;
			return(hr);			
		}
		else
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateInputLayout() Succeded.\n");
			fclose(gpFile);
			gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);
			pID3DBlob_VertexShaderCode->Release();
			pID3DBlob_VertexShaderCode = NULL;
		}

		gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

		pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;

	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;


	D3D11_BUFFER_DESC bufferDesc_VertexBuffer_Position_Q;
	ZeroMemory(&bufferDesc_VertexBuffer_Position_Q, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_VertexBuffer_Position_Q.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer_Position_Q.ByteWidth = sizeof(float) * 18;
	bufferDesc_VertexBuffer_Position_Q.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer_Position_Q.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer_Position_Q, NULL, &gpID3D11Buffer_VertexBuffer_Position_Q);


	D3D11_BUFFER_DESC bufferDesc_VertexBuffer_Texture_Q;
	ZeroMemory(&bufferDesc_VertexBuffer_Texture_Q, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_VertexBuffer_Texture_Q.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer_Texture_Q.ByteWidth = sizeof(float) * 12;
	bufferDesc_VertexBuffer_Texture_Q.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer_Texture_Q.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer_Texture_Q, NULL, &gpID3D11Buffer_VertexBuffer_Texture_Q);
	//-----------------------------------------------------------------------------------------

		float texcoords[] =
		{
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,

			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f
		};


		
		D3D11_MAPPED_SUBRESOURCE mappedSubresource_texture_Q;
		ZeroMemory(&mappedSubresource_texture_Q, sizeof(D3D11_MAPPED_SUBRESOURCE));
		gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Texture_Q, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource_texture_Q);
		memcpy(mappedSubresource_texture_Q.pData, texcoords, sizeof(texcoords));
		gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Texture_Q, NULL);


		// create vertex buffer

		D3D11_BUFFER_DESC bufferDesc_VertexBuffer_Position_R;
		ZeroMemory(&bufferDesc_VertexBuffer_Position_R, sizeof(D3D11_BUFFER_DESC));

		bufferDesc_VertexBuffer_Position_R.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc_VertexBuffer_Position_R.ByteWidth = sizeof(float) * 18;
		bufferDesc_VertexBuffer_Position_R.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc_VertexBuffer_Position_R.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer_Position_R, NULL, &gpID3D11Buffer_VertexBuffer_Position_R);


		if(FAILED(hr))
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Failed for Vertex Buffer.\n");
			fclose(gpFile);
			return(hr);			
		}
		else
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Succeded for Vertex Buffer.\n");
			fclose(gpFile);
		}

		// Create Color Buffer
		D3D11_BUFFER_DESC bufferDesc_VertexBuffer_Texture_R;
		ZeroMemory(&bufferDesc_VertexBuffer_Texture_R, sizeof(D3D11_BUFFER_DESC));

		bufferDesc_VertexBuffer_Texture_R.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc_VertexBuffer_Texture_R.ByteWidth = sizeof(float) * 12;
		bufferDesc_VertexBuffer_Texture_R.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc_VertexBuffer_Texture_R.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer_Texture_R, NULL, &gpID3D11Buffer_VertexBuffer_Texture_R);
		if(FAILED(hr))
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Failed for Color Buffer.\n");
			fclose(gpFile);
			return(hr);			
		}
		else
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Succeded for Color Buffer.\n");
			fclose(gpFile);
		}

		// Copy color into the above buffer
		D3D11_MAPPED_SUBRESOURCE mappedSubresource_texture_R;
		ZeroMemory(&mappedSubresource_texture_R, sizeof(D3D11_MAPPED_SUBRESOURCE));
		gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Texture_R, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource_texture_R);
		memcpy(mappedSubresource_texture_R.pData, texcoords, sizeof(texcoords));
		gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Texture_R, NULL);

		D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
		ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));

		bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
		bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, 0, &gpID3D11Buffer_ConstantBuffer);
		if(FAILED(hr))
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Failed for Constant Buffer.\n");
			fclose(gpFile);
			return(hr);			
		}
		else
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Succeded for Constant Buffer.\n");
			fclose(gpFile);
		}

		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory((void*)&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerDesc.AntialiasedLineEnable = FALSE;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		hr = gpID3D11Device->CreateRasterizerState(&rasterizerDesc, &gpID3D11RasterizerState);
		if(FAILED(hr))
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateRasterizerState Failed.\n");
			fclose(gpFile);
			return(hr);			
		}
		else
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:: CreateRasterizerState Succeded.\n");
			fclose(gpFile);
		}






	// Create the sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

	hr = gpID3D11Device->CreateSamplerState(&samplerDesc, &gpID3D11SamplerState_Texture);

	// d3d Clear Color (blue)
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	// Set projection matrix to identity
	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	// resize
	hr = Resize(WIN_WIDTH, WIN_HEIGHT);
	if(FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "resize() FAILED. Exitting now...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "resize() Succeded.\n");
		fclose(gpFile);
	}

	return (S_OK);
}


void makeCheckImage(void) {
	// Variable Declaration
	int i = 0;
	int j = 0;
	int c = 0;
	for (i = 0; i < CHECKIMAGEHEIGHT; i++) {
		for (j = 0; j < CHECKIMAGEWIDTH; j++) {
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

			checkImage[i][j][0] = (UCHAR)c;
			checkImage[i][j][1] = (UCHAR)c;
			checkImage[i][j][2] = (UCHAR)c;
			checkImage[i][j][3] = 255;
		}
	}
}


void TwoDToOneDArray(void) {
	
	for (uint8_t i = 0; i < CHECKIMAGEWIDTH; i++) {
		for (uint8_t j = 0; j < CHECKIMAGEHEIGHT; j++) {
				checkImage_1D[(i * CHECKIMAGEWIDTH * 4) + (j * 4) + 0] = checkImage[i][j][0];
				checkImage_1D[(i * CHECKIMAGEWIDTH * 4) + (j * 4) + 1] = checkImage[i][j][1];
				checkImage_1D[(i * CHECKIMAGEWIDTH * 4) + (j * 4) + 2] = checkImage[i][j][2];
				checkImage_1D[(i * CHECKIMAGEWIDTH * 4) + (j * 4) + 3] = checkImage[i][j][3];
		}
	}
}


HRESULT LoadD3DTexture(ID3D11ShaderResourceView **ppID3D11ShaderResourceView) {
	// function declarations
	void makeCheckImage(void);
	void TwoDToOneDArray(void);

	// variable declarations
	HRESULT hr = S_OK;

	makeCheckImage();

	D3D11_TEXTURE2D_DESC texture_desc;
	ZeroMemory(&texture_desc, sizeof(D3D11_TEXTURE2D_DESC));
	texture_desc.Width = CHECKIMAGEWIDTH;
	texture_desc.Height = CHECKIMAGEHEIGHT;
	texture_desc.MipLevels = texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DYNAMIC;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture_desc.MiscFlags = 0;

	hr = gpID3D11Device->CreateTexture2D(&texture_desc, NULL, &gpID3D11Texture2D);
	if (FAILED(hr)) {
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "0x%x\n", hr);
		fclose(gpFile);
		return (hr);
	}
	

	// Texture Data
	D3D11_MAPPED_SUBRESOURCE mappedSubresource_texel_data;
	ZeroMemory(&mappedSubresource_texel_data, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Texture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource_texel_data);
	memcpy(mappedSubresource_texel_data.pData, checkImage, CHECKIMAGEWIDTH * CHECKIMAGEHEIGHT * 4 * sizeof(UCHAR));
	gpID3D11DeviceContext->Unmap(gpID3D11Texture2D, NULL);

	// Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
	ZeroMemory(&shader_resource_view_desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shader_resource_view_desc.Format = texture_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

	hr = gpID3D11Device->CreateShaderResourceView(gpID3D11Texture2D, &shader_resource_view_desc, ppID3D11ShaderResourceView);
	if (FAILED(hr)) {
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "0x%x\n", hr);
		fclose(gpFile);
		return (hr);
	}
	
	if (ppID3D11ShaderResourceView == NULL) {
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "0x%x\n", hr);
		fclose(gpFile);
		return (hr);
	}

	return (hr);
}


HRESULT Resize(int width, int height)
{
	// code
	HRESULT hr = S_OK;

	if (gpID3D11DepthStencilView) {
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	if (gpID3D11RenderTargetView) {
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pID3D11Texture2D_BackBuffer);

	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL,
		&gpID3D11RenderTargetView);
	if(FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateRenderTargetView() FAILED. Exitting now...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateRenderTargetView() Succeded.\n");
		fclose(gpFile);
	}

	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	// Create depth stencil buffer
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;	// In real world, this can be up to 4
	textureDesc.SampleDesc.Quality = 0;	// If above is 4, then it is 1
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D *pID3D11Texture2D_DepthBuffer;
	gpID3D11Device->CreateTexture2D(&textureDesc, NULL, &pID3D11Texture2D_DepthBuffer);

	// Create depth stencil view from above depth stencil buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer, &depthStencilViewDesc, &gpID3D11DepthStencilView);
	if(FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateDepthStencilView() FAILED. Exitting now...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateDepthStencilView() Succeded.\n");
		fclose(gpFile);
	}

	pID3D11Texture2D_DepthBuffer->Release();
	pID3D11Texture2D_DepthBuffer = NULL;

	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView);

	// Set viewport
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;

	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);

	// Set Perspective Matrix
	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width / (float)height,
		0.1f, 100.0f);

	return (hr);
}

/*void printD3DInfo()
{
	// variable declarations
	IDXGIFactory *piDXGIFactory = NULL;
	IDXGIAdapter *piDXGIAdapter	= NULL;

	DXGI_ADAPTER_DESC dxgiAdapterDesc;
	HRESULT hr;
	char str[255];

	// code
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&piDXGIFactory);
	if(FAILED(hr))
	{
		
		// createDXGIFactor failed
		printf("createDXGIFactory FAILED\n");
		goto cleanup;

	}

	if(piDXGIFactory->EnumAdapters(0, &piDXGIAdapter) == DXGI_ERROR_NOT_FOUND)
	{
		printf("DXGIAdaptor cannot be found\n");
		goto cleanup;
	}

	ZeroMemory((void*)&dxgiAdapterDesc, sizeof(DXGI_ADAPTER_DESC));
	hr = piDXGIAdapter->GetDesc(&dxgiAdapterDesc);

	WideCharToMultiByte(CP_ACP, 0, dxgiAdapterDesc.Description, 255, str, 255, NULL, NULL);
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Graphic card name = %s\n", str);
	printf("Graphic card VRAM = %I64d bytes\n", (__int64)dxgiAdapterDesc.DedicatedVideoMemory);
	printf("VRAM in GB = %d\n", int(ceil(dxgiAdapterDesc.DedicatedVideoMemory / 1024.0 / 1024.0 / 1024.0)));
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	cleanup:
	if(piDXGIAdapter)
	{
		piDXGIAdapter->Release();
		piDXGIAdapter = NULL;
	}

	if(piDXGIFactory)
	{
		piDXGIFactory->Release();
		piDXGIFactory = NULL;
	}
}*/

void Display(void)
{
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	UINT stride = sizeof(float) * 3;
	UINT offset = 0;

	float vertices_Q[] =
	{
		-2.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		-2.0f, -1.0f, 0.0f,

		-2.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, -1.0f, 0.0f
	};

	D3D11_MAPPED_SUBRESOURCE mappedSubresource_position_Q;
	ZeroMemory(&mappedSubresource_position_Q, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Position_Q, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource_position_Q);
	memcpy(mappedSubresource_position_Q.pData, vertices_Q, sizeof(vertices_Q));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Position_Q, NULL);
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Position_Q, &stride, &offset);

	stride = sizeof(float) * 2;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Texture_Q, &stride, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gpID3D11DeviceContext->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture);
	gpID3D11DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture);

	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();

	worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 7.0f);
	worldMatrix = rotationMatrix * worldMatrix;

	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	CBUFFER constantBuffer;
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	gpID3D11DeviceContext->Draw(3, 0);
	gpID3D11DeviceContext->Draw(3, 3);

	stride = sizeof(float) * 3;
	offset = 0;

	float vertices_R[] =
	{
		1.0f, 1.0f, 0.0f,
		2.41421f, 1.0f, 1.41421f,
		1.0f, -1.0f, 0.0f,

		1.0f, -1.0f, 0.0f,
		2.41421f, 1.0f, 1.41421f,
		2.41421f, -1.0f, 1.41421f
	};

	D3D11_MAPPED_SUBRESOURCE mappedSubresource_position_R;
	ZeroMemory(&mappedSubresource_position_R, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Position_R, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource_position_R);
	memcpy(mappedSubresource_position_R.pData, vertices_R, sizeof(vertices_R));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Position_R, NULL);

	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Position_R, &stride, &offset);

	stride = sizeof(float) * 2;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Texture_R, &stride, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gpID3D11DeviceContext->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture);
	gpID3D11DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture);

	worldMatrix = XMMatrixIdentity();
	viewMatrix = XMMatrixIdentity();
	rotationMatrix = XMMatrixIdentity();

	worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	worldMatrix = rotationMatrix * worldMatrix;

	wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	gpID3D11DeviceContext->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture);
	gpID3D11DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture);

	gpID3D11DeviceContext->Draw(3, 0);
	gpID3D11DeviceContext->Draw(3, 3);

	gpIDXGISwapChain->Present(0, 0);
}



void UnInitialize()
{
	// code
	if(gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}


	if(gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}


	if(gpID3D11Buffer_VertexBuffer)
	{
		gpID3D11Buffer_VertexBuffer->Release();
		gpID3D11Buffer_VertexBuffer = NULL;
	}


	if(gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}


	if(gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}

	if(gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if(gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}

	if(gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}

	if(gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}
	
	if(gpFile)
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "UnInitialize() Succeded.\n");
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
