#include "pch.h"
#include "SpriteAnimatorComponent.h"
#include "Math/MathUtils.h"
#include "Renderer/TextureFrames.h"
#include "Core/StringUtils.h"
#include "Engine.h"

namespace nu
{
	FACTORY_REGISTER(SpriteAnimatorComponent)

	void SpriteAnimatorComponent::OnStart()
	{
		if (!m_defaultAnimationName.empty()) {
			Play(m_defaultAnimationName);
		}
		else {
			// default to first element in map
			auto iter = m_animations.begin();
			std::string name = iter->first;
			Play(name);
		}
	}
	void SpriteAnimatorComponent::Update(float dt)
	{
		if (!m_currentAnimation.textureFrames) return;

		m_frameTimer += dt;
		float frameTime = 1.0f / m_currentAnimation.fps;
		// increase frame while frame timer is greater than frame time
		while (m_frameTimer >= frameTime) {
			m_frame++;
			if (m_currentAnimation.loop) {
				m_frame %= m_currentAnimation.textureFrames->GetTotalFrames();
			}
			else {
				m_frame = math::Clamp(0, m_currentAnimation.textureFrames->GetTotalFrames() - 1, m_frame);
			}

			m_frameTimer -= frameTime;
		}

		m_sourceRect = m_currentAnimation.textureFrames->GetFrameRect(m_frame);
	}

	void SpriteAnimatorComponent::Play(const std::string& name)
	{
		auto iter = m_animations.find(ToLower(name));

		if (iter == m_animations.end()) {
			std::cerr << "Could not find animation: " << name << std::endl;
			return;
		}

		m_currentAnimation = iter->second;
		m_frame = 0;
		m_frameTimer = 0.0f;

		m_texture = m_currentAnimation.textureFrames->GetTexture();
		m_sourceRect = m_currentAnimation.textureFrames->GetFrameRect(m_frame);
	}

	bool SpriteAnimatorComponent::IsAnimationDone() const
	{
		return (m_frame == m_currentAnimation.textureFrames->GetTotalFrames() - 1);
	}

	int SpriteAnimatorComponent::GetTotalFrames() const {
		return m_currentAnimation.textureFrames->GetTotalFrames();
	}

	void SpriteAnimatorComponent::Read(const json::value_t& value)
	{
		SpriteRendererComponent::Read(value);

		JSON_READ_NAME(value, "default_animation", m_defaultAnimationName);

		if (JSON_HAS(value, animations) && JSON_GET(value, animations).IsArray())
		{
			for (auto& animatorValue : JSON_GET(value, animations).GetArray())
			{
				Animation spriteAnimation;

				std::string textureAnimationName;

				JSON_READ_NAME(animatorValue, "name", spriteAnimation.name);

				std::string texture_frames;
				JSON_READ_REQ(animatorValue, texture_frames);

				if (!texture_frames.empty())
				{
					spriteAnimation.textureFrames = Resources().Get<TextureFrames>(texture_frames, Engine::Get().GetRenderer());
					if (!spriteAnimation.textureFrames)
					{
						std::cerr << "Could not load texture frames: " << texture_frames << std::endl;
					}
				}

				JSON_READ_NAME_REQ(animatorValue, "fps", spriteAnimation.fps);
				JSON_READ_NAME(animatorValue, "loop", spriteAnimation.loop);

				m_animations[ToLower(spriteAnimation.name)] = spriteAnimation;
			}
		}

	}
}
