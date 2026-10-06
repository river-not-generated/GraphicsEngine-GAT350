#pragma once
#include "Resources/Resource.h"
#include "Math/Vector2.h"
#include "Math/Rect.h"

#include <string>

namespace nu
{
	class TextureFrames : public Resource {
	public:
		bool Load(const std::string& filename, class Renderer& renderer);
		Rect GetFrameRect(int frame);

		int GetTotalFrames() const { return m_totalFrames; }
		res_t<class Texture> GetTexture() { return m_texture; }

	private:
		int m_numColumns = 0;
		int m_numRows = 0;
		int m_startFrame = 0;
		int m_totalFrames = 0;
		Vector2 m_frameSize{ 0.0f, 0.0f };
		res_t<class Texture> m_texture;
	};
}