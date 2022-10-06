#include "Misc.h"
#include "Graphics/GaussianBlurShader.h"

GaussianBlurShader::GaussianBlurShader(ID3D11Device* device)
{
    // シェーダーの生成
    {
        // 入力レイアウト
        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        // 頂点シェーダー生成
        create_vs_from_cso(device, "Shader\\GaussianBlurVS.cso",
            vertexShader.GetAddressOf(),
            inputLayout.GetAddressOf(),
            inputElementDesc, ARRAYSIZE(inputElementDesc));

        // ピクセルシェーダー生成
        create_ps_from_cso(device, "Shader\\GaussianBlurPS.cso", pixelShader.GetAddressOf());
    }
    

    // 定数バッファ
    {
        // シーン用バッファ
        D3D11_BUFFER_DESC desc;
        ::memset(&desc, 0, sizeof(desc));

        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.ByteWidth = sizeof(CBFilter);
        desc.StructureByteStride = 0;


        HRESULT hr = device->CreateBuffer(&desc, 0, filterConstantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }


    // ブレンドステート
    {
        D3D11_BLEND_DESC desc;
        ::memset(&desc, 0, sizeof(desc));

        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


        HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }


    // 深度ステンシルステート
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        ::memset(&desc, 0, sizeof(desc));

        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;


        HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }


    // ラスタライザーステート
    {
        D3D11_RASTERIZER_DESC desc;
        ::memset(&desc, 0, sizeof(desc));

        desc.FrontCounterClockwise = false;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.DepthClipEnable = true;
        desc.ScissorEnable = false;
        desc.MultisampleEnable = true;
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.AntialiasedLineEnable = false;


        HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }


    // サンプラステート
    {
        D3D11_SAMPLER_DESC desc;
        ::memset(&desc, 0, sizeof(desc));

        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        desc.BorderColor[0] = 1.0f;
        desc.BorderColor[1] = 1.0f;
        desc.BorderColor[2] = 1.0f;
        desc.BorderColor[3] = 1.0f;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;


        HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
}

void GaussianBlurShader::Begin(const RenderContext& rc)
{
    rc.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    rc.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    rc.deviceContext->IASetInputLayout(inputLayout.Get());
    rc.deviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    rc.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


    ID3D11Buffer* constantBuffers[] =
    {
        filterConstantBuffer.Get(),
    };

    rc.deviceContext->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
    rc.deviceContext->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
    
    const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    rc.deviceContext->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
    rc.deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    rc.deviceContext->RSSetState(rasterizerState.Get());
    rc.deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

}

void GaussianBlurShader::Draw(const RenderContext& rc, const Sprite* sprite)
{

    // ガウスフィルター値の計算
    CBFilter cbFilter;
    
    CalcGaussianFilter(cbFilter, rc.gaussianFilterData);
    rc.deviceContext->UpdateSubresource(filterConstantBuffer.Get(), 0, 0, &cbFilter, 0, 0);
    
    UINT stride = sizeof(Sprite::Vertex);
    UINT offset = 0;
    
    rc.deviceContext->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
    rc.deviceContext->PSSetShaderResources(0, 1, sprite->GetShaderResourceView().GetAddressOf());
    rc.deviceContext->Draw(4, 0);

}

void GaussianBlurShader::End(const RenderContext& rc)
{
    rc.deviceContext->VSSetShader(nullptr, nullptr, 0);
    rc.deviceContext->PSSetShader(nullptr, nullptr, 0);
    rc.deviceContext->IASetInputLayout(nullptr);
}

void GaussianBlurShader::CalcGaussianFilter(CBFilter& cbFilter, const GaussianFilterData& gaussianFilterData)
{
    int kernelSize = gaussianFilterData.kernelSize;

    // 偶数の場合は奇数に直す
    if (kernelSize % 2 == 0)    kernelSize++;

    kernelSize = max(1, min(MaxKernelSize - 1, MaxKernelSize));
    cbFilter.kernelSize = kernelSize;
    cbFilter.texcel.x = 1.0f / gaussianFilterData.textureSize.x;
    cbFilter.texcel.y = 1.0f / gaussianFilterData.textureSize.y;
    
    float deviationPow2 = 2.0f * gaussianFilterData.deviation * gaussianFilterData.deviation;
    float sum = 0.0f;
    int id = 0;

    for (int y = -kernelSize/2; y <= kernelSize/2; y++)
    {
        for (int x = -kernelSize/2; x <= kernelSize/2; x++)
        {
            cbFilter.weights[id].x = (float)x;
            cbFilter.weights[id].y = (float)y;
            cbFilter.weights[id].z =
                1 / (DirectX::XM_PI * deviationPow2) * expf(-((cbFilter.weights[id].x * cbFilter.weights[id].x + cbFilter.weights[id].y * cbFilter.weights[id].y) /deviationPow2));
            
            sum += cbFilter.weights[id].z;
            id++;
        }
    }
    for (int i = 0; i < cbFilter.kernelSize * cbFilter.kernelSize; i++)
    {
        cbFilter.weights[i].z /= sum;
    }

} 
