#include "pch.h"
#include "Renderer.h"
#include "Math/Random.h"
#include "Math/Transform.h"
#include "Model.h"
#include "Math/MathUtils.h"
#include "Texture.h"
#include "Math/Rect.h"
#include "Pipeline.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"


namespace nu
{
    bool Renderer::Initialize(const char* name, float width, float height, bool fullscreen)
    {
        SDL_Init(SDL_INIT_VIDEO);

        m_window = SDL_CreateWindow(name, (int)width, (int)height, (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
        if (m_window == nullptr) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }


        if (!TTF_Init()) {
            std::cerr << "TTF_Init Error: " << SDL_GetError() << std::endl;
            return false;
        }

        m_width = width;
        m_height = height;

        SDL_GPUShaderFormat formats = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL;
        m_gpuDevice = SDL_CreateGPUDevice(formats, true, "vulkan");
        if (!m_gpuDevice)
        {
            std::cerr << "Failed to create GPU Device: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            return false;
        }

        // claim the window for our modern GPU context
        SDL_ClaimWindowForGPUDevice(m_gpuDevice, m_window);

        // print out the driver being utilized (e.g., "vulkan" or "d3d12")
        std::cout << "GPU Driver Initialized: " << SDL_GetGPUDeviceDriver(m_gpuDevice) << std::endl;

        CreateDepthBuffer();

        return true;
    }

    void Renderer::Clear() {
        SDL_RenderClear(m_renderer);
    }

    void Renderer::Present() {
        SDL_RenderPresent(m_renderer);
    }

    bool Renderer::BeginFrame()
    {
        m_cmdBuffer = SDL_AcquireGPUCommandBuffer(m_gpuDevice);
        if (!m_cmdBuffer)
        {
            std::cerr << "Could not acquire command buffer: " << SDL_GetError() << std::endl;
            return false;
        }


        SDL_GPUTexture* swapchainTexture = nullptr;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(m_cmdBuffer, m_window, &swapchainTexture, nullptr, nullptr))
        {
            std::cerr << "Could not acquire swapchain texture: " << SDL_GetError() << std::endl;
            return false;
        }

        if (swapchainTexture != nullptr)
        {
            // configure the color target attachments (This handles clearing the screen)
            SDL_GPUColorTargetInfo color_target_info{};
            color_target_info.texture = swapchainTexture;
            color_target_info.clear_color = SDL_FColor{ 0.3f, 0.3f, 0.3f, 1.0f };
            color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
            color_target_info.store_op = SDL_GPU_STOREOP_STORE;

            // configure the depth target (This handles the depth sorting)
            SDL_GPUDepthStencilTargetInfo depthTargetInfo = {
                .texture = m_depthTexture,
                .clear_depth = 1.0f,
                .load_op = SDL_GPU_LOADOP_CLEAR,
                .store_op = SDL_GPU_STOREOP_DONT_CARE,
                .stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
                .stencil_store_op = SDL_GPU_STOREOP_DONT_CARE
            };

            m_renderPass = SDL_BeginGPURenderPass(m_cmdBuffer, &color_target_info, 1, &depthTargetInfo);
        }

        return true;
    }

    bool Renderer::EndFrame()
    {
        if (m_renderPass)
        {
            SDL_EndGPURenderPass(m_renderPass);
            m_renderPass = nullptr;
        }

        if (!SDL_SubmitGPUCommandBuffer(m_cmdBuffer))
        {
            std::cerr << "Could not submit command buffer: " << SDL_GetError() << std::endl;
            return false;
        }

        m_cmdBuffer = nullptr;

        return true;
    }

    void Renderer::SetPipeline(const Pipeline& pipeline)
    {
        SDL_BindGPUGraphicsPipeline(m_renderPass, pipeline.m_gpuPipeline);
    }

    void Renderer::SetVertexBuffer(const VertexBuffer& vertexBuffer)
    {
        SDL_GPUBufferBinding binding{
            .buffer = {vertexBuffer.m_gpuBuffer}, // todo: the vertex buffer's gpu buffer
            .offset = 0   // start reading at the beginning of the buffer
        };

        SDL_BindGPUVertexBuffers(m_renderPass, 0, &binding, 1);
    }

    void Renderer::SetIndexBuffer(const IndexBuffer& indexBuffer)
    {
        SDL_GPUBufferBinding binding{
            .buffer = {indexBuffer.m_gpuBuffer},
            .offset = 0 
        };

        SDL_BindGPUIndexBuffer(m_renderPass, &binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
    }

    void Renderer::Draw(uint32_t vertexCount)
    {
        SDL_DrawGPUPrimitives(m_renderPass, vertexCount, 1, 0, 0);
    }

    void Renderer::DrawIndex(const IndexBuffer& indexBuffer)
    {
        SDL_DrawGPUIndexedPrimitives(m_renderPass, indexBuffer.GetIndexCount(), 1, 0, 0, 0);
    }


    void Renderer::SetColour(float r, float g, float b, float a) const
    {
        SDL_SetRenderDrawColor(m_renderer, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a);
    }

    void Renderer::SetColour(const SDL_Color& colour) const {
        SDL_SetRenderDrawColor(m_renderer, colour.r, colour.g, colour.b, colour.a);
    }

    void Renderer::SetColourRandom() const {
        SetColour((float)RandomInt(256), (float)RandomInt(256), (float)RandomInt(256));
    }

    void Renderer::DrawPoint(float x, float y) const {
        SDL_RenderPoint(m_renderer, x, y);
    }

    void Renderer::DrawFillRect(float x, float y, float width, float height) const {
        SDL_FRect rect{ x, y, width, height };
        SDL_RenderFillRect(m_renderer, &rect);
    }

    void Renderer::DrawFillRect(const SDL_FRect* rect) const {
        SDL_RenderFillRect(m_renderer, rect);
    }

    void Renderer::DrawRect(float x, float y, float width, float height) const {
        SDL_FRect rect{ x, y, width, height };
        SDL_RenderRect(m_renderer, &rect);
    }

    void Renderer::DrawRect(const SDL_FRect* rect) const {
        SDL_RenderRect(m_renderer, rect);
    }

    void Renderer::DrawLine(float x1, float y1, float x2, float y2) const {
        SDL_RenderLine(m_renderer, x1, y1, x2, y2);
    }

    void Renderer::DrawDebugText(float x, float y, const char* text) const
    {
        SDL_RenderDebugText(m_renderer, x, y, text);
    }

    void Renderer::SetVertexUniform(uint32_t slot, const void* data, uint32_t size)
    {
        SDL_PushGPUVertexUniformData(m_cmdBuffer, slot, data, size);

    }

    void Renderer::SetFragmentUniform(uint32_t slot, const void* data, uint32_t size)
    {
        SDL_PushGPUFragmentUniformData(m_cmdBuffer, slot, data, size);
    }

    void Renderer::SetFragmentTexture(uint32_t slot, const Texture& texture)
    {
        SDL_GPUTextureSamplerBinding binding{
            .texture = texture.m_gpuTexture,
            .sampler = texture.m_gpuSampler
        };
        SDL_BindGPUFragmentSamplers(m_renderPass, slot, &binding, 1);
    }

    void Renderer::CreateDepthBuffer()
    {
        SDL_GPUTextureCreateInfo depthInfo{
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
            .width = (uint32_t) m_width,
            .height = (uint32_t) m_height,
            .layer_count_or_depth = 1,
            .num_levels = 1,
            .sample_count = SDL_GPU_SAMPLECOUNT_1
        };

        m_depthTexture = SDL_CreateGPUTexture(m_gpuDevice, &depthInfo);
    }

    void Renderer::Delay(int ms) {
        SDL_Delay(ms);
    }


    void Renderer::Shutdown() {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        TTF_Quit();
        SDL_Quit();
    }
}
