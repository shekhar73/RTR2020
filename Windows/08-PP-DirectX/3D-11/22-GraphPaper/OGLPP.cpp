#include <windows.h>
#include <stdio.h>
//#include <math.h>


#include <d3d11.h>
#include <d3dcompiler.h>

#pragma warning(disable: 4838)
#include "XNAMath_204\xnamath.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")	// Directx Graphics Infrastructure it is analogus with wgi



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


ID3D11Buffer* gpID3D11Buffer_Graph_Vertex = NULL;
ID3D11Buffer* gpID3D11Buffer_Graph_Color = NULL;

ID3D11Buffer* gpID3D11Buffer_Color_Vertex = NULL;
ID3D11Buffer* gpID3D11Buffer_Axis_Color = NULL;

ID3D11Buffer* gpID3D11Buffer_Triangle = NULL;
ID3D11Buffer* gpID3D11Buffer_Triangle_Color = NULL;

ID3D11Buffer* gpID3D11Buffer_Rectangle = NULL;
ID3D11Buffer* gpID3D11Buffer_Rectangle_Color = NULL;

ID3D11Buffer* gpID3D11Buffer_Circle_First = NULL;
ID3D11Buffer* gpID3D11Buffer_Circle_First_Color = NULL;

ID3D11Buffer* gpID3D11Buffer_Circle_Second = NULL;
ID3D11Buffer* gpID3D11Buffer_Circle_Second_Color = NULL;

float InsideCenter_XAxix;
float InsideCenter_YAxix;


ID3D11RasterizerState* gpID3D11RasterizerState = NULL;


struct CBUFFER
{
	XMMATRIX WorldViewProjectionMatrix;		// ModelViewProjectionMatrix in OpenGL i.e. Modeling Transformation
};

XMMATRIX gPerspectiveProjectionMatrix;

float angleRotation = 0.0f;

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
		TEXT("OpenGL Programmable Pipeline : Window"),
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
		const char* vertexShaderSourceCode =
		"cbuffer ConstantBuffer"\
		"{"\
			"float4x4 worldViewProjectionMatrix;"\
		"}"\
		"struct vertex_output{"\
			"float4 position:SV_POSITION;"\
			"float4 color:COLOR;"\
		"};"
		"vertex_output main(float4 pos:POSITION, float4 col:COLOR)"\
		"{"\
			"vertex_output output;"\
			"output.position = mul(worldViewProjectionMatrix, pos);"
			"output.color = col;"\
			"return(output);"\
		"}";

	
	ID3DBlob* pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob* pID3DBlob_Error = NULL;

	hr = D3DCompile(
		vertexShaderSourceCode,
		lstrlen(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error
	);

	if (FAILED(hr)) {
		if (pID3DBlob_Error != NULL) {
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Failed For Vertex Shader : %s.\n",(char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
		else{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "Some COM error.\n");
			fclose(gpFile);
		}
	}
	else {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() Succeeded For Vertex Shader.\n");
		fclose(gpFile);
	}

	hr = gpID3D11Device->CreateVertexShader(
		pID3DBlob_VertexShaderCode->GetBufferPointer(), 
		pID3DBlob_VertexShaderCode->GetBufferSize(), 
		NULL, 
		&gpID3D11VertexShader
	);

	if (FAILED(hr)) {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateVertexShader() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateVertexShader() Succeeded.\n");
		fclose(gpFile);
	}
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);
	
	const char* pixelShaderSourceCode =
		"struct vertex_output{"\
		"float4 position:SV_POSITION;"\
		"float4 color:COLOR;"\
		"};"
		"float4 main(vertex_output input) : SV_TARGET" \
		"{" \
		"return(input.color);" \
		"}";

	ID3DBlob* pID3DBlob_PixelShaderCode = NULL;
	pID3DBlob_Error = NULL;
	hr = D3DCompile(pixelShaderSourceCode,
		lstrlen(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode,
		&pID3DBlob_Error);

	if (FAILED(hr)) {
		if (pID3DBlob_Error != NULL) {
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Failed For Pixel Shader : %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() Succeeded For Pixel Shader.\n");
		fclose(gpFile);
	}

	hr = gpID3D11Device->CreatePixelShader(
		pID3DBlob_PixelShaderCode->GetBufferPointer(), 
		pID3DBlob_PixelShaderCode->GetBufferSize(), 
		NULL, 
		&gpID3D11PixelShader
	);
	if (FAILED(hr)) {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreatePixelShader() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreatePixelShader() Succeeded.\n");
		fclose(gpFile);
	}
	
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, 0, 0);
	
	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;
	
	D3D11_INPUT_ELEMENT_DESC inputElementsDesc[2];
	inputElementsDesc[0].SemanticName = "POSITION";
	inputElementsDesc[0].SemanticIndex = 0;
	inputElementsDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementsDesc[0].InputSlot = 0;
	inputElementsDesc[0].AlignedByteOffset = 0;
	inputElementsDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementsDesc[0].InstanceDataStepRate = 0;

	inputElementsDesc[1].SemanticName = "COLOR";
	inputElementsDesc[1].SemanticIndex = 0;
	inputElementsDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementsDesc[1].InputSlot = 1;
	inputElementsDesc[1].AlignedByteOffset = 0;
	inputElementsDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementsDesc[1].InstanceDataStepRate = 0;

	hr = gpID3D11Device->CreateInputLayout(
		inputElementsDesc,
		2,
		pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout
	);
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


	float x = -1.0f;
	float verticesGraph[42 * 2 * 3 * 2];
	for (int i = 0; i < 41 * 2 * 3; i = i + 6) {
		verticesGraph[i] = x;
		verticesGraph[i + 1] = 1.0f;
		verticesGraph[i + 2] = 0.0f;

		verticesGraph[i + 3] = x;
		verticesGraph[i + 4] = -1.0f;
		verticesGraph[i + 5] = 0.0f;

		x = x + 0.05f;
	}
	float y = 1.0f;
	for (int i = 41 * 2 * 3; i < 41 * 2 * 3 * 2; i = i + 6) {
		verticesGraph[i] = -1.0f;
		verticesGraph[i + 1] = y;
		verticesGraph[i + 2] = 0.0f;

		verticesGraph[i + 3] = 1.0f;
		verticesGraph[i + 4] = y;
		verticesGraph[i + 5] = 0.0f;

		y = y - 0.05f;
	}

	float colorsOfGraph[42 * 2 * 3 * 2];
	for (int i = 0; i < 42 * 2 * 3 * 2; i = i + 3) {
		colorsOfGraph[i] = 0.0f;
		colorsOfGraph[i + 1] = 0.0f;
		colorsOfGraph[i + 2] = 1.0f;
	}
	// Initialize Vertex Buffer and Create Buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(verticesGraph);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Graph_Vertex);
	if (FAILED(hr)) {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Vertex Buffer Graph.\n");
		fclose(gpFile);
		return(hr);
	}
	else {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer Graph.\n");
		fclose(gpFile);
	}
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Graph_Vertex,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, verticesGraph, sizeof(verticesGraph));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Graph_Vertex, 0);

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(colorsOfGraph);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Graph_Color);
	if (FAILED(hr)) {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Color Buffer Graph.\n");
		fclose(gpFile);
		return(hr);
	}
	else {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Color Buffer Graph.\n");
		fclose(gpFile);
	}

	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Graph_Color,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, colorsOfGraph, sizeof(colorsOfGraph));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Graph_Color, 0);

	const float AxisVertices[] = {
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, -1.0f, 0.0f
	};
	const float AxisColors[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(AxisVertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Color_Vertex);
	if (FAILED(hr)) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Vertex Buffer Axis.\n");
		fclose(gpFile);
		return(hr);
	}
	else 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer Axis.\n");
		fclose(gpFile);
	}
	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Color_Vertex,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, AxisVertices, sizeof(AxisVertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Color_Vertex, 0);

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(AxisColors);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Axis_Color);
	if (FAILED(hr)) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Color Buffer Axis.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Color Buffer Axis.\n");
		fclose(gpFile);
	}

	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Axis_Color,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, AxisColors, sizeof(AxisColors));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Axis_Color, 0);


	float radius = 0.75f;
	float circle_One_Vertices[360 * 9];
	float circle_One_Colors[360 * 9];
	float angle = 0.0f;
	for (int i = 0; i < 360 * 9; i = i + 3)
	{
		circle_One_Vertices[i] = radius * cos(angle);
		circle_One_Vertices[i + 1] = radius * sin(angle);
		circle_One_Vertices[i + 2] = 0.0f;

		angle = angle + 0.006f;

		circle_One_Colors[i] = 1.0f;
		circle_One_Colors[i + 1] = 0.05f;
		circle_One_Colors[i + 2] = 1.0f;
	}

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(circle_One_Vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Circle_First);
	if (FAILED(hr)) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Vertex Buffer Axis.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer Axis.\n");
		fclose(gpFile);
	}

	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Circle_First,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, circle_One_Vertices, sizeof(circle_One_Vertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Circle_First, 0);

	// Pixel Buffer
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(circle_One_Colors);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Circle_First_Color);
	if (FAILED(hr)) {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Color Buffer Axis.\n");
		fclose(gpFile);
		return(hr);
	}
	else {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Color Buffer Axis.\n");
		fclose(gpFile);
	}

	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Circle_First_Color,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, circle_One_Colors, sizeof(circle_One_Colors));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Circle_First_Color, 0);

	float x1 = 0.0f;
	float y1 = 0.75f;

	float x2 = (cos(120.0f) * 0.75f);
	float y2 = -(sin(120.0f) * 0.75f);

	float x3 = -(cos(120.0f) * 0.75f);
	float y3 = -(sin(120.0f) * 0.75f);
	const float triangleVertices[] = {
		x1, y1, 0.0f,
		x2, y2, 0.0f,

		x2, y2, 0.0f,
		x3, y3, 0.0f,

		x3, y3, 0.0f,
		x1, y1, 0.0f,
	};

	float triangleColors[] = {
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f
	};

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(triangleVertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Triangle);
	if (FAILED(hr)) {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Vertex Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else {
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer.\n");
		fclose(gpFile);
	}
	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Triangle,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, triangleVertices, sizeof(triangleVertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Triangle, 0);

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(triangleColors);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Triangle_Color);
	if (FAILED(hr)) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Color Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Color Buffer.\n");
		fclose(gpFile);
	}

	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Triangle_Color,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, triangleColors, sizeof(triangleColors));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Triangle_Color, 0);



	float a = sqrt(((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)));
	float b = sqrt(((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)));
	float c = sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
	InsideCenter_XAxix = ((a * x1) + (b * x2) + (c * x3)) / (a + b + c);
	InsideCenter_YAxix = ((a * y1) + (b * y2) + (c * y3)) / (a + b + c);
	float Perimeter = (a + b + c) / 2;
	float Area = sqrt(Perimeter * (Perimeter - a) * (Perimeter - b) * (Perimeter - c));
	float radius_inside_circle = Area / Perimeter;

	float circle_two_vertices[360 * 9];
	float circle_two_colors[360 * 9];

	angle = 0.0f;
	for (int i = 0; i < 360 * 9; i = i + 3) {
		circle_two_vertices[i] = radius_inside_circle * cos(angle);
		circle_two_vertices[i + 1] = radius_inside_circle * sin(angle);
		circle_two_vertices[i + 2] = 0.0f;

		angle = angle + 0.006f;

		circle_two_colors[i] = 1.5f;
		circle_two_colors[i + 1] = 0.05f;
		circle_two_colors[i + 2] = 0.0f;
	}
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(circle_two_vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Circle_Second);
	
	if (FAILED(hr)) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Vertex Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer.\n");
		fclose(gpFile);
	}
	
	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Circle_Second,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	
	memcpy(mappedSubresource.pData, circle_two_vertices, sizeof(circle_two_vertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Circle_Second, 0);


	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(circle_two_colors);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Circle_Second_Color);
	
	if (FAILED(hr)) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Color Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Color Buffer.\n");
		fclose(gpFile);
	}

	
	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Circle_Second_Color,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, circle_two_colors, sizeof(circle_two_colors));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Circle_Second_Color, 0);

	const float rectangleVertices[] = {
		-1.0f, 	0.75f, 0.0f,
		-1.0f, -0.75f, 0.0f,

		-1.0f, 	-0.75f, 0.0f,
		1.0f, 	-0.75f, 0.0f,

		1.0f, 	-0.75f, 0.0f,
		1.0f, 	0.75f, 	0.0f,

		1.0f, 	0.75f, 	0.0f,
		-1.0f, 	0.75f, 	0.0f
	};

	const float rectangleColors[] = {
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};


	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(rectangleVertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Rectangle);
	
	if (FAILED(hr)) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Vertex Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer.\n");
		fclose(gpFile);
	}

	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Rectangle,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, rectangleVertices, sizeof(rectangleVertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Rectangle, 0);

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(rectangleColors);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_Rectangle_Color);
	
	if (FAILED(hr)) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Color Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Color Buffer.\n");
		fclose(gpFile);
	}


	ZeroMemory((void*)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_Rectangle_Color,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource
	);
	memcpy(mappedSubresource.pData, rectangleColors, sizeof(rectangleColors));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_Rectangle_Color, 0);


	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc_ConstantBuffer.CPUAccessFlags = 0;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpID3D11Buffer_ConstantBuffer);

	if (FAILED(hr)) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device:CreateBuffer() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device:CreateBuffer() Succeeded For Constant Buffer.\n");
		fclose(gpFile);
	}
	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
	
	D3D11_RASTERIZER_DESC d3d11RasterizerDesc;
	ZeroMemory(&d3d11RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	d3d11RasterizerDesc.AntialiasedLineEnable = FALSE;
	d3d11RasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3d11RasterizerDesc.DepthBias = 0;
	d3d11RasterizerDesc.DepthBiasClamp = 0.0f;
	d3d11RasterizerDesc.DepthClipEnable = TRUE;
	d3d11RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3d11RasterizerDesc.FrontCounterClockwise = FALSE;
	d3d11RasterizerDesc.MultisampleEnable = FALSE;
	d3d11RasterizerDesc.ScissorEnable = FALSE;
	d3d11RasterizerDesc.SlopeScaledDepthBias = 0.0f;

	hr = gpID3D11Device->CreateRasterizerState(&d3d11RasterizerDesc, &gpID3D11RasterizerState);
	
	if (FAILED(hr)) 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateRasterizerState() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else 
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateRasterizerState() Succeeded.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);



	gClearColor[0] = 0.2f;
	gClearColor[1] = 0.2f;
	gClearColor[2] = 0.1f;
	gClearColor[3] = 1.0f;

	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	hr = Resize(WIN_WIDTH, WIN_HEIGHT);
	
	return (S_OK);
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
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	// select which vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_Graph_Vertex, &stride, &offset);


	stride = sizeof(float) * 3;
	offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_Graph_Color, &stride, &offset);
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	XMMATRIX worldMatrix = translationMatrix;
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	CBUFFER constantBuffer;
	ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);
	
	for (int i = 0; i < 41 * 2; i++) 
	{
		gpID3D11DeviceContext->Draw(2, i * 2);
	}

	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_Color_Vertex, &stride, &offset);

	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_Axis_Color, &stride, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	worldMatrix = translationMatrix;
	viewMatrix = XMMatrixIdentity();
	wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);
	for (int i = 0; i < 2 * 2; i++) 
	{
		gpID3D11DeviceContext->Draw(2, i * 2);
	}

	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_Rectangle, &stride, &offset);

	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_Rectangle_Color, &stride, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	worldMatrix = translationMatrix;
	viewMatrix = XMMatrixIdentity();
	wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);
	
	for (int i = 0; i < 8; i++) 
	{
		gpID3D11DeviceContext->Draw(2, i * 2);
	}

	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_Circle_First, &stride, &offset);

	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_Circle_First_Color, &stride, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	worldMatrix = translationMatrix;
	viewMatrix = XMMatrixIdentity();
	wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);
	
	for (int i = 0; i < 360 * 9; i++) 
	{
		gpID3D11DeviceContext->Draw(1, i);
	}


	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_Triangle, &stride, &offset);

	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_Triangle_Color, &stride, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	worldMatrix = translationMatrix;
	viewMatrix = XMMatrixIdentity();
	wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);
	
	for (int i = 0; i < 6; i++) 
	{
		gpID3D11DeviceContext->Draw(2, i*2); //last parameter is set of 2
	}


	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_Circle_Second, &stride, &offset);

	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_Circle_Second_Color, &stride, &offset);
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	translationMatrix = XMMatrixTranslation(InsideCenter_XAxix, InsideCenter_YAxix, 6.0f);
	worldMatrix = translationMatrix;
	viewMatrix = XMMatrixIdentity();
	wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);
	
	for (int i = 0; i < 360 * 9; i++)
	{
		gpID3D11DeviceContext->Draw(1, i);
	}
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
