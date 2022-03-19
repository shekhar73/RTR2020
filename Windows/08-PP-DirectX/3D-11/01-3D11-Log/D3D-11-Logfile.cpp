#include <stdio.h>
#include <d3d11.h>
#include <math.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")	// Directx Graphics Infrastructure it is analogus with wgi

int main(void)
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
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Graphic card name = %s\n", str);
	printf("Graphic card VRAM = %I64d bytes\n", (__int64)dxgiAdapterDesc.DedicatedVideoMemory);
	printf("VRAM in GB = %d\n", int(ceil(dxgiAdapterDesc.DedicatedVideoMemory / 1024.0 / 1024.0 / 1024.0)));
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

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

	return 0;
	
}

