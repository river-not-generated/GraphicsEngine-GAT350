#pragma once

#include "Math/Vector2.h"

#include <vector>

namespace nu
{
	class Input {
		public:
			enum class MouseButton {
				Left = 1,
				Middle = 2,
				Right = 3,
				X1 = 4,
				X2 = 5
			};

		public:
			bool Initialize();
			void Shutdown();

			void Update();

			bool GetKeyDown(int key) const { return m_keyStates[key]; }
			bool GetPrevKeyDown(int key) const { return m_prevKeyStates[key]; }
			// pressed: previous frame it was not pressed but now it is
			bool GetKeyPressed(int key) const { return m_keyStates[key] && !m_prevKeyStates[key]; }
			// released: previous frame it was pressed but now it's not
			bool GetKeyReleased(int key) const { return !m_keyStates[key] && m_prevKeyStates[key]; }

			// bitwise and
			bool GetMouseDown(MouseButton button) const { return m_buttonStates & GetButtonBit(button); }
			bool GetPrevMouseDown(MouseButton button) const { return m_prevButtonStates & GetButtonBit(button); }
			// logical and
			bool GetMousePressed(MouseButton button) const { return !GetPrevMouseDown(button) && GetMouseDown(button); }
			bool GetMouseReleased(MouseButton button) const { return GetPrevMouseDown(button) && !GetMouseDown(button); }

			Vector2 GetMousePosition() const { return m_mousePosition; }

		private:
			// keyboard
			std::vector<bool> m_keyStates;
			std::vector<bool> m_prevKeyStates;

			// mouse
			uint32_t m_buttonStates = 0;
			uint32_t m_prevButtonStates = 0;

			Vector2 m_mousePosition{ 0, 0 };

			uint32_t GetButtonBit(MouseButton button) const;
	};
}