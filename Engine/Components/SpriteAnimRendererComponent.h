#pragma once

#include "SpriteRendererComponent.h"
#include "Resources/ResourceManager.h"

#include <string>

namespace nu
{
	class SpriteAnimRendererComponent : public SpriteRendererComponent {
	public:
		CLASS_PROTOTYPE(SpriteAnimRendererComponent)

		void OnStart() override;
		void Update(float dt) override;

		void Read(const json::value_t& value) override;

	private:
		float m_fps = 1.0f;
		float m_frameTimer = 0.0f;

		bool m_loop = true;
		int m_frame = 0;

		std::string m_textureFramesName;
		res_t<class TextureFrames> m_textureFrames;

	};
}