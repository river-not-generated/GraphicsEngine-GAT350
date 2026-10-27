#pragma once

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3_image/SDL_image.h"
#include "Math/Vector2.h"

namespace nu
{
	class Renderer {
	public:
		bool Initialize(const char* name, float width, float height, bool fullscreen = false);

		void Clear();
		void Present();

		bool BeginFrame();
		bool EndFrame();

		float GetWindowWidth() { return m_width; }
		float GetWindowHeight() { return m_height; }

		void SetColour(float r, float g, float b, float a = 255.0f) const;
		void SetColour(const SDL_Color& colour) const;
		void SetColourRandom() const;

		void DrawPoint(float x, float y) const;
		void DrawFillRect(float x, float y, float width, float height) const;
		void DrawFillRect(const SDL_FRect* rect) const;
		void DrawRect(float x, float y, float width, float height) const;
		void DrawRect(const SDL_FRect* rect) const;
		void DrawLine(float x1, float y1, float x2, float y2) const;

		void DrawTexture(const class Texture& texture, float x, float y, float angle = 0.0f, float scale = 1.0f, bool flipH = false) const;
		void DrawTexture(const class Texture& texture, const struct Transform& trans, bool flipH = false, const Vector2& origin = { 0.5f, 0.5f }) const;
		void DrawTexture(const class Texture& texture, const struct Rect& source, const struct Transform& trans, bool flipH = false, const Vector2& origin = { 0.5f, 0.5f }) const;
		void DrawTexture(const class Texture& texture, const struct Rect& source, const struct Vector2& position, float rotation, float scale, bool flipH = false, const Vector2& origin = { 0.5f, 0.5f }) const;

		void DrawDebugText(float x, float y, const char* text) const;

		// SDL GPU

		SDL_GPUDevice* GetGPUDevice() const { return m_gpuDevice; }
		SDL_Window* GetWindow() const { return m_window; }

		void SetPipeline(const class Pipeline& pipeline);
		void SetVertexBuffer(const class VertexBuffer& vertexBuffer);
		void SetIndexBuffer(const class IndexBuffer& indexBuffer);

		void Draw(uint32_t vertexCount);

		void DrawIndex(const class IndexBuffer& indexBuffer);

		void SetVertexUniform(uint32_t slot, const void* data, uint32_t size);
		void SetFragmentUniform(uint32_t slot, const void* data, uint32_t size);
		void SetFragmentTexture(uint32_t slot, const class Texture& texture);

		// THE Z BUFFER
		void CreateDepthBuffer();


		void Delay(int ms);

		void Shutdown();

		friend class Text;
		friend class Texture;
		friend class Shader;
		friend class VertexBuffer;
		friend class IndexBuffer;

	private:
		SDL_Window* m_window = nullptr;
		SDL_Renderer* m_renderer = nullptr;

		SDL_GPUDevice* m_gpuDevice = nullptr;
		SDL_GPUCommandBuffer* m_cmdBuffer = nullptr;
		SDL_GPURenderPass* m_renderPass = nullptr;

		SDL_GPUTexture* m_depthTexture = nullptr;

		float m_width = 1024.0f;
		float m_height = 1080.0f;

		bool m_cameraEnabled = false;
	};
};