#include "pch.h"
#include "SpriteAnimRendererComponent.h"
#include "Engine.h"
#include "Renderer/TextureFrames.h"

namespace nu
{
	FACTORY_REGISTER(SpriteAnimRendererComponent)

	void SpriteAnimRendererComponent::OnStart()
	{
		if (!m_textureFramesName.empty()) {
			m_textureFrames = Resources().Get<TextureFrames>(m_textureFramesName, Engine::Get().GetRenderer());
			if (m_textureFrames) {
				m_sourceRect = m_textureFrames->GetFrameRect(0);
				m_size = { m_sourceRect.w, m_sourceRect.h };
				m_texture = m_textureFrames->GetTexture();
			}
		}
		else {
			std::cerr << "Could not load texture frames: " << m_textureFramesName << std::endl;
		}
	}

	void SpriteAnimRendererComponent::Update(float dt)
	{
		m_frameTimer += dt;
		float frameTime = 1.0f / m_fps;
		// increase frame while frame timer is greater than frame time
		while (m_frameTimer >= frameTime) {
			m_frame++;
			if (m_loop) {
				m_frame %= m_textureFrames->GetTotalFrames();
			}
			else {
				m_frame = math::Clamp(0, m_textureFrames->GetTotalFrames() - 1, m_frame);
			}

			m_frameTimer -= frameTime;
		}

		m_sourceRect = m_textureFrames->GetFrameRect(m_frame);
	}


	void SpriteAnimRendererComponent::Read(const json::value_t& value)
	{
		SpriteRendererComponent::Read(value);

		JSON_READ_NAME_REQ(value, "fps", m_fps);
		JSON_READ_NAME(value, "loop", m_loop);

		JSON_READ_NAME_REQ(value, "texture_frames", m_textureFramesName);

	}
}
