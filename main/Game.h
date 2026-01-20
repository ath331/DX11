#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief Game을 동작 시키는 대표 클래스
////////////////////////////////////////////////////////////////////////////////////////////////////
class Game
{
private:
	/// 윈도우 핸들
	HWND m_hwnd;

	/// 윈도우 창 넓이
	uint32 m_width;

	/// 윈도우 창 높이
	uint32 m_height;

private:
	/// Device
	ComPtr< ID3D11Device > m_device;

	/// Context
	ComPtr< ID3D11DeviceContext > m_deviceContext;

	/// SwapChain
	ComPtr< IDXGISwapChain > m_swapChain;

	/// RTV
	ComPtr< ID3D11RenderTargetView > m_renderTargetView;

	/// RTV - ClearColor
	float m_clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };

	/// Misc
	D3D11_VIEWPORT m_viewPort;

private:
	/// Geometry
	std::vector< Vertex > m_vertices;

	/// VertexBufffer
	ComPtr< ID3D11Buffer > m_vertexBuffer;

	/// InputLayOut
	ComPtr< ID3D11InputLayout > m_inputLayout;

	/// VS
	ComPtr< ID3D11VertexShader > m_vertexShader;
	ComPtr< ID3DBlob > m_vsBlob;

	/// PS
	ComPtr< ID3D11PixelShader > m_pixelShader;
	ComPtr< ID3DBlob > m_psBlob;

public:
	/// 생성자
	Game();

	/// 소멸자
	~Game();

	/// 클래스를 초기화한다.
	void Init( HWND hwnd );

	/// 업데이트 한다.
	void Update();

	/// 렌더링을 한다.
	void Render();

private:
	/// 렌더링을 시작한다.
	void _RenderBegin();

	/// 렌더링을 종료한다.
	void _RenderEnd();

private:
	/// Device와 SwapChain 객체를 생성한다.
	void _CreateDeviceAndSwapChain();

	/// RenderTargetView 객체를 생성한다.
	void _CreateRenderTargetView();

	/// ViewPort를 세팅한다.
	void _SetViewPort();

private:
	/// Geometry를 생성한다.
	void _CreateGeometry();

	/// InputLayout을 생성한다.
	void _CreateInputLayout();

	/// VS를 생성한다.
	void _CreateVS();

	/// PS를 생성한다.
	void _CreatePS();

	/// Shader 파일을 로드한다.
	void _LoadShaderFromFile(
		const std::wstring&            path,
		const std::string&             name,
		const std::string&             version,
		      ComPtr< ID3DBlob >& blob );
};

