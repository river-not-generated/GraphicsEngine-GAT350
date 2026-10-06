#include "pch.h"
#include "TextureFrames.h"
#include "Resources/ResourceManager.h"
#include "Texture.h"

#include "Serialization/Json.h"

namespace nu
{
	bool TextureFrames::Load(const std::string& filename, class Renderer& renderer) {
		json::document_t document;

		if (!json::Load(filename, document)) {
			std::cerr << "Could not load TextureFrames: " << filename << std::endl;
			return false;
		}

		std::string texture_name;
		JSON_READ_REQ(document, texture_name);
		if (texture_name.empty()) return false;

		m_texture = Resources().Get<Texture>(texture_name, renderer);
		if (!m_texture) { 
			std::cerr << "Could not load TextureFrames texture: " << texture_name << std::endl;
			return false;
		}

		JSON_READ_NAME(document, "columns", m_numColumns);
		JSON_READ_NAME(document, "rows", m_numRows);
		JSON_READ_NAME(document, "start_frame", m_startFrame);
		JSON_READ_NAME(document, "total_frames", m_totalFrames);

		if (m_numColumns <= 0 || m_numRows <= 0) {
			std::cerr << "Could not load TextureFrames, rows or columns are less than or equal to 0" << std::endl;
			return false;
		}

		Vector2 textureSize = m_texture->GetSize();

		m_frameSize = textureSize / Vector2{ (float) m_numColumns, (float) m_numRows };

		return true;
	}

	Rect TextureFrames::GetFrameRect(int frame) {
		if (frame < 0 || frame >= m_totalFrames) {
			std::cerr << "Referenced frame out of bounds" << std::endl;
			frame = 0;
		}

		int currentFrame = m_startFrame + frame;
		int column = currentFrame % m_numColumns;
		int row = currentFrame / m_numColumns;
		
		return Rect{
			 column * m_frameSize.x,
			 row * m_frameSize.y,
			 m_frameSize.x,
			 m_frameSize.y
		};
	}
}