////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief Game을 동작 시키는 대표 클래스
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Game.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief 생성자
////////////////////////////////////////////////////////////////////////////////////////////////////
Game::Game()
{
	m_hwnd   = (HWND)( 0 );
	m_width  = GWinSizeX;
	m_height = GWinSizeY;

	m_viewPort = { 0 };

	m_device        = nullptr;
	m_deviceContext = nullptr;
	m_swapChain     = nullptr;

	m_vertexBuffer = nullptr;
	m_inputLayout = nullptr;

	m_vertexShader = nullptr;
	m_vsBlob = nullptr;

	m_pixelShader = nullptr;
	m_psBlob = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief 소멸자
////////////////////////////////////////////////////////////////////////////////////////////////////
Game::~Game()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief 클래스를 초기화한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::Init( HWND hwnd )
{
	m_hwnd = hwnd;

	_CreateDeviceAndSwapChain();
	_CreateRenderTargetView();
	_SetViewPort();

	_CreateGeometry(); // 삼각형의 기하학 도형을 생성
	_CreateVS(); // VertexShader를 로드함
	_CreateInputLayout();
	_CreatePS(); // PixelShader를 로드함
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief 업데이트 한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::Update()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief 렌더링을 한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::Render()
{
	_RenderBegin();

	// TODO
	// IA - VS - RS - PS - OM
	{
		uint32 stride = sizeof( Vertex );
		uint32 offset = 0;

		// IA
		m_deviceContext->IASetVertexBuffers( 0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset );
		m_deviceContext->IASetInputLayout( m_inputLayout.Get() );
		m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		// VS
		m_deviceContext->VSSetShader( m_vertexShader.Get(), nullptr, 0 );

		// RS
		
		// PS
		m_deviceContext->PSSetShader( m_pixelShader.Get(), nullptr, 0 );

		// OM


		m_deviceContext->Draw( (uint32)( m_vertices.size() ), 0 );
	}

	_RenderEnd();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief 렌더링을 시작한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::_RenderBegin()
{
	m_deviceContext->OMSetRenderTargets( 1, m_renderTargetView.GetAddressOf(), nullptr );
	m_deviceContext->ClearRenderTargetView( m_renderTargetView.Get(), m_clearColor );
	m_deviceContext->RSSetViewports( 1, &m_viewPort );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief 렌더링을 종료한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::_RenderEnd()
{
	// 전면버퍼와 후면버퍼를 이용하고있고, _RenderBegin()를 통해서 후면 버퍼에 완성시킨걸
	// 전면 버퍼로 옮겨달라는 의미
	HRESULT hr = m_swapChain->Present( 1, 0 );
	CHECK( hr );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief Device와 SwapChain 객체를 생성한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::_CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory( &desc, sizeof( desc ) );
	{
		desc.BufferDesc.Width                   = m_width;
		desc.BufferDesc.Height                  = m_height;
		desc.BufferDesc.RefreshRate.Numerator   = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count                   = 1;
		desc.SampleDesc.Quality                 = 0;
		desc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount                        = 1;
		desc.OutputWindow                       = m_hwnd;
		desc.Windowed                           = true;
		desc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
	}

	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		m_swapChain.GetAddressOf(),
		m_device.GetAddressOf(),
		nullptr,
		m_deviceContext.GetAddressOf() );

	HR_LOG( hr );
	CHECK( hr );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief RenderTargetView 객체를 생성한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::_CreateRenderTargetView()
{
	ComPtr < ID3D11Texture2D > backBuffer = nullptr;

	HRESULT hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)( backBuffer.GetAddressOf() ) );
	CHECK( hr );

	m_device->CreateRenderTargetView( backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf() );
	CHECK( hr );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief ViewPort를 세팅한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::_SetViewPort()
{
	m_viewPort.TopLeftX = 0.f;
	m_viewPort.TopLeftY = 0.f;
	m_viewPort.Width = static_cast< float> ( m_width );
	m_viewPort.Height = static_cast< float> ( m_height );
	m_viewPort.MinDepth = 0.f;
	m_viewPort.MaxDepth = 1.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief Geometry를 생성한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::_CreateGeometry()
{
	/// Vertex
	{
		m_vertices.resize( 3 );

		m_vertices[ 0 ].position = Vec3 ( -0.5f, -0.5f, 0.f  );
		m_vertices[ 0 ].color    = Color( 1.f, 0.f, 0.f, 1.f );
		m_vertices[ 1 ].position = Vec3 ( 0.f, 0.5f, 0.f     );
		m_vertices[ 1 ].color    = Color( 0.f, 0.f, 1.f, 1.f );
		m_vertices[ 2 ].position = Vec3 ( 0.5f, -0.5f, 0.f   );
		m_vertices[ 2 ].color    = Color( 0.f, 1.f, 0.f, 1.f );
	}

	/// VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = (uint32)( sizeof( Vertex ) * m_vertices.size() );

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory( &data, sizeof( data ) );
		data.pSysMem = m_vertices.data();

		m_device->CreateBuffer( &desc, &data, m_vertexBuffer.GetAddressOf() );;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief InputLayout을 생성한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::_CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	const int32 count = sizeof( layout ) / sizeof( D3D11_INPUT_ELEMENT_DESC );
	m_device->CreateInputLayout( 
		layout, 
		count, 
		m_vsBlob->GetBufferPointer(), 
		m_vsBlob->GetBufferSize(), 
		m_inputLayout.GetAddressOf() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief VS를 생성한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::_CreateVS()
{
	_LoadShaderFromFile( L"98.Shaders/Default.hlsl", "VS", "vs_5_0", m_vsBlob );

	HRESULT hr = m_device->CreateVertexShader(
		m_vsBlob->GetBufferPointer(),
		m_vsBlob->GetBufferSize(),
		nullptr,
		m_vertexShader.GetAddressOf() );

	HR_LOG( hr );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief PS를 생성한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::_CreatePS()
{
	_LoadShaderFromFile( L"98.Shaders/Default.hlsl", "PS", "ps_5_0", m_psBlob );

	HRESULT hr = m_device->CreatePixelShader(
		m_psBlob->GetBufferPointer(),
		m_psBlob->GetBufferSize(),
		nullptr,
		m_pixelShader.GetAddressOf() );

	HR_LOG( hr );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief Shader 파일을 로드한다.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::_LoadShaderFromFile(
	const std::wstring&            path,
	const std::string&             name,
	const std::string&             version,
	      ComPtr< ID3DBlob >& blob )
{
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		blob.GetAddressOf(),
		nullptr );

	HR_LOG( hr );
}
