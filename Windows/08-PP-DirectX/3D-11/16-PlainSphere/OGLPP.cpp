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
IDXGISwapChain* gpIDXGISwapChain;
ID3D11Device* gpID3D11Device = NULL;
ID3D11DeviceContext* gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView* gpID3D11RenderTargetView = NULL;

ID3D11VertexShader* gpID3D11VertexShader = NULL;
ID3D11PixelShader* gpID3D11PixelShader = NULL;
ID3D11Buffer* gpID3D11Buffer_VertexBuffer_Position = NULL;
ID3D11Buffer* gpID3D11Buffer_VertexBuffer_Normal = NULL;
ID3D11Buffer* gpID3D11Buffer_ConstantBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_IndexBuffer = NULL;

ID3D11InputLayout* gpID3D11InputLayout = NULL;
ID3D11DepthStencilView* gpID3D11DepthStencilView = NULL;
ID3D11RasterizerState* gpID3D11RasterizerState = NULL;

struct CBUFFER
{
	XMMATRIX WorldViewProjectionMatrix;
	//XMMATRIX ProjectionMatrix;		// ModelViewProjectionMatrix in OpenGL i.e. Modeling Transformation
	XMVECTOR Ld;
	XMVECTOR Kd;
	XMVECTOR LightPosition;
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

bool bLight = false;
float angle_cube = 0.0f;

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

		default:
			break;
		}
		break;

		case WM_CHAR:
		switch (wParam)
			{
			case 'L':
			case 'l':
				if (bLight == true)
				{
					bLight = false;
				}
				else
				{
					bLight = true;
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
	const char* vertexShaderSourceCode =
		"cbuffer ConstantBuffer"	\
		"{"	\
			"float4x4 worldViewProjectionMatrix;"	\
		"}"	\
		"float4 main(float4 pos : POSITION) : SV_POSITION"	\
		"{"	\
			"float4 position = mul(worldViewProjectionMatrix, pos);"	\
			"return(position);"	\
		"}";

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

	hr = gpID3D11Device->CreateVertexShader(
		pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader
	);
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

	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);


	//----------------------------------------------------------------------------------------------------------

	const char* pixelShaderSourceCode =
		"float4 main(void) : SV_TARGET"	\
		"{"	\
			"return (float4(1.0f, 1.0f, 1.0f, 1.0f));"	\
		"}";



	ID3DBlob* pID3DBlob_PixelShaderCode = NULL;
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


	// create Pixel Shader
	hr = gpID3D11Device->CreatePixelShader(
		pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader
	);

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

	// Release PixelShaderCode
	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;


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

	hr = gpID3D11Device->CreateInputLayout(
		inputElementDesc,	// not we are sending array so no & (array itself as base address)
		ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout
	);

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


	//-----------------------------------------------------------------------------------------
	// VERTICES
	//-----------------------------------------------------------------------------------------

	// SPHERE RELATED CODE
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	// create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;	// 4th parameter of glBufferData() in OpenGL like GL_STATIC_DRAW
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float); // 2nd parameter of glBufferData() in OpenGL
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_Position); // ya fun madla 2nd parameter ha D3D11_SUBRESOURCE_STRUCTURE ahe. It is NULL bcz we dont send static data, we are sending the data memcpy unmap way, and it is a std way of sending the data
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
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_VertexBuffer_Position,	// to whom we have to map (konala map karaych ahe)
		0,										// on which index we have to map, mazyakde ekach ahe, multiple nahi so 0
		D3D11_MAP_WRITE_DISCARD,				// how to map? -> discard the old/existing data and write new data
		0,										// CPU ne push karaych jar GPU busy asel tar, no need to wait so 0
		&mappedSubresource						// konashi mapp karyach ahe
	);											// antrapatha chya palikadache and alikadche data mapp karyach
	memcpy(mappedSubresource.pData, sphere_vertices, gNumVertices * 3 * sizeof(float));	// vertices made aslela data pData made send karyacha ahe
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Position, 0);	// pData madla data gpID3D11Buffer_VertexBuffer_Position made janaysathi Unmap karyacha

	//-----------------------------------------------------------------------------------------
    // NORMAL
	//-----------------------------------------------------------------------------------------
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;	// 4th parameter of glBufferData() in OpenGL like GL_STATIC_DRAW
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float); // 2nd parameter of glBufferData() in OpenGL
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_Normal); // ya fun madla 2nd parameter ha D3D11_SUBRESOURCE_STRUCTURE ahe. It is NULL bcz we dont send static data, we are sending the data memcpy unmap way, and it is a std way of sending the data
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

	// copy vertices into above buffer or push the data
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_VertexBuffer_Normal,	// to whom we have to map (konala mapp karaych ahe)
		0,									// on which index we have to map, mazyakde ekach ahe, multiple nahi so 0
		D3D11_MAP_WRITE_DISCARD,			// how to map? -> discard the old/existing data and write new data
		0,									// CPU ne push karaych jar GPU busy asel tar, no need to wait so 0
		&mappedSubresource					// konashi mapp karyach ahe
	);										// antrapatha chya palikadache and alikadche data mapp karyach
	memcpy(mappedSubresource.pData, sphere_normals, gNumVertices * 3 * sizeof(float));	// vertices made aslela data pData made send karyacha ahe
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Normal, 0);	// pData madla data gpID3D11Buffer_VertexBuffer_Normal made janaysathi Unmap karyacha


	//-----------------------------------------------------------------------------------------
	// INDEX BUFFER
	//-----------------------------------------------------------------------------------------
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;	// 4th parameter of glBufferData() in OpenGL like GL_STATIC_DRAW
	bufferDesc.ByteWidth = gNumElements * 3 * sizeof(short); // 2nd parameter of glBufferData() in OpenGL
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_IndexBuffer); // ya fun madla 2nd parameter ha D3D11_SUBRESOURCE_STRUCTURE ahe. It is NULL bcz we dont send static data, we are sending the data memcpy unmap way, and it is a std way of sending the data
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Failed for Index Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Succeded for Index Buffer.\n");
		fclose(gpFile);
	}

	// copy vertices into above buffer or push the data
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_IndexBuffer,	// to whom we have to map (konala mapp karaych ahe)
		0,									// on which index we have to map, mazyakde ekach ahe, multiple nahi so 0
		D3D11_MAP_WRITE_DISCARD,			// how to map? -> discard the old/existing data and write new data
		0,									// CPU ne push karaych jar GPU busy asel tar, no need to wait so 0
		&mappedSubresource					// konashi mapp karyach ahe
	);										// antrapatha chya palikadache and alikadche data mapp karyach
	memcpy(mappedSubresource.pData, sphere_elements, gNumElements * 3 * sizeof(short));	// vertices made aslela data pData made send karyacha ahe
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_IndexBuffer, 0);	// pData madla data gpID3D11Buffer_VertexBuffer_Normal made janaysathi Unmap karyacha



	// here we didn't set this buffer in pipeline, because we have to set in Draw or display

	//-----------------------------------------------------------------------------------------
	// define and set constant buffer
	//-----------------------------------------------------------------------------------------
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.CPUAccessFlags = 0;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpID3D11Buffer_ConstantBuffer);
	if (FAILED(hr))
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

	gpID3D11DeviceContext->VSSetConstantBuffers(
		0, 	// 0th index
		1, // how many buffers
		&gpID3D11Buffer_ConstantBuffer	// which buffer
	);


	// TO ON THE CULLING FACE, BY DEFAULT IT IS ON IN DIRECTX
	//----------------------------------------------------------------
	// CREATE AND STATE RASTERRIZER STATE
	//----------------------------------------------------------------

	D3D11_RASTERIZER_DESC d3d11_rasterizer_desc;
	ZeroMemory(&d3d11_rasterizer_desc, sizeof(D3D11_RASTERIZER_DESC));

	d3d11_rasterizer_desc.AntialiasedLineEnable = FALSE;
	d3d11_rasterizer_desc.CullMode = D3D11_CULL_NONE;
	d3d11_rasterizer_desc.DepthBias = 0;
	d3d11_rasterizer_desc.DepthBiasClamp = 0.0f;
	d3d11_rasterizer_desc.DepthClipEnable = TRUE;
	d3d11_rasterizer_desc.FillMode = D3D11_FILL_SOLID;		// if we want wireframe then change here
	d3d11_rasterizer_desc.FrontCounterClockwise = FALSE;
	d3d11_rasterizer_desc.MultisampleEnable = FALSE;				// if we want it's quality is too rich then performance will be slow
	d3d11_rasterizer_desc.ScissorEnable = FALSE;
	d3d11_rasterizer_desc.SlopeScaledDepthBias = 0.0f;

	hr = gpID3D11Device->CreateRasterizerState(&d3d11_rasterizer_desc, &gpID3D11RasterizerState);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Failed for CreateRasterizerState.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device:: CreateBuffer() Succeded for CreateRasterizerState.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);


	// d3d clear color (blue)
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	// set projection matrix to identity matrix
	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	// call resize for first time
	hr = Resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
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

	// resize swap chain buffers accordingly
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
	// code
	// clear render target view to a chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);

	// select which vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Position, &stride, &offset); // 1st para : number of slot, 2nd para: number of buffers
	
	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Normal, &stride, &offset);


	// select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	

	// translation is concerned with world matirx transformation
	XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.5f);
	XMMATRIX viewMatrix	 = XMMatrixIdentity();

	// final WorldViewProjection matrix
	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	// load the data into the constant buffer
	CBUFFER constantBuffer;
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	//constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0); //  like bufferData 

	// draw vertex buffer to render target
	gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);	// 1st para: NumElements evde indices ghe, 2nd para: indices madlya 0th member pasun start kar, 3rd para: vertex buffer madlya 0th member pasun start kar

	// switch between frot & back buffers
	gpIDXGISwapChain->Present(0, 0);
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


	if (gpID3D11Buffer_VertexBuffer_Position)
	{
		gpID3D11Buffer_VertexBuffer_Position->Release();
		gpID3D11Buffer_VertexBuffer_Position = NULL;
	}

	if (gpID3D11Buffer_IndexBuffer)
	{
		gpID3D11Buffer_IndexBuffer->Release();
		gpID3D11Buffer_IndexBuffer = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Normal)
	{
		gpID3D11Buffer_VertexBuffer_Normal->Release();
		gpID3D11Buffer_VertexBuffer_Normal = NULL;
	}


	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if (gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
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
		fprintf_s(gpFile, "UnInitialize() Succeded.\n");
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}