#pragma once

#include "Math/Vector2.h"
#include "Math/Vector3.h"

#include <vector>

namespace nu
{
	class Mesh {
		public:
			Mesh(const std::vector<Vector2>& points, const Colour& colour) :
				m_points{ points }, m_colour{ colour } {
			}
			Mesh() = default;

			const std::vector<Vector2>& GetPoints() const {
				return m_points;
			}
			void SetPoints(const std::vector<Vector2>& points) {
				m_points = points;
			}

			const Colour& GetColour() const {
				return m_colour;
			}
			void SetColour(const Colour& colour) {
				this->m_colour = colour;
			}

			float GetRadius() const;

		private:
			std::vector<Vector2> m_points;
			Colour m_colour;
	};
}