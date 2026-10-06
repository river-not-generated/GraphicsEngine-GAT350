#pragma once
#include "SpriteAnimRendererComponent.h"
#include "Resources/ResourceManager.h"
#include <map>
#include <string>

namespace nu
{
	class SpriteAnimatorComponent : public SpriteRendererComponent
	{
	public:
		struct Animation {
			std::string name;
			float fps = 10.0f;
			bool loop = true;
			res_t<class TextureFrames> textureFrames;
		};

	public:
		CLASS_PROTOTYPE(SpriteAnimatorComponent)

		void Update(float dt) override;
		void OnStart() override;

		void Play(const std::string& name);
		bool IsPaused() { return m_paused; }
		void SetPaused(bool paused = true) { m_paused = paused; }
		void Pause() { m_paused = true; }

		int GetFrame() const { return m_frame; }
		bool IsAnimationDone() const;
		const std::string& GetAnimationName() const { return m_currentAnimation.name; }
		int GetTotalFrames() const;

		void Read(const json::value_t& value) override;

	private:
		int m_frame = 0;
		float m_frameTimer = 0.0f;
		bool m_paused = false;

		std::string m_defaultAnimationName;

		Animation m_currentAnimation;
		std::map<std::string, Animation> m_animations;
	};
}