#pragma once
#include "RendererComponent.h"
#include "Resources/Resource.h"
#include "Renderer/Texture.h"
#include "Math/Rect.h"

#include <string>

namespace nu
{
	class SpriteRendererComponent : public RendererComponent {
	public:
		CLASS_PROTOTYPE(SpriteRendererComponent)

		void OnStart() override;
		void Draw(const class Renderer& renderer) override;

		virtual void Read(const json::value_t& value);

		bool GetFlipH() const { return m_flipH; }
		void SetFlipH(bool flipH = false) { m_flipH = flipH; }
		void Flip() { m_flipH = !m_flipH; }

		const Vector2& GetSize() const { return m_size; }

	protected:
		std::string m_textureName;
		Rect m_sourceRect;
		Vector2 m_size{ 0.0f, 0.0f };
		Vector2 m_origin{ 0.5f, 0.5f };
		bool m_flipH = false;

		res_t<Texture> m_texture;
	};
}