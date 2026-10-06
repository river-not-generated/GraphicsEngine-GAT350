#include "pch.h"
#include "Actor.h"
#include "Renderer/Renderer.h"
#include "Math/MathUtils.h"
#include "Engine.h"
#include "Renderer/Texture.h"
#include "Core/Factory.h"
#include "Components/RendererComponent.h"

namespace nu
{
    FACTORY_REGISTER(Actor)

       Actor::Actor(const Actor& other) : Object{ other }
        , m_tag{ other.m_tag }
        , m_transform{ other.m_transform }
        , m_lifespan{ other.m_lifespan } 
    {
        for (const auto& component : other.m_components) {
            auto clone = std::unique_ptr<Component>(dynamic_cast<Component*>(component->Clone().release()));
            AddComponent(std::move(clone));
        }
    }

    void Actor::OnStart()  {
        for (auto& c : m_components) {
            c->OnStart();
        }
    }

    void Actor::Update(float dt) {
        // lifespan
        if (m_lifespan > 0.0f && !m_destroyed) {
            m_lifespan -= dt;
            m_destroyed = m_lifespan <= 0;
        }

        for (auto& c : m_components) {
            if (c->IsActive()) c->Update(dt);
        }
    }

    void Actor::Draw(const Renderer& renderer) const {
        for (auto& c : m_components) {
            // check if component is a renderer component (can be drawn)
            auto rc = dynamic_cast<RendererComponent*>(c.get());
            if (rc && rc->IsActive()) rc->Draw(renderer);
        }
    }

    void Actor::OnDestroy() const {
        for (auto& c : m_components) {
            c->OnDestroy();
        }
    }

    
    float Actor::GetRadius(float error) const {

        return 0.0f;
    }

    void Actor::Read(const json::value_t& value) {
        Object::Read(value);

        if (JSON_HAS_NAME(value, "transform")) {
            m_transform.Read(JSON_GET_NAME(value, "transform"));
        }
        JSON_READ_NAME(value, "tag", m_tag);
        JSON_READ_NAME(value, "life", m_lifespan);
        JSON_READ_NAME(value, "persistent", m_persistent);

        // read actor components
        if (JSON_HAS_NAME(value, "components")) {
            // iterate through actor components
            for (auto& compValue : JSON_GET_NAME(value, "components").GetArray()) {
                // get component type
                std::string typeName;
                JSON_READ_NAME(compValue, "type", typeName);

                std::cout << "Loading component: " << typeName << std::endl;

                // create component of type
                auto component = Factory::Instance().Create <Component>(typeName);

                if (component) {
                    component->Read(compValue);
                    AddComponent(std::move(component));
                }
            }
        }
    }
    void Actor::AddComponent(std::unique_ptr<Component> component)
    {
        component->SetOwner(this);
        m_components.push_back(std::move(component));
    }


}