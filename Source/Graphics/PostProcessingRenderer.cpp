#include "PostprocessingRenderer.h"
#include "Graphics/Graphics.h"

PostprocessingRenderer::PostprocessingRenderer()
{
    Graphics& graphics = Graphics::Instance();
    renderSprite = std::make_unique<Sprite>();
    // 高輝度抽出用描画ターゲットを生成
    luminanceExtractRenderTarget = std::make_unique<RenderTarget>(static_cast<UINT>(graphics.GetScreenWidth()),
                                                                  static_cast<UINT>(graphics.GetScreenHeight()), DXGI_FORMAT_R8G8B8A8_UNORM);

    // 高輝度抽出ぼかし用描画ターゲットを生成
    luminanceExtractBokehRenderTarget =
        std::make_unique<RenderTarget>(static_cast<UINT>(graphics.GetScreenWidth()),
            static_cast<UINT>(graphics.GetScreenHeight()), DXGI_FORMAT_R8G8B8A8_UNORM);

    bloomData.gaussianFilterData.textureSize.x = static_cast<float>(luminanceExtractBokehRenderTarget->GetWidth());
    bloomData.gaussianFilterData.textureSize.y = static_cast<float>(luminanceExtractBokehRenderTarget->GetHeight());
}

PostprocessingRenderer::~PostprocessingRenderer()
{
}

void PostprocessingRenderer::Render(ID3D11DeviceContext* device)
{
    Graphics& graphics = Graphics::Instance();

    // 現在設定されているバッファを退避して初期化しておく
    UINT cachedViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
    D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
    {
        device->RSGetViewports(&cachedViewportCount, cachedViewports);
        device->OMGetRenderTargets(1
            , cachedRenderTargetView.ReleaseAndGetAddressOf()
            , cachedDepthStencilView.ReleaseAndGetAddressOf());
    }


    RenderContext rc;
    rc.deviceContext = device;
    rc.luminanceExtractionData = bloomData.luminanceExtractionData;
    rc.gaussianFilterData = bloomData.gaussianFilterData;

    if (effectFlag)
    {
        // 高輝度抽出用バッファに描画先を変更して高輝度抽出
        {
            // 描画先を変更
            ID3D11RenderTargetView* rtv = luminanceExtractRenderTarget->GetRenderTargetView().Get();

            FLOAT color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            rc.deviceContext->ClearRenderTargetView(rtv, color);
            rc.deviceContext->OMSetRenderTargets(1, &rtv, nullptr);

            D3D11_VIEWPORT viewport{};
            viewport.Width = static_cast<float>(luminanceExtractRenderTarget->GetWidth());
            viewport.Height = static_cast<float>(luminanceExtractRenderTarget->GetHeight());
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;

            rc.deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);


            // 高輝度抽出処理
            SpriteShader* shader = graphics.GetShader(SpriteShaderId::LuminanceExtraction);

            shader->Begin(rc);

            // 描画対象を変更
            renderSprite->SetShaderResourceView(sceneData.srv, sceneData.width, sceneData.height);

            renderSprite->Update(0, 0, viewport.Width, viewport.Height,
                0, 0, static_cast<float>(sceneData.width), static_cast<float>(sceneData.height),
                0,
                1, 1, 1, 1);

            shader->Draw(rc, renderSprite.get());

            shader->End(rc);
        }


        // 抽出した高輝度描画対象を暈して書き込む
        {
            // 描画先を変更
            ID3D11RenderTargetView* rtv = luminanceExtractBokehRenderTarget->GetRenderTargetView().Get();

            FLOAT color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            rc.deviceContext->ClearRenderTargetView(rtv, color);
            rc.deviceContext->OMSetRenderTargets(1, &rtv, nullptr);

            D3D11_VIEWPORT viewport{};
            viewport.Width = static_cast<float>(luminanceExtractBokehRenderTarget->GetWidth());
            viewport.Height = static_cast<float>(luminanceExtractBokehRenderTarget->GetHeight());
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;

            rc.deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);

            // 高輝度抽出処理
            SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);

            // X方向にブラー
            {
                rc.BlurCount = 0;

                shader->Begin(rc);

                // 描画対象を変更
                renderSprite->SetShaderResourceView(luminanceExtractRenderTarget->GetShaderResourceView(),
                    luminanceExtractRenderTarget->GetWidth(),
                    luminanceExtractRenderTarget->GetHeight());

                renderSprite->Update(0, 0, viewport.Width, viewport.Height,
                    0, 0, static_cast<float>(luminanceExtractRenderTarget->GetWidth()),
                    static_cast<float>(luminanceExtractRenderTarget->GetHeight()),
                    0,
                    1, 1, 1, 1);

                shader->Draw(rc, renderSprite.get());

                shader->End(rc);
            }
            // Y方向にブラー
            {
                rc.BlurCount = 1;

                shader->Begin(rc);

                // 描画対象を変更
                renderSprite->SetShaderResourceView(luminanceExtractRenderTarget->GetShaderResourceView(),
                    luminanceExtractRenderTarget->GetWidth(),
                    luminanceExtractRenderTarget->GetHeight());

                renderSprite->Update(0, 0, viewport.Width, viewport.Height,
                    0, 0, static_cast<float>(luminanceExtractRenderTarget->GetWidth()),
                    static_cast<float>(luminanceExtractRenderTarget->GetHeight()),
                    0,
                    1, 1, 1, 1);

                shader->Draw(rc, renderSprite.get());

                shader->End(rc);
            }
        }
    }


    // 元のバッファに戻す
    {
        device->RSSetViewports(cachedViewportCount, cachedViewports);
        device->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
     }


    // 最終パス
    {
        // 高輝度抽出処理
        SpriteShader* shader = graphics.GetShader(SpriteShaderId::Finalpass);

        shader->Begin(rc);

        // 描画対象を変更
        renderSprite->SetShaderResourceView(sceneData.srv, sceneData.width, sceneData.height);

        renderSprite->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(),
            0, 0, static_cast<float>(sceneData.width),
            static_cast<float>(sceneData.height),
            0,
            1, 1, 1, 1);

#if 0
        renderSprite->SetShaderResourceView(luminanceExtractBokehRenderTarget->GetShaderResourceView(),
                                            luminanceExtractBokehRenderTarget->GetWidth(),
                                            luminanceExtractBokehRenderTarget->GetHeight());

        renderSprite->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(),
                             0, 0, static_cast<float>(luminanceExtractBokehRenderTarget->GetWidth()),
                             static_cast<float>(luminanceExtractBokehRenderTarget->GetHeight()),
                             0,
                             1, 1, 1, 1);
#endif
        // シェーダに渡すテクスチャの設定
        rc.finalpassData.bloomTexture = luminanceExtractBokehRenderTarget->GetShaderResourceView().Get();

        shader->Draw(rc, renderSprite.get());

        shader->End(rc);
    }


}

void PostprocessingRenderer::Render(const RenderContext* renderContext)
{
    Graphics& graphics = Graphics::Instance();

    // 現在設定されているバッファを退避して初期化しておく
    UINT cachedViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
    D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
    {
        renderContext->deviceContext->RSGetViewports(&cachedViewportCount, cachedViewports);
        renderContext->deviceContext->OMGetRenderTargets(1
            , cachedRenderTargetView.ReleaseAndGetAddressOf()
            , cachedDepthStencilView.ReleaseAndGetAddressOf());
    }


    RenderContext rc;
    rc.deviceContext = renderContext->deviceContext;
    rc.luminanceExtractionData = bloomData.luminanceExtractionData;
    rc.gaussianFilterData = bloomData.gaussianFilterData;
    //rc.BlurCount = renderContext->BlurCount;

    if (effectFlag)
    {
        // 高輝度抽出用バッファに描画先を変更して高輝度抽出
        {
            // 描画先を変更
            ID3D11RenderTargetView* rtv = luminanceExtractRenderTarget->GetRenderTargetView().Get();

            FLOAT color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            rc.deviceContext->ClearRenderTargetView(rtv, color);
            rc.deviceContext->OMSetRenderTargets(1, &rtv, nullptr);

            D3D11_VIEWPORT viewport{};
            viewport.Width = static_cast<float>(luminanceExtractRenderTarget->GetWidth());
            viewport.Height = static_cast<float>(luminanceExtractRenderTarget->GetHeight());
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;

            rc.deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);


            // 高輝度抽出処理
            SpriteShader* shader = graphics.GetShader(SpriteShaderId::LuminanceExtraction);

            shader->Begin(rc);

            // 描画対象を変更
            renderSprite->SetShaderResourceView(sceneData.srv, sceneData.width, sceneData.height);

            renderSprite->Update(0, 0, viewport.Width, viewport.Height,
                0, 0, static_cast<float>(sceneData.width), static_cast<float>(sceneData.height),
                0,
                1, 1, 1, 1);

            shader->Draw(rc, renderSprite.get());

            shader->End(rc);
        }


        // 抽出した高輝度描画対象を暈して書き込む
        {
            // 描画先を変更
            ID3D11RenderTargetView* rtv = luminanceExtractBokehRenderTarget->GetRenderTargetView().Get();

            FLOAT color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            rc.deviceContext->ClearRenderTargetView(rtv, color);
            rc.deviceContext->OMSetRenderTargets(1, &rtv, nullptr);

            D3D11_VIEWPORT viewport{};
            viewport.Width = static_cast<float>(luminanceExtractBokehRenderTarget->GetWidth());
            viewport.Height = static_cast<float>(luminanceExtractBokehRenderTarget->GetHeight());
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;

            rc.deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);

            // 高輝度抽出処理
            SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);

            // X方向ブラー
            {
                rc.BlurCount = 0;

                shader->Begin(rc);

                // 描画対象を変更
                renderSprite->SetShaderResourceView(luminanceExtractRenderTarget->GetShaderResourceView(),
                    luminanceExtractRenderTarget->GetWidth(),
                    luminanceExtractRenderTarget->GetHeight());

                renderSprite->Update(0, 0, viewport.Width, viewport.Height,
                    0, 0, static_cast<float>(luminanceExtractRenderTarget->GetWidth()),
                    static_cast<float>(luminanceExtractRenderTarget->GetHeight()),
                    0,
                    1, 1, 1, 1);

                shader->Draw(rc, renderSprite.get());

                shader->End(rc);
            }
            // Y方向ブラー
            {
                rc.BlurCount = 1;

                shader->Begin(rc);

                // 描画対象を変更
                renderSprite->SetShaderResourceView(luminanceExtractRenderTarget->GetShaderResourceView(),
                    luminanceExtractRenderTarget->GetWidth(),
                    luminanceExtractRenderTarget->GetHeight());

                renderSprite->Update(0, 0, viewport.Width, viewport.Height,
                    0, 0, static_cast<float>(luminanceExtractRenderTarget->GetWidth()),
                    static_cast<float>(luminanceExtractRenderTarget->GetHeight()),
                    0,
                    1, 1, 1, 1);

                shader->Draw(rc, renderSprite.get());

                shader->End(rc);
            }
        }
    }


    // 元のバッファに戻す
    {
       
         rc.deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);
         rc.deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
    }


    // 最終パス
    {
        // 高輝度抽出処理
        SpriteShader* shader = graphics.GetShader(SpriteShaderId::Finalpass);

        shader->Begin(rc);

        // 描画対象を変更
        renderSprite->SetShaderResourceView(sceneData.srv, sceneData.width, sceneData.height);

        renderSprite->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(),
            0, 0, static_cast<float>(sceneData.width),
            static_cast<float>(sceneData.height),
            0,
            1, 1, 1, 1);

#if 0
        renderSprite->SetShaderResourceView(luminanceExtractBokehRenderTarget->GetShaderResourceView(),
            luminanceExtractBokehRenderTarget->GetWidth(),
            luminanceExtractBokehRenderTarget->GetHeight());

        renderSprite->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(),
            0, 0, static_cast<float>(luminanceExtractBokehRenderTarget->GetWidth()),
            static_cast<float>(luminanceExtractBokehRenderTarget->GetHeight()),
            0,
            1, 1, 1, 1);
#endif
        // シェーダに渡すテクスチャの設定
        rc.finalpassData.bloomTexture = luminanceExtractBokehRenderTarget->GetShaderResourceView().Get();

        shader->Draw(rc, renderSprite.get());

        shader->End(rc);
    }


}


void PostprocessingRenderer::SetSceneData(ShaderResourceViewData srvData)
{
    sceneData = srvData;
}

void PostprocessingRenderer::DrawDebugGUI()
{
    if (ImGui::TreeNode("PostProcess"))
    {
        if (ImGui::Button("ON"))effectFlag = true;
        if (ImGui::Button("OFF"))effectFlag = false;

        if (ImGui::TreeNode("Bloom"))
        {
            ImGui::SliderFloat("threshold", &bloomData.luminanceExtractionData.threshold, 0.0f, 1.0f);
            ImGui::SliderFloat("intensity", &bloomData.luminanceExtractionData.intensity, 0.0f, 10.0f);
            ImGui::SliderInt("kernelSize", &bloomData.gaussianFilterData.kernelSize, 1, MaxKernelSize-1);
            ImGui::SliderFloat("deviation", &bloomData.gaussianFilterData.deviation, 1.0f, 10.0f);


            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

}
