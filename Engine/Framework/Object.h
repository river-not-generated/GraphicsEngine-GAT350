#pragma once
#include <string>
#include <memory>
#include "Serialization/Json.h"

#define CLASS_PROTOTYPE(classname) virtual std::unique_ptr<Object> Clone() const { return std::make_unique<classname>(*this); }

namespace nu
{
	struct ObjectDesc {
		std::string name;
	};

	class Object {
	public:
		Object() = default;
		Object(ObjectDesc desc) : m_name{ desc.name } {}
		virtual ~Object() = default;

		CLASS_PROTOTYPE(Object)

		const std::string& GetName() const { return m_name; }
		bool IsActive() const { return m_active; }
		void SetActive(bool active = true) { m_active = active; }

		virtual void Read(const json::value_t& value) {
			JSON_READ_NAME(value, "name", m_name);
			JSON_READ_NAME(value, "active", m_active);
		}

	protected:
		std::string m_name;
		bool m_active = true;
	};
}