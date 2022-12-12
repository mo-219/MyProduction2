#include "PostprocessingRenderer.h"
#include "Graphics/Graphics.h"

PostprocessingRenderer::PostprocessingRenderer()
{
    Graphics& graphics = Graphics::Instance();
    renderSprite = std::make_unique<Sprite>();
    // ���P�x���o�p�`��^�[�Q�b�g�𐶐�
    luminanceExtractRenderTarget = std::make_unique<RenderTarget>(static_cast<UINT>(graphics.GetScreenWidth()),
                                                                  static_cast<UINT>(graphics.GetScreenHeight()), DXGI_FORMAT_R8G8B8A8_UNORM);

    // ���P�x���o�ڂ����p�`��^�[�Q�b�g�𐶐�
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

    // ���ݐݒ肳��Ă���o�b�t�@��ޔ����ď��������Ă���
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
        // ���P�x���o�p�o�b�t�@�ɕ`����ύX���č��P�x���o
        {
            // �`����ύX
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


            // ���P�x���o����
            SpriteShader* shader = graphics.GetShader(SpriteShaderId::LuminanceExtraction);

            shader->Begin(rc);

            // �`��Ώۂ�ύX
            renderSprite->SetShaderResourceView(sceneData.srv, sceneData.width, sceneData.height);

            renderSprite->Update(0, 0, viewport.Width, viewport.Height,
                0, 0, static_cast<float>(sceneData.width), static_cast<float>(sceneData.height),
                0,
                1, 1, 1, 1);

            shader->Draw(rc, renderSprite.get());

            shader->End(rc);
        }


        // ���o�������P�x�`��Ώۂ�򂵂ď�������
        {
            // �`����ύX
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

            // ���P�x���o����
            SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);

            // X�����Ƀu���[
            {
                rc.BlurCount = 0;

                shader->Begin(rc);

                // �`��Ώۂ�ύX
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
            // Y�����Ƀu���[
            {
                rc.BlurCount = 1;

                shader->Begin(rc);

                // �`��Ώۂ�ύX
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


    // ���̃o�b�t�@�ɖ߂�
    {
        device->RSSetViewports(cachedViewportCount, cachedViewports);
        device->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
     }


    // �ŏI�p�X
    {
        // ���P�x���o����
        SpriteShader* shader = graphics.GetShader(SpriteShaderId::Finalpass);

        shader->Begin(rc);

        // �`��Ώۂ�ύX
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
        // �V�F�[�_�ɓn���e�N�X�`���̐ݒ�
        rc.finalpassData.bloomTexture = luminanceExtractBokehRenderTarget->GetShaderResourceView().Get();

        shader->Draw(rc, renderSprite.get());

        shader->End(rc);
    }


}

void PostprocessingRenderer::Render(const RenderContext* renderContext)
{
    Graphics& graphics = Graphics::Instance();

    // ���ݐݒ肳��Ă���o�b�t�@��ޔ����ď��������Ă���
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
        // ���P�x���o�p�o�b�t�@�ɕ`����ύX���č��P�x���o
        {
            // �`����ύX
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


            // ���P�x���o����
            SpriteShader* shader = graphics.GetShader(SpriteShaderId::LuminanceExtraction);

            shader->Begin(rc);

            // �`��Ώۂ�ύX
            renderSprite->SetShaderResourceView(sceneData.srv, sceneData.width, sceneData.height);

            renderSprite->Update(0, 0, viewport.Width, viewport.Height,
                0, 0, static_cast<float>(sceneData.width), static_cast<float>(sceneData.height),
                0,
                1, 1, 1, 1);

            shader->Draw(rc, renderSprite.get());

            shader->End(rc);
        }


        // ���o�������P�x�`��Ώۂ�򂵂ď�������
        {
            // �`����ύX
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

            // ���P�x���o����
            SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);

            // X�����u���[
            {
                rc.BlurCount = 0;

                shader->Begin(rc);

                // �`��Ώۂ�ύX
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
            // Y�����u���[
            {
                rc.BlurCount = 1;

                shader->Begin(rc);

                // �`��Ώۂ�ύX
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


    // ���̃o�b�t�@�ɖ߂�
    {
       
         rc.deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);
         rc.deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
    }


    // �ŏI�p�X
    {
        // ���P�x���o����
        SpriteShader* shader = graphics.GetShader(SpriteShaderId::Finalpass);

        shader->Begin(rc);

        // �`��Ώۂ�ύX
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
        // �V�F�[�_�ɓn���e�N�X�`���̐ݒ�
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
