#include <windows.h>
#include <stdio.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma warning(disable: 4838)
#include "XNAMath_204\xnamath.h"
#include "Sphere.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")	// Directx Graphics Infrastructure it is analogus with wgi
#pragma comment(lib, "Sphere.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Prototype of WndProc() delclared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declarations

DWORD dwStyle;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
FILE* gpVendorInfoFile = NULL;
FILE* gpFile = NULL;
char gszLogFileName[] = "Log.txt";
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;

float gClearColor[4];	// RGBA


ID3D11VertexShader* gpID3D11VertexShader = NULL;
ID3D11PixelShader* gpID3D11PixelShader = NULL;
ID3D11InputLayout* gpID3D11InputLayout = NULL;
ID3D11Buffer* gpID3D11Buffer_ConstantBuffer = NULL;

IDXGISwapChain* gpIDXGISwapChain;
ID3D11Device* gpID3D11Device = NULL;
ID3D11DeviceContext* gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView* gpID3D11RenderTargetView = NULL;


ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Sphere_Position = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Sphere_Normal = NULL;

ID3D11Buffer* gpID3D11Buffer_IndexBuffer = NULL;

ID3D11DepthStencilView* gpID3D11DepthStencilView = NULL;
ID3D11RasterizerState* gpID3D11RasterizerState = NULL;

struct CBUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;
	
	XMVECTOR La_R;
	XMVECTOR Ld_R;
	XMVECTOR Ls_R;
	XMVECTOR LightPosition_R;
	
	XMVECTOR La_G;
	XMVECTOR Ld_G;
	XMVECTOR Ls_G;
	XMVECTOR LightPosition_G;
	
	XMVECTOR La_B;
	XMVECTOR Ld_B;
	XMVECTOR Ls_B;
	XMVECTOR LightPosition_B;
	
	XMVECTOR Ka;
	XMVECTOR Kd;
	XMVECTOR Ks;
	
	float MaterialShininess;
	unsigned int KeyPressed;
};

// Sphere related code
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

unsigned int gNumVertices;
unsigned int gNumElements;

XMMATRIX gPerspectiveProjectionMatrix;

float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

float lightDiffuse_R[] = { 1.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse_G[] = { 0.0f, 1.0f, 0.0f, 1.0f };
float lightDiffuse_B[] = { 0.0f, 0.0f, 1.0f, 1.0f };
float lightPosition_R[] = { 10.0f, 10.0f, -10.0f, 1.0f };
float lightPosition_G[] = { 10.0f, 10.0f, -10.0f, 1.0f };
float lightPosition_B[] = { 10.0f, 10.0f, -10.0f, 1.0f };

float MaterialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float MaterialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float MaterialShininess = 50.0f;

bool bLight = false;
float gAngle = 0.0f;

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

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Programmable Pipeline : Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		((GetSystemMetrics(SM_CXSCREEN) / 2) - (WIN_WIDTH / 2)),
		((GetSystemMetrics(SM_CYSCREEN) / 2) - (WIN_HEIGHT / 2)),
		WIN_WIDTH,
		WIN_HEIGHT,
		HWND_DESKTOP,
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
	if (FAILED(hr))
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

	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
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

			if (gbActiveWindow == true)
			{
				update();

				if (gbEscapeKeyIsPressed == true)
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
		if (HIWORD(wParam) == 0)
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
		if (gpID3D11DeviceContext)
		{
			hr = Resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
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

		case 'l':
		case 'L':
			bLight = !bLight;
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
	//void printD3DInfo();

	// variable declarations
	HRESULT hr;
	//printD3DInfo();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] = { D3D_DRIVER_TYPE_HARDWARE,
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

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; numDriverTypes++)
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
			&gpID3D11Device,				// Device
			&d3dFeatureLevel_acquired,		// Feature Level
			&gpID3D11DeviceContext);		// Device Context

		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
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
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpFile, "Hardware Type.\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpFile, "Warp Type.\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpFile, "Reference Type.\n");
		}
		else
		{
			fprintf_s(gpFile, "Unkown Type.\n");
		}

		fprintf_s(gpFile, "The Supported Highest Feature Level Is ");
		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf_s(gpFile, "11.0\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
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
		"	float4 la_r;																					\n" \
		"	float4 ld_r;																					\n" \
		"	float4 ls_r;																					\n" \
		"	float4 lightPosition_r;																			\n" \
		"																									\n" \
		"	float4 la_g;																					\n" \
		"	float4 ld_g;																					\n" \
		"	float4 ls_g;																					\n" \
		"	float4 lightPosition_g;																			\n" \
		"																									\n" \
		"	float4 la_b;																					\n" \
		"	float4 ld_b;																					\n" \
		"	float4 ls_b;																					\n" \
		"	float4 lightPosition_b;																			\n" \
		"																									\n" \
		"	float4 ka;																						\n" \
		"	float4 kd;																						\n" \
		"	float4 ks;																						\n" \
		"																									\n" \
		"	float materialShininess;																		\n" \
		"	uint keyPressed;																				\n" \
		"};																									\n" \
		"																									\n" \
		"struct vertex_output {																				\n" \
		"	float4 position: SV_POSITION;																	\n" \
		"	float4 phong_ads_color: COLOR;																	\n" \
		"};																									\n" \
		"																									\n" \
		"vertex_output main(float4 pos:POSITION, float4 normal:NORMAL) {									\n" \
		"	vertex_output output;																			\n" \
		"																									\n" \
		"	if (keyPressed == 1) {																			\n" \
		"		float4 eyeCoordinates = mul(worldMatrix, pos);												\n" \
		"		eyeCoordinates = mul(viewMatrix, eyeCoordinates);											\n" \
		"		float3 transformedNorm = normalize(mul((float3x3)worldMatrix, (float3)normal));				\n" \
		"																									\n" \
		"		// Red Light																				\n" \
		"		float3 lightDirection_r = (float3)normalize((float3)lightPosition_r - eyeCoordinates);		\n" \
		"																									\n" \
		"		float tn_dot_light_direction_r = max(dot(transformedNorm, lightDirection_r), 0.0);			\n" \
		"																									\n" \
		"		float4 ambient_r = la_r * ka;																\n" \
		"		float4 diffuse_r = ld_r * kd * tn_dot_light_direction_r;									\n" \
		"																									\n" \
		"		float3 reflectionVector_r = reflect(-lightDirection_r, transformedNorm);					\n" \
		"		float3 viewerVector = normalize(-eyeCoordinates.xyz);										\n" \
		"																									\n" \
		"		float4 specular_r = ls_r * ks * pow(max(dot(reflectionVector_r, viewerVector), 0.0), materialShininess);	\n" \
		"																									\n" \
		"		float4 ads_r = ambient_r + diffuse_r + specular_r;											\n" \
		"																									\n" \
		"		// Green Light																				\n" \
		"		float3 lightDirection_g = (float3)normalize((float3)lightPosition_g - eyeCoordinates);		\n" \
		"																									\n" \
		"		float tn_dot_light_direction_g = max(dot(transformedNorm, lightDirection_g), 0.0);			\n" \
		"																									\n" \
		"		float4 ambient_g = la_g * ka;																\n" \
		"		float4 diffuse_g = ld_g * kd * tn_dot_light_direction_g;									\n" \
		"																									\n" \
		"		float3 reflectionVector_g = reflect(-lightDirection_g, transformedNorm);					\n" \
		"																									\n" \
		"		float4 specular_g = ls_g * ks * pow(max(dot(reflectionVector_g, viewerVector), 0.0), materialShininess);	\n" \
		"																									\n" \
		"		float4 ads_g = ambient_g + diffuse_g + specular_g;											\n" \
		"																									\n" \
		"		// Blue Light																				\n" \
		"		float3 lightDirection_b = (float3)normalize((float3)lightPosition_b - eyeCoordinates);		\n" \
		"																									\n" \
		"		float tn_dot_light_direction_b = max(dot(transformedNorm, lightDirection_b), 0.0);			\n" \
		"																									\n" \
		"		float4 ambient_b = la_b * ka;																\n" \
		"		float4 diffuse_b = ld_b * kd * tn_dot_light_direction_b;									\n" \
		"																									\n" \
		"		float3 reflectionVector_b = reflect(-lightDirection_b, transformedNorm);					\n" \
		"																									\n" \
		"		float4 specular_b = ls_b * ks * pow(max(dot(reflectionVector_b, viewerVector), 0.0), materialShininess);	\n" \
		"																									\n" \
		"		float4 ads_b = ambient_b + diffuse_b + specular_b;											\n" \
		"																									\n" \
		"		output.phong_ads_color = ads_r + ads_g + ads_b;												\n" \
		"	}																								\n" \
		"	else {																							\n" \
		"		output.phong_ads_color = float4(1.0, 1.0, 1.0, 1.0);										\n" \
		"	}																								\n" \
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

	ID3DBlob* pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob* pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,	// ASCII length
		"VS",									// Vertex shader
		NULL,									// MACRO
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",									// entry point function name
		"vs_5_0",								// vertex shader version number
		0,										// compiler flags for shader if any
		0,										// compiler flags for effect if any
		&pID3DBlob_VertexShaderCode,			// blob
		&pID3DBlob_Error);						// blob

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() FAILED For Vertex Shader. %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
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

	// Generally Create<> functions are called on Device
	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader);

	if (FAILED(hr))
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

	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, NULL);

	//----------------------------------------------------------------------------------------------------------

	const char *pixelShaderSourceCode =
	{
		"float4 main(float4 pos: SV_POSITION, float4 phong_ads_color: COLOR): SV_TARGET {				\n" \
		"	float4 color = phong_ads_color;																\n" \
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

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() FAILED For Pixel Shader. %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
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

	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader);

	if (FAILED(hr))
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

	
	// initialize create and set input layout structure
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];

	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;		// if we are going to send multiple geometry in the same semanticName then by seperated by indices and this is the first geometry hence it is 0
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // vertexAttribPointer cha 2nd paratmeter in OpenGL
	inputElementDesc[0].InputSlot = 0;		// Like enum of POSITON, COLOR, NORMAL, TEXTURE0 in OpenGL
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// draw vertex by using vertex data
	inputElementDesc[0].InstanceDataStepRate = 0;		// gap between 2 instnce data

	inputElementDesc[1].SemanticName = "NORMAL";
	inputElementDesc[1].SemanticIndex = 0;		// if we are going to send multiple geometry in the same semanticName then by seperated by indices and this is the first geometry hence it is 0
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT; // vertexAttribPointer cha 2nd paratmeter in OpenGL
	inputElementDesc[1].InputSlot = 1;		// Like enum of POSITON, COLOR, NORMAL, TEXTURE0 in OpenGL
	inputElementDesc[1].AlignedByteOffset = 0;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// draw vertex by using vertex data
	inputElementDesc[1].InstanceDataStepRate = 0;		// gap between 2 instnce data

	hr = gpID3D11Device->CreateInputLayout(inputElementDesc, _ARRAYSIZE(inputElementDesc), pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(), &gpID3D11InputLayout);

	if (FAILED(hr))
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

	// Release ShaderCode Objects
	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;

	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;

	//-----------------------------------------------------------------------------------------
	// VERTICES
	//-----------------------------------------------------------------------------------------

	// SPHERE RELATED CODE
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	// create vertex buffer
	// Create Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc_VertexBuffer_Sphere_Position;
	ZeroMemory(&bufferDesc_VertexBuffer_Sphere_Position, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_VertexBuffer_Sphere_Position.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer_Sphere_Position.ByteWidth = sizeof(float) * _ARRAYSIZE(sphere_vertices);
	bufferDesc_VertexBuffer_Sphere_Position.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer_Sphere_Position.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer_Sphere_Position, NULL, &gpID3D11Buffer_VertexBuffer_Sphere_Position); // ya fun madla 2nd parameter ha D3D11_SUBRESOURCE_STRUCTURE ahe. It is NULL bcz we dont send static data, we are sending the data memcpy unmap way, and it is a std way of sending the data
	
    if (FAILED(hr))
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

	// copy vertices into above buffer or push the data

	// Copy triangle vertices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource_Sphere_Position;
	ZeroMemory(&mappedSubresource_Sphere_Position, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Sphere_Position, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource_Sphere_Position);
	memcpy(mappedSubresource_Sphere_Position.pData, sphere_vertices, sizeof(sphere_vertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Sphere_Position, NULL);

	//-----------------------------------------------------------------------------------------
    // NORMAL
	//-----------------------------------------------------------------------------------------
	D3D11_BUFFER_DESC bufferDesc_VertexBuffer_Sphere_Normal;
	ZeroMemory(&bufferDesc_VertexBuffer_Sphere_Normal, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_VertexBuffer_Sphere_Normal.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer_Sphere_Normal.ByteWidth = sizeof(float) * _ARRAYSIZE(sphere_normals);
	bufferDesc_VertexBuffer_Sphere_Normal.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer_Sphere_Normal.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer_Sphere_Normal, NULL, &gpID3D11Buffer_VertexBuffer_Sphere_Normal);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Failed for Normal Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Succeded for Normal Buffer.\n");
		fclose(gpFile);
	}




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
	if (FAILED(hr))
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
	if (FAILED(hr))
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

	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

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
	if (FAILED(hr))
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
	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);


	// d3d Clear Color (blue)
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	// Set projection matrix to identity
	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	// Resize
	hr = Resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
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

	return (S_OK);
}

HRESULT Resize(int width, int height)
{
	// code
	HRESULT hr = S_OK;
	void UnInitialize();

	// depth is size dependent so release it and create again
	// depth buffer code should be before ... and after ...
	// here should be deinitialize/release previous depth stencil view

	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	// free any size-dependant resources
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}


	// just like needed for RTV, DSV also need texture buffer
	/**/
	D3D11_TEXTURE2D_DESC d3d11Texture2dDesc;
	ZeroMemory(&d3d11Texture2dDesc, sizeof(D3D11_TEXTURE2D_DESC));

	d3d11Texture2dDesc.Width = (UINT)width;
	d3d11Texture2dDesc.Height = (UINT)height;
	d3d11Texture2dDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3d11Texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	d3d11Texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3d11Texture2dDesc.SampleDesc.Count = 1;
	d3d11Texture2dDesc.SampleDesc.Quality = 0;
	d3d11Texture2dDesc.ArraySize = 1;
	d3d11Texture2dDesc.MipLevels = 1;
	d3d11Texture2dDesc.CPUAccessFlags = 0;
	d3d11Texture2dDesc.MiscFlags = 0;

	ID3D11Texture2D* pID3D11Texture2D_DepthBuffer = NULL;
	hr = gpID3D11Device->CreateTexture2D(&d3d11Texture2dDesc, NULL, &pID3D11Texture2D_DepthBuffer);	// NULL : This is D3D11_subresource_data, it is used if you have some data right now
	
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateTexture2D() FAILED. Exitting now...\n");
		fclose(gpFile);
		UnInitialize();
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateTexture2D() Succeded.\n");
		fclose(gpFile);
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC d3d11_depth_stencil_veiw_desc;
	ZeroMemory(&d3d11_depth_stencil_veiw_desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	d3d11_depth_stencil_veiw_desc.Format = DXGI_FORMAT_D32_FLOAT;
	d3d11_depth_stencil_veiw_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;	// TEXTURE2DMS : Texture 2D Mutli Sample

	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer, &d3d11_depth_stencil_veiw_desc, &gpID3D11DepthStencilView);	// NULL : This is D3D11_subresource_data, it is used if you have some data right now
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateDepthStencilView() FAILED. Exitting now...\n");
		fclose(gpFile);
		UnInitialize();
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateDepthStencilView() Succeded.\n");
		fclose(gpFile);
	}

	// Resize swap chain buffers accordingly
	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// again get back buffer from swap chain
	ID3D11Texture2D* pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);

	// again get render target view from d3d11 device using above back buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView);
	
    if (FAILED(hr))
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
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView); // 1st para: is array? If yes then 2nd parameter would be name of the array otherwise pass the addres of that variable and 3rd parameter is depth variable

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

void Display(void)
{
	static float angle_cube = 0.0f;
	// code
	// clear render target view to a chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);

	// select which vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Sphere_Position, &stride, &offset);
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Sphere_Normal, &stride, &offset);
	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX projectionMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 2.5f);


	worldMatrix = rotationMatrix * worldMatrix;
	XMMATRIX wvMatrix = worldMatrix * viewMatrix;
	projectionMatrix = gPerspectiveProjectionMatrix;


	CBUFFER constantBuffer;
	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = viewMatrix;
	constantBuffer.ProjectionMatrix = projectionMatrix;
	if (bLight == true) {
		constantBuffer.KeyPressed = 1;
		
		constantBuffer.MaterialShininess = MaterialShininess;
		
		constantBuffer.La_R = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
		constantBuffer.La_R = constantBuffer.La_G = constantBuffer.La_B;
		
		constantBuffer.Ls_R = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
		constantBuffer.Ls_R = constantBuffer.Ls_G = constantBuffer.Ls_B;
		
		constantBuffer.Ld_R = XMVectorSet(lightDiffuse_R[0], lightDiffuse_R[1], lightDiffuse_R[2], lightDiffuse_R[3]);
		constantBuffer.Ld_G = XMVectorSet(lightDiffuse_G[0], lightDiffuse_G[1], lightDiffuse_G[2], lightDiffuse_G[3]);
		constantBuffer.Ld_B = XMVectorSet(lightDiffuse_B[0], lightDiffuse_B[1], lightDiffuse_B[2], lightDiffuse_B[3]);
		
		constantBuffer.LightPosition_R = XMVectorSet(lightPosition_R[0], lightPosition_R[1], lightPosition_R[2], lightPosition_R[3]);
		constantBuffer.LightPosition_G = XMVectorSet(lightPosition_G[0], lightPosition_G[1], lightPosition_G[2], lightPosition_G[3]);
		constantBuffer.LightPosition_B = XMVectorSet(lightPosition_B[0], lightPosition_B[1], lightPosition_B[2], lightPosition_B[3]);
		
		constantBuffer.Ka = XMVectorSet(MaterialAmbient[0], MaterialAmbient[1], MaterialAmbient[2], MaterialAmbient[3]);
		constantBuffer.Kd = XMVectorSet(MaterialDiffuse[0], MaterialDiffuse[1], MaterialDiffuse[2], MaterialDiffuse[3]);
		constantBuffer.Ks = XMVectorSet(MaterialSpecular[0], MaterialSpecular[1], MaterialSpecular[2], MaterialSpecular[3]);
	}
	else {
		constantBuffer.KeyPressed = 0;
	}
	
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	// Draw vertex buffer to render target
	gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	// Switch between front and back buffers
	gpIDXGISwapChain->Present(0, 0);
}


void update(void) {
	// code
	if (gAngle <= 360.0f) {
		gAngle += 0.000075f;
	}
	else {
		gAngle = 0.0f;
	}
	
	// Red Light Motion
	lightPosition_R[1] = 100.0f * sin(gAngle);
	lightPosition_R[2] = 100.0f * cos(gAngle);
	
	// Green Light Motion
	lightPosition_G[0] = 100.0f * cos(gAngle + 22.5f);
	lightPosition_G[2] = 100.0f * sin(gAngle + 22.5f);
	
	// Blue Light Motion
	lightPosition_B[0] = 100.0f * sin(gAngle - 45.0f);
	lightPosition_B[1] = 100.0f * cos(gAngle - 45.0f);
}


void UnInitialize()
{
	// code
	if (gpID3D11Buffer_ConstantBuffer) 
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}

	if (gpID3D11InputLayout) 
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}
	
	if (gpID3D11Buffer_IndexBuffer) 
	{
		gpID3D11Buffer_IndexBuffer->Release();
		gpID3D11Buffer_IndexBuffer = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Sphere_Normal) 
	{
		gpID3D11Buffer_VertexBuffer_Sphere_Normal->Release();
		gpID3D11Buffer_VertexBuffer_Sphere_Normal = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Sphere_Position) 
	{
		gpID3D11Buffer_VertexBuffer_Sphere_Position->Release();
		gpID3D11Buffer_VertexBuffer_Sphere_Position = NULL;
	}

	if (gpID3D11PixelShader) 
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if (gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}

	if (gpID3D11RenderTargetView) 
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if (gpIDXGISwapChain) 
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}

	if (gpID3D11DeviceContext) 
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}

	if (gpID3D11Device) 
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}

	if (gpFile) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "uninitialize() Succeeded.\n");
		fprintf_s(gpFile, "Log File Closed Successfully.");
		fclose(gpFile);
	}
}
