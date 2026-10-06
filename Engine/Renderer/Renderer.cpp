#include "pch.h"
#include "Renderer.h"
#include "Math/Random.h"
#include "Math/Transform.h"
#include "Model.h"
#include "Math/MathUtils.h"
#include "Texture.h"
#include "Math/Rect.h"


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

        m_renderer = SDL_CreateRenderer(m_window, NULL);
        if (m_renderer == nullptr) {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            return false;
        }

        SDL_SetRenderVSync(m_renderer, 1);
        return true;
    }

    void Renderer::Clear() {
        SDL_RenderClear(m_renderer);
    }

    void Renderer::Present() {
        SDL_RenderPresent(m_renderer);
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

    void Renderer::DrawModel(const class Model& model, const struct Transform& trans) const {
        for (auto mesh : model.GetMeshes()) {

            auto& points = mesh.GetPoints();
            SetColour((Uint8)mesh.GetColour().r, (Uint8)mesh.GetColour().g, (Uint8)mesh.GetColour().b);

            for (int i = 0; i + 1 < points.size(); i++) {
                Vector2 v1 = points.at(i);
                Vector2 v2 = points.at(i + 1);

                // convert to world space
                v1 *= trans.scale;
                v2 *= trans.scale;

                v1 = v1.Rotate(trans.rotation * math::DEG_TO_RAD);
                v2 = v2.Rotate(trans.rotation * math::DEG_TO_RAD);

                v1 += trans.position;
                v2 += trans.position;

                DrawLine(v1.x, v1.y, v2.x, v2.y);
            }
        }
    }


    void Renderer::DrawTexture(const Texture& texture, float x, float y, float angle, float scale, bool flipH) const
    {
        Vector2 size = texture.GetSize();

        SDL_FRect destRect;
        destRect.x = x;
        destRect.y = y;
        destRect.w = size.x * scale;
        destRect.h = size.y * scale;

        SDL_SetTextureScaleMode(texture.m_texture, SDL_SCALEMODE_NEAREST);
        SDL_RenderTextureRotated(m_renderer, texture.m_texture, NULL, &destRect, angle, NULL, (flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
    }

    void Renderer::DrawTexture(const Texture& texture, const Transform& trans, bool flipH, const Vector2& origin) const {
        Vector2 size = texture.GetSize();

        SDL_FRect destRect;
        destRect.w = size.x * trans.scale;
        destRect.h = size.y * trans.scale;
        destRect.x = trans.position.x - (destRect.w * origin.x);
        destRect.y = trans.position.y - (destRect.h * origin.y);

        SDL_SetTextureScaleMode(texture.m_texture, SDL_SCALEMODE_NEAREST);
        SDL_RenderTextureRotated(m_renderer, texture.m_texture, NULL, &destRect, trans.rotation, NULL, (flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
    }


    void Renderer::DrawTexture(const Texture& texture, const Rect& source, const Vector2& position, float rotation, float scale, bool flipH, const Vector2& origin) const
    {
        SDL_FRect sourceRect;
        sourceRect.x = source.x;
        sourceRect.y = source.y;
        sourceRect.w = source.w;
        sourceRect.h = source.h;

        SDL_FRect destRect;
        destRect.w = sourceRect.w * scale;
        destRect.h = sourceRect.h * scale;
        destRect.x = position.x - (destRect.w * origin.x);
        destRect.y = position.y - (destRect.h * origin.y);

        SDL_SetTextureScaleMode(texture.m_texture, SDL_SCALEMODE_NEAREST);
        SDL_RenderTextureRotated(m_renderer, texture.m_texture, &sourceRect, &destRect, rotation, NULL, (flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
    }

    void Renderer::DrawTexture(const class Texture& texture, const struct Rect& source, const struct Transform& trans, bool flipH, const Vector2& origin) const
    {
        DrawTexture(texture, source, trans.position, trans.rotation, trans.scale, flipH, origin);
    }

    void Renderer::DrawDebugText(float x, float y, const char* text) const
    {
        SDL_RenderDebugText(m_renderer, x, y, text);
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
