#include <windows.h>
#include <stdio.h>
//#include <math.h>

#include "Sphere.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma warning(disable: 4838)
#include "XNAMath_204\xnamath.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")	// Directx Graphics Infrastructure it is analogus with wgi
#pragma comment (lib, "DirectXTK.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

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
ID3D11Buffer *gpID3D11Buffer_VertexBuffer 			= NULL;
ID3D11InputLayout *gpID3D11InputLayout 				= NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer 		= NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Sphere_Position = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Sphere_Normal = NULL;

ID3D11Buffer *gpID3D11Buffer_IndexBuffer = NULL;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

unsigned int gNumElements;
unsigned int gNumVertices;

float winWidth = WIN_WIDTH;
float winHeight = WIN_HEIGHT;

unsigned int KeyPressed = 0;

float gAngle = 0.0f;
bool bLight = false;

ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

// Rasterizer State
ID3D11RasterizerState *gpID3D11RasterizerState = NULL;

// Uniform
struct CBUFFER {
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;
	XMVECTOR La;
	XMVECTOR Ld;
	XMVECTOR Ls;
	XMVECTOR Ka;
	XMVECTOR Kd;
	XMVECTOR Ks;
	XMVECTOR LightPosition;
	float MaterialShininess;
	unsigned int KeyPressed;
};

// Color
float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightPosition[] = { 0.0f, 0.0f, -100.0f, 1.0f };

float MaterialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float MaterialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float MaterialShininess = 50.0f;

float matAmbient[24][3];
float matDiffuse[24][3];
float matSpecular[24][3];
float matShininess[24];

XMMATRIX gPerspectiveProjectionMatrix;


// Entry Point function i.e. main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Prototype
	HRESULT Initialize(void);
	void UnInitialize(void);
	void Display(void);
	void update(void);

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
		TEXT("Direct3D11- Sphere"),
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
				update();
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
			case 'X':
			case 'x':
				KeyPressed = 1;
				bLight = true;
				break;

			case 'Y':
			case 'y':
				KeyPressed = 2;
				bLight = true;
				break;

			case 'Z':
			case 'z':
				KeyPressed = 3;
				bLight = true;
				break;	
				
			case 'l':
			case 'L':
				bLight = !bLight;
				break;	


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
	void setMaterialProperties(void);
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
			"	float4x4 worldMatrix;																			\n" \
			"	float4x4 viewMatrix;																			\n" \
			"	float4x4 projectionMatrix;																		\n" \
			"																									\n" \
			"	float4 la;																						\n" \
			"	float4 ld;																						\n" \
			"	float4 ls;																						\n" \
			"	float4 ka;																						\n" \
			"	float4 kd;																						\n" \
			"	float4 ks;																						\n" \
			"																									\n" \
			"	float4 lightPosition;																			\n" \
			"	float materialShininess;																		\n" \
			"	uint keyPressed;																				\n" \
			"};																									\n" \
			"																									\n" \
			"struct vertex_output {																				\n" \
			"	float4 position: SV_POSITION;																	\n" \
			"	float3 transformedNorm: NORMAL0; 																\n" \
			"	float3 lightDirection: NORMAL1;																	\n" \
			"	float3 viewerVector: NORMAL2;																	\n" \
			"};																									\n" \
			"																									\n" \
			"vertex_output main(float4 pos:POSITION, float4 normal:NORMAL) {									\n" \
			"	vertex_output output;																			\n" \
			"																									\n" \
			"	float4 eyeCoordinates = mul(worldMatrix, pos);													\n" \
			"	eyeCoordinates = mul(viewMatrix, eyeCoordinates);												\n" \
			"																									\n" \
			"	output.viewerVector = normalize(-eyeCoordinates.xyz);											\n" \
			"	output.transformedNorm = normalize(mul((float3x3)worldMatrix, (float3)normal));					\n" \
			"	output.lightDirection = (float3)normalize((float3)lightPosition - eyeCoordinates);				\n" \
			"																									\n" \
			"	float4 position = mul(worldMatrix, pos);														\n" \
			"	position = mul(viewMatrix, position);															\n" \
			"	position = mul(projectionMatrix, position);														\n" \
			"																									\n" \
			"	output.position = position;																		\n" \
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

		if (FAILED(hr))
		{
			if (pID3DBlob_Error != NULL) 
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "D3DCompile() Failed For Vertex Shader : %s.\n",(char*)pID3DBlob_Error->GetBufferPointer());
				fclose(gpFile);
				pID3DBlob_Error->Release();
				pID3DBlob_Error = NULL;
				return(hr);
			}
			else
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "Some COM error.\n");
				fclose(gpFile);
			}
		}
		else 
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Succeeded For Vertex Shader.\n");
			fclose(gpFile);
		}
		

		// Generally Create<> functions are called on Device
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
			"cbuffer ConstantBuffer {																			\n" \
			"	float4x4 worldMatrix;																			\n" \
			"	float4x4 viewMatrix;																			\n" \
			"	float4x4 projectionMatrix;																		\n" \
			"																									\n" \
			"	float4 la;																						\n" \
			"	float4 ld;																						\n" \
			"	float4 ls;																						\n" \
			"	float4 ka;																						\n" \
			"	float4 kd;																						\n" \
			"	float4 ks;																						\n" \
			"																									\n" \
			"	float4 lightPosition;																			\n" \
			"	float materialShininess;																		\n" \
			"	uint keyPressed;																				\n" \
			"};																									\n" \
			"																									\n" \
			"struct vertex_output {																				\n" \
			"	float4 position: SV_POSITION;																	\n" \
			"	float3 transformedNorm: NORMAL0; 																\n" \
			"	float3 lightDirection: NORMAL1;																	\n" \
			"	float3 viewerVector: NORMAL2;																	\n" \
			"};																									\n" \
			"																									\n" \
			"float4 main(float4 pos: SV_POSITION, vertex_output input): SV_TARGET {								\n" \
			"	float4 phong_ads_color;																			\n" \
			"	if (keyPressed == 1) {																			\n" \
			"		float3 tNorm = normalize(input.transformedNorm);											\n" \
			"		float3 tLightDirection = normalize(input.lightDirection);									\n" \
			"		float3 tViewerVector = normalize(input.viewerVector);										\n" \
			"																									\n" \
			"		float tn_dot_light_direction = max(dot(tNorm, tLightDirection), 0.0);						\n" \
			"																									\n" \
			"		float4 ambient = la * ka;																	\n" \
			"		float4 diffuse = ld * kd * tn_dot_light_direction;											\n" \
			"																									\n" \
			"		float3 reflectionVector = reflect(-tLightDirection, tNorm);									\n" \
			"																									\n" \
			"		float4 specular = ls * ks * pow(max(dot(reflectionVector, tViewerVector), 0.0), materialShininess); \n" \
			"																									\n" \
			"		phong_ads_color = ambient + diffuse + specular;												\n" \
			"	}																								\n" \
			"	else {																							\n" \
			"		phong_ads_color = float4(1.0, 1.0, 1.0, 1.0);												\n" \
			"	}																								\n" \
			"	float4 color = phong_ads_color;																	\n" \
			"	return (color);																					\n" \
			"}																									\n"
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

		if (FAILED(hr)) 
		{
			if (pID3DBlob_Error != NULL) 
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "D3DCompile() Failed For Pixel Shader : %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
				fclose(gpFile);
				pID3DBlob_Error->Release();
				pID3DBlob_Error = NULL;
				return(hr);
			}
		}
		else 
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Succeeded For Pixel Shader.\n");
			fclose(gpFile);
		}

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

		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, NULL);

		// **** CREATE AND SET INPUT LAYOUT ******
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
		ZeroMemory(inputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC) * _ARRAYSIZE(inputElementDesc));

		inputElementDesc[0].SemanticName = "POSITION";
		inputElementDesc[0].SemanticIndex = 0;
		inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDesc[0].InputSlot = 0;
		inputElementDesc[0].AlignedByteOffset = 0;
		inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc[0].InstanceDataStepRate = 0;

		inputElementDesc[1].SemanticName = "NORMAL";
		inputElementDesc[1].SemanticIndex = 0;
		inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDesc[1].InputSlot = 1;
		inputElementDesc[1].AlignedByteOffset = 0;
		inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc[1].InstanceDataStepRate = 0;


		hr = gpID3D11Device->CreateInputLayout(inputElementDesc, _ARRAYSIZE(inputElementDesc), pID3DBlob_VertexShaderCode->GetBufferPointer(),
			pID3DBlob_VertexShaderCode->GetBufferSize(), &gpID3D11InputLayout);

		if (FAILED(hr)) {
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:CreateInputLayout() Failed.\n");
			fclose(gpFile);
			if(pID3DBlob_VertexShaderCode){
				pID3DBlob_VertexShaderCode->Release();
				pID3DBlob_VertexShaderCode = NULL;
			}
			return(hr);
		}
		else {
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device:CreateInputLayout() Succeeded.\n");
			fclose(gpFile);
			if (pID3DBlob_VertexShaderCode) {
				pID3DBlob_VertexShaderCode->Release();
				pID3DBlob_VertexShaderCode = NULL;
			}
		}

		gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

		// Release ShaderCode Objects
		pID3DBlob_VertexShaderCode->Release();
		pID3DBlob_VertexShaderCode = NULL;

		pID3DBlob_PixelShaderCode->Release();
		pID3DBlob_PixelShaderCode = NULL;

		// ******* BUFFER DESC **********
		getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
		gNumVertices = getNumberOfSphereVertices();
		gNumElements = getNumberOfSphereElements();

		// Create Vertex Buffer
		D3D11_BUFFER_DESC bufferDesc_VertexBuffer_Sphere_Position;
		ZeroMemory(&bufferDesc_VertexBuffer_Sphere_Position, sizeof(D3D11_BUFFER_DESC));

		bufferDesc_VertexBuffer_Sphere_Position.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc_VertexBuffer_Sphere_Position.ByteWidth = sizeof(float) * _ARRAYSIZE(sphere_vertices);
		bufferDesc_VertexBuffer_Sphere_Position.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc_VertexBuffer_Sphere_Position.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer_Sphere_Position, NULL, &gpID3D11Buffer_VertexBuffer_Sphere_Position);
		
		if (FAILED(hr)) 
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Color Buffer Graph.\n");
			fclose(gpFile);
			return(hr);
		}
		else 
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Color Buffer Graph.\n");
			fclose(gpFile);
		}

		// Copy triangle vertices into above buffer
		D3D11_MAPPED_SUBRESOURCE mappedSubresource_Sphere_Position;
		ZeroMemory(&mappedSubresource_Sphere_Position, sizeof(D3D11_MAPPED_SUBRESOURCE));
		gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Sphere_Position, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource_Sphere_Position);
		memcpy(mappedSubresource_Sphere_Position.pData, sphere_vertices, sizeof(sphere_vertices));
		gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Sphere_Position, NULL);

		
		// ******* NORMAL BUFFER DESC ******
		// Create Normal Buffer
		D3D11_BUFFER_DESC bufferDesc_VertexBuffer_Sphere_Normal;
		ZeroMemory(&bufferDesc_VertexBuffer_Sphere_Normal, sizeof(D3D11_BUFFER_DESC));

		bufferDesc_VertexBuffer_Sphere_Normal.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc_VertexBuffer_Sphere_Normal.ByteWidth = sizeof(float) * _ARRAYSIZE(sphere_normals);
		bufferDesc_VertexBuffer_Sphere_Normal.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc_VertexBuffer_Sphere_Normal.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer_Sphere_Normal, NULL, &gpID3D11Buffer_VertexBuffer_Sphere_Normal);



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


	
		// Copy color into the above buffer
		D3D11_MAPPED_SUBRESOURCE mappedSubresource_Sphere_Normal;
		ZeroMemory(&mappedSubresource_Sphere_Normal, sizeof(D3D11_MAPPED_SUBRESOURCE));
		gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Sphere_Normal, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource_Sphere_Normal);
		memcpy(mappedSubresource_Sphere_Normal.pData, sphere_normals, sizeof(sphere_normals));
		gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Sphere_Normal, NULL);
		
		// ******* INDEX BUFFER ******
		// Create Index Buffer
		D3D11_BUFFER_DESC bufferDesc_IndexBuffer;
		ZeroMemory(&bufferDesc_IndexBuffer, sizeof(D3D11_BUFFER_DESC));

		bufferDesc_IndexBuffer.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc_IndexBuffer.ByteWidth = sizeof(short) * gNumElements;
		bufferDesc_IndexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc_IndexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = gpID3D11Device->CreateBuffer(&bufferDesc_IndexBuffer, NULL, &gpID3D11Buffer_IndexBuffer);




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
	// Copy color into the above buffer
		D3D11_MAPPED_SUBRESOURCE mappedSubresource_index_buffer;
		ZeroMemory(&mappedSubresource_index_buffer, sizeof(D3D11_MAPPED_SUBRESOURCE));
		gpID3D11DeviceContext->Map(gpID3D11Buffer_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource_index_buffer);
		memcpy(mappedSubresource_index_buffer.pData, sphere_elements, sizeof(short) * gNumElements);
		gpID3D11DeviceContext->Unmap(gpID3D11Buffer_IndexBuffer, NULL);

		// ***** Define and set the constant buffers ******
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
		gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

		// Set the rasterization state
		// Set the culling as 'OFF'
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

		gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

	// Set Material Properties
	setMaterialProperties();

	// d3d Clear Color (blue)
	gClearColor[0] = 0.25f;
	gClearColor[1] = 0.25f;
	gClearColor[2] = 0.25f;
	gClearColor[3] = 1.0f;

	// Set projection matrix to identity
	gPerspectiveProjectionMatrix = XMMatrixIdentity();


	// call resize for first time
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
	
	return(S_OK);
}

HRESULT Resize(int width, int height)
{
	// code
	HRESULT hr = S_OK;

	// free any size-dependant resources
	if(gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	// resize swap chain buffers accordingly
	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// again get back buffer from swap chain
	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);

	// again get render target view from d3d11 device using above back buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView);
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

	// set render target view as render target
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, NULL);

	// set viewport
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);

	// set perspective matrix
	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), ((float)width / (float)height), 0.1f, 100.0f);
	
	return(hr);
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
	// function declarations
	void drawSpheres(void);
	
	// code
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Sphere_Position, &stride, &offset);
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Sphere_Normal, &stride, &offset);
	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Select the geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	drawSpheres();
	gpIDXGISwapChain->Present(0, 0);
}

void setUniforms(XMVECTOR materialAmbient, XMVECTOR materialDiffuse, XMVECTOR materialSpecular, float materialShininess) {
	// code
	// World matrix transformation
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX projectionMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();

	worldMatrix = XMMatrixScaling(2.0f * (4.0f / 6.0f), 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.0f, 3.0f);

	worldMatrix = rotationMatrix * worldMatrix;
	XMMATRIX wvMatrix = worldMatrix * viewMatrix;
	projectionMatrix = gPerspectiveProjectionMatrix;

	CBUFFER constantBuffer;
	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = viewMatrix;
	constantBuffer.ProjectionMatrix = projectionMatrix;
	
	if (bLight == true) {
		constantBuffer.KeyPressed = 1;
		
		constantBuffer.MaterialShininess = materialShininess;
		
		constantBuffer.La = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
		constantBuffer.Ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
		constantBuffer.Ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			
		constantBuffer.Ka = XMVectorSet(XMVectorGetX(materialAmbient), XMVectorGetY(materialAmbient), 
							XMVectorGetZ(materialAmbient), XMVectorGetW(materialAmbient));
							
		constantBuffer.Kd = XMVectorSet(XMVectorGetX(materialDiffuse), XMVectorGetY(materialDiffuse), 
							XMVectorGetZ(materialDiffuse), XMVectorGetW(materialDiffuse));
		
		constantBuffer.Ks = XMVectorSet(XMVectorGetX(materialSpecular), XMVectorGetY(materialSpecular), 
							XMVectorGetZ(materialSpecular), XMVectorGetW(materialSpecular));
		
		constantBuffer.LightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);	
	}
	else {
		constantBuffer.KeyPressed = 0;
	}
	
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);
}

void setMaterialProperties(void)
{
	// code
	matAmbient[0][0] = 0.0215f;
	matAmbient[0][1] = 0.1745f;
	matAmbient[0][2] = 0.0215f;
	
	matAmbient[1][0] = 0.135f;
	matAmbient[1][1] = 0.2225f;
	matAmbient[1][2] = 0.1575f;
	
	matAmbient[2][0] = 0.05375f;
	matAmbient[2][1] = 0.05f;
	matAmbient[2][2] = 0.06625f;
	
	matAmbient[3][0] = 0.25f;
	matAmbient[3][1] = 0.20725f;
	matAmbient[3][2] = 0.20725f;
	
	matAmbient[4][0] = 0.1745f;
	matAmbient[4][1] = 0.01175f;
	matAmbient[4][2] = 0.01175f;
	
	matAmbient[5][0] = 0.1f;
	matAmbient[5][1] = 0.18725f;
	matAmbient[5][2] = 0.1745f;
	
	// **** METALS *****
	matAmbient[6][0] = 0.329412f;
	matAmbient[6][1] = 0.223529f;
	matAmbient[6][2] = 0.027451f;
	
	matAmbient[7][0] = 0.2125f;
	matAmbient[7][1] = 0.1275f;
	matAmbient[7][2] = 0.054f;
	
	matAmbient[8][0] = 0.25f;
	matAmbient[8][1] = 0.25f;
	matAmbient[8][2] = 0.25f;
	
	matAmbient[9][0] = 0.19125f;
	matAmbient[9][1] = 0.0735f;
	matAmbient[9][2] = 0.0225f;
	
	matAmbient[10][0] = 0.24725f;
	matAmbient[10][1] = 0.1995f;
	matAmbient[10][2] = 0.0745f;
	
	matAmbient[11][0] = 0.19225f;
	matAmbient[11][1] = 0.19225f;
	matAmbient[11][2] = 0.19225f;
	
	
	matAmbient[12][0] = 0.0f;
	matAmbient[12][1] = 0.0f;
	matAmbient[12][2] = 0.0f;
	
	matAmbient[13][0] = 0.0f;
	matAmbient[13][1] = 0.1f;
	matAmbient[13][2] = 0.06f;
	
	matAmbient[14][0] = 0.0f;
	matAmbient[14][1] = 0.0f;
	matAmbient[14][2] = 0.0f;
	
	matAmbient[15][0] = 0.0f;
	matAmbient[15][1] = 0.0f;
	matAmbient[15][2] = 0.0f;
	
	matAmbient[16][0] = 0.0f;
	matAmbient[16][1] = 0.0f;
	matAmbient[16][2] = 0.0f;
	
	matAmbient[17][0] = 0.0f;
	matAmbient[17][1] = 0.0f;
	matAmbient[17][2] = 0.0f;
	
	// ***** RUBBER *****
	matAmbient[18][0] = 0.02f;
	matAmbient[18][1] = 0.02f;
	matAmbient[18][2] = 0.02f;
	
	matAmbient[19][0] = 0.0f;
	matAmbient[19][1] = 0.05f;
	matAmbient[19][2] = 0.05f;
	
	matAmbient[20][0] = 0.0f;
	matAmbient[20][1] = 0.05f;
	matAmbient[20][2] = 0.0f;
	
	matAmbient[21][0] = 0.05f;
	matAmbient[21][1] = 0.0f;
	matAmbient[21][2] = 0.0f;
	
	matAmbient[22][0] = 0.05f;
	matAmbient[22][1] = 0.05f;
	matAmbient[22][2] = 0.05f;
	
	matAmbient[23][0] = 0.05f;
	matAmbient[23][1] = 0.05f;
	matAmbient[23][2] = 0.0f;
	
	// ******* DIFFUSE ******
	matDiffuse[0][0] = 0.07568f;
	matDiffuse[0][1] = 0.61424f;
	matDiffuse[0][2] = 0.07568f;
	
	matDiffuse[1][0] = 0.54f;
	matDiffuse[1][1] = 0.89f;
	matDiffuse[1][2] = 0.63f;
	
	matDiffuse[2][0] = 0.18275f;
	matDiffuse[2][1] = 0.17f;
	matDiffuse[2][2] = 0.2252f;
	
	matDiffuse[3][0] = 1.0f;
	matDiffuse[3][1] = 0.829f;
	matDiffuse[3][2] = 0.829f;
	
	matDiffuse[4][0] = 0.61424f;
	matDiffuse[4][1] = 0.04136f;
	matDiffuse[4][2] = 0.04136f;
	
	matDiffuse[5][0] = 0.396f;
	matDiffuse[5][1] = 0.74151f;
	matDiffuse[5][2] = 0.69102f;
	
	
	matDiffuse[6][0] = 0.780392f;
	matDiffuse[6][1] = 0.568627f;
	matDiffuse[6][2] = 0.113725f;
	
	matDiffuse[7][0] = 0.714f;
	matDiffuse[7][1] = 0.4284f;
	matDiffuse[7][2] = 0.18144f;
	
	matDiffuse[8][0] = 0.4f;
	matDiffuse[8][1] = 0.4f;
	matDiffuse[8][2] = 0.4f;
	
	matDiffuse[9][0] = 0.256777f;
	matDiffuse[9][1] = 0.137622f;
	matDiffuse[9][2] = 0.086014f;
	
	matDiffuse[10][0] = 0.75164f;
	matDiffuse[10][1] = 0.60648f;
	matDiffuse[10][2] = 0.22648f;
	
	matDiffuse[11][0] = 0.50754f;
	matDiffuse[11][1] = 0.50754f;
	matDiffuse[11][2] = 0.50754f;
	
	
	matDiffuse[12][0] = 0.01f;
	matDiffuse[12][1] = 0.01f;
	matDiffuse[12][2] = 0.01f;
	
	matDiffuse[13][0] = 0.0f;
	matDiffuse[13][1] = 0.50980392f;
	matDiffuse[13][2] = 0.50980392f;
	
	matDiffuse[14][0] = 0.1f;
	matDiffuse[14][1] = 0.35f;
	matDiffuse[14][2] = 0.1f;
	
	matDiffuse[15][0] = 0.5f;
	matDiffuse[15][1] = 0.0f;
	matDiffuse[15][2] = 0.0f;
	
	matDiffuse[16][0] = 0.55f;
	matDiffuse[16][1] = 0.55f;
	matDiffuse[16][2] = 0.55f;
	
	matDiffuse[17][0] = 0.5f;
	matDiffuse[17][1] = 0.5f;
	matDiffuse[17][2] = 0.0f;
	
	
	matDiffuse[18][0] = 0.01f;
	matDiffuse[18][1] = 0.01f;
	matDiffuse[18][2] = 0.01f;
	
	matDiffuse[19][0] = 0.4f;
	matDiffuse[19][1] = 0.5f;
	matDiffuse[19][2] = 0.5f;
	
	matDiffuse[20][0] = 0.4f;
	matDiffuse[20][1] = 0.5f;
	matDiffuse[20][2] = 0.4f;
	
	matDiffuse[21][0] = 0.5f;
	matDiffuse[21][1] = 0.4f;
	matDiffuse[21][2] = 0.4f;
	
	matDiffuse[22][0] = 0.5f;
	matDiffuse[22][1] = 0.5f;
	matDiffuse[22][2] = 0.5f;
	
	matDiffuse[23][0] = 0.5f;
	matDiffuse[23][1] = 0.5f;
	matDiffuse[23][2] = 0.4f;
	
	// ***** SPECULAR ******
	matSpecular[0][0] = 0.633f;
	matSpecular[0][1] = 0.727811f;
	matSpecular[0][2] = 0.633f;
	
	matSpecular[1][0] = 0.316228f;
	matSpecular[1][1] = 0.316228f;
	matSpecular[1][2] = 0.316228f;
	
	matSpecular[2][0] = 0.332741f;
	matSpecular[2][1] = 0.328634f;
	matSpecular[2][2] = 0.346435f;
	
	matSpecular[3][0] = 0.296648f;
	matSpecular[3][1] = 0.296648f;
	matSpecular[3][2] = 0.296648f;
	
	matSpecular[4][0] = 0.727811f;
	matSpecular[4][1] = 0.626959f;
	matSpecular[4][2] = 0.626959f;
	
	matSpecular[5][0] = 0.297254f;
	matSpecular[5][1] = 0.30829f;
	matSpecular[5][2] = 0.36678;
	
	
	matSpecular[6][0] = 0.992157f;
	matSpecular[6][1] = 0.941176f;
	matSpecular[6][2] = 0.807843f;
	
	matSpecular[7][0] = 0.393548f;
	matSpecular[7][1] = 0.271906f;
	matSpecular[7][2] = 0.166721f;
	
	matSpecular[8][0] = 0.774597f;
	matSpecular[8][1] = 0.774597f;
	matSpecular[8][2] = 0.774597f;
	
	matSpecular[9][0] = 0.256777f;
	matSpecular[9][1] = 0.137622f;
	matSpecular[9][2] = 0.086014f;
	
	matSpecular[10][0] = 0.628281f;
	matSpecular[10][1] = 0.555802f;
	matSpecular[10][2] = 0.366065f;
	
	matSpecular[11][0] = 0.508273f;
	matSpecular[11][1] = 0.508273f;
	matSpecular[11][2] = 0.508273f;
	
	
	matSpecular[12][0] = 0.5f;
	matSpecular[12][1] = 0.5f;
	matSpecular[12][2] = 0.5f;
	
	matSpecular[13][0] = 0.50196078f;
	matSpecular[13][1] = 0.50196078f;
	matSpecular[13][2] = 0.50196078f;
	
	matSpecular[14][0] = 0.45f;
	matSpecular[14][1] = 0.55f;
	matSpecular[14][2] = 0.45f;
	
	matSpecular[15][0] = 0.7f;
	matSpecular[15][1] = 0.6f;
	matSpecular[15][2] = 0.6f;
	
	matSpecular[16][0] = 0.7f;
	matSpecular[16][1] = 0.7f;
	matSpecular[16][2] = 0.7f;
	
	matSpecular[17][0] = 0.6f;
	matSpecular[17][1] = 0.6f;
	matSpecular[17][2] = 0.5f;
	
	
	matSpecular[18][0] = 0.4f;
	matSpecular[18][1] = 0.4f;
	matSpecular[18][2] = 0.4f;
	
	matSpecular[19][0] = 0.04f;
	matSpecular[19][1] = 0.7f;
	matSpecular[19][2] = 0.7f;
	
	matSpecular[20][0] = 0.04f;
	matSpecular[20][1] = 0.7f;
	matSpecular[20][2] = 0.04f;
	
	matSpecular[21][0] = 0.704f;
	matSpecular[21][1] = 0.04f;
	matSpecular[21][2] = 0.04f;
	
	matSpecular[22][0] = 0.7f;
	matSpecular[22][1] = 0.7f;
	matSpecular[22][2] = 0.7f;
	
	matSpecular[23][0] = 0.7f;
	matSpecular[23][1] = 0.7f;
	matSpecular[23][2] = 0.04f;
	
	// **** MATERIAL SHINIESS *****
	matShininess[0] = 0.6f * 128.0f;
	matShininess[1] = 0.1f * 128.0f;
	matShininess[2] = 0.3f * 128.0f;
	matShininess[3] = 0.088f * 128.0f;
	matShininess[4] = 0.6f * 128.0f;
	matShininess[5] = 0.1f * 128.0f;
	
	matShininess[6] = 0.21794872f * 128.0f;
	matShininess[7] = 0.2f * 128.0f;
	matShininess[8] = 0.6f * 128.0f;
	matShininess[9] = 0.1f * 128.0f;
	matShininess[10] = 0.4f * 128.0f;
	matShininess[11] = 0.4f * 128.0f;
	
	matShininess[12] = 0.25f * 128.0f;
	matShininess[13] = 0.25f * 128.0f;
	matShininess[14] = 0.25f * 128.0f;
	matShininess[15] = 0.25f * 128.0f;
	matShininess[16] = 0.25f * 128.0f;
	matShininess[17] = 0.25f * 128.0f;
	
	matShininess[18] = 0.078125f * 128.0f;
	matShininess[19] = 0.078125f * 128.0f;
	matShininess[20] = 0.078125f * 128.0f;
	matShininess[21] = 0.078125f * 128.0f;
	matShininess[22] = 0.078125f * 128.0f;
	matShininess[23] = 0.078125f * 128.0f;
}

void drawSpheres(void) 
{
	void setUniforms(XMVECTOR, XMVECTOR, XMVECTOR, float);
	
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 6; j++) {
			d3dViewPort.TopLeftX = i * (winWidth / 4);
			d3dViewPort.TopLeftY = j * (winHeight / 6);
			d3dViewPort.Width = (float)winWidth / 4;
			d3dViewPort.Height = (float)winHeight / 6;
			
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);
			
			setUniforms(XMVectorSet(matAmbient[(i * 6) + j][0], matAmbient[(i * 6) + j][1], matAmbient[(i * 6) + j][2], 1.0f),
			XMVectorSet(matDiffuse[(i * 6) + j][0], matDiffuse[(i * 6) + j][1], matDiffuse[(i * 6) + j][2], 1.0f),
			XMVectorSet(matSpecular[(i * 6) + j][0], matSpecular[(i * 6) + j][1], matSpecular[(i * 6) + j][2], 1.0f),
			matShininess[(i * 6) + j]);
			
			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
		}
	}
}


void update(void) 
{
	if (gAngle <= 360.0f) {
		gAngle += 0.0035f;
	}
	else {
		gAngle = 0.0f;
	}
	
	if (KeyPressed == 1) {
		lightPosition[0] = 0.0f;
		lightPosition[1] = 100.0f * sin(gAngle);
		lightPosition[2] = 100.0f * cos(gAngle);
		lightPosition[3] = 1.0f;
	}
	else if (KeyPressed == 2) {
		lightPosition[0] = 100.0f * cos(gAngle);
		lightPosition[1] = 0.0f;
		lightPosition[2] = 100.0f * sin(gAngle);
		lightPosition[3] = 1.0f;
	}
	else if (KeyPressed == 3) {
		lightPosition[0] = 100.0f * cos(gAngle);
		lightPosition[1] = 100.0f * sin(gAngle);
		lightPosition[2] = 0.0f;
		lightPosition[3] = 1.0f;
	}
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

	if (gpID3D11Buffer_VertexBuffer_Sphere_Normal) {
		gpID3D11Buffer_VertexBuffer_Sphere_Normal->Release();
		gpID3D11Buffer_VertexBuffer_Sphere_Normal = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Sphere_Position) {
		gpID3D11Buffer_VertexBuffer_Sphere_Position->Release();
		gpID3D11Buffer_VertexBuffer_Sphere_Position = NULL;
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
