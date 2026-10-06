#pragma once
#include <box2d/box2d.h>
#include <memory>
#include "Math/Vector2.h"

namespace nu
{
	class Physics
	{
	public:
		Physics() = default;

		bool Initialize();
		void Shutdown();

		static Vector2 PixelToWorld(const Vector2& pixel) {
			return pixel / m_pixelsPerUnit;
		}
		static Vector2 WorldToPixel(const Vector2& world) {
			return world * m_pixelsPerUnit;
		}

		static b2Vec2 ToB2(const Vector2& v) { return b2Vec2{v.x, v.y}; }
		static Vector2 ToVector2(const b2Vec2& b2) { return Vector2{ b2.x, b2.y }; }

		void Update(float dt);

		friend class PhysicsBody;

	public:
		static float m_pixelsPerUnit;

	private:
		void ProcessCollisionEvents();

	private:
		b2WorldId m_worldId;
	};
}
