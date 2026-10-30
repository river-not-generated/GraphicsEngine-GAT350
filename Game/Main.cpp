#include "Engine.h"

#include <iostream>
#include <vector>
#include <random>
#include <map>
#include <memory>
#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Pipeline.h"

#define CHECK_UNIFORM_BOUNDARY(T) static_assert((sizeof(T) % 16) == 0);
#define MAX_LIGHTS 8

using namespace nu;
using namespace std;

const float WIN_WIDTH = 1240.0f;
const float WIN_HEIGHT = 800.0f;

enum class LightType {
	DIRECTIONAL,
	POINT,
	SPOT
};

struct VertexUniform {
	glm::mat4 mvp;
	glm::mat4 model;
	glm::vec3 cameraPosition;
	float pad1;
};

struct Light {
	glm::vec3 position;
	LightType type;
	glm::vec3 direction;
	float range;
	glm::vec3 color;
	float intensity;
	glm::vec3 ambient;
	float innerCutoff;
	float outerCutoff;
	glm::vec3 pad1;

};

struct LightUniform {
	Light lights[MAX_LIGHTS];
	uint32_t numLights;
	glm::vec3 ambient;
};

glm::float4 colour{ 0,0,0,1 };

struct MaterialUniform {
	glm::vec4 color;
	glm::vec2 tiling;
	glm::vec2 offset;
	float shininess;
	glm::vec3 pad1;
};

MaterialUniform materialUniform{
	.color{1, 1, 1, 1},
	.tiling{1.0f, 1.0f},
	.offset{0.0f, 0.0f},
	.shininess{32}
};


Light light1{
	.position{10, 2, 0},
	.type{LightType::POINT},
	.direction{0, -1, 0},
	.range{80},
	.color{1.0f, 0.9f, 0.7f},
	.intensity{0.9f},
	.ambient{0.2f},
	.innerCutoff{0.2f},
	.outerCutoff{0.4f},
};


Light light2{
	.position{0, 2, 0},
	.type{LightType::POINT},
	.direction{0, -1, 0},
	.range{80},
	.color{1.0f, 0.0f, 0.0f},
	.intensity{0.9f},
	.ambient{0.2f},
	.innerCutoff{0.2f},
	.outerCutoff{0.4f},
};


// size in bytes needs to be divisible by 16
// padding is just filler data
CHECK_UNIFORM_BOUNDARY(LightUniform)
CHECK_UNIFORM_BOUNDARY(VertexUniform)
CHECK_UNIFORM_BOUNDARY(MaterialUniform)

int main()
{   
	SetWorkingDirectory("assets");

	// initialize the engine
	if (Engine::Get().Initialize(WIN_WIDTH, WIN_HEIGHT) == false) return 0;

	auto model = Resources().Get<Model>("Models/sphere.obj", Engine::Get().GetRenderer());
	auto texture = Resources().Get<Texture>("Textures/brick_diffuse.png", Engine::Get().GetRenderer());

	auto vshader = Resources().Get<nu::Shader>("shaders/phong.vert", Engine::Get().GetRenderer(), 3);
	auto fshader = Resources().Get<nu::Shader>("shaders/phong.frag", Engine::Get().GetRenderer(), 3, 1);

	auto pipeline = std::make_shared<Pipeline>();

	pipeline->AddVertexBuffer(sizeof(Model::Vertex));
	pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
	pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
	pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
	pipeline->AddVertexAttribute(3, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, tangent));

	pipeline->Create(*vshader.get(), *fshader.get(),
		Engine::Get().GetRenderer().GetGPUDevice(),
		Engine::Get().GetRenderer().GetWindow());

	VertexUniform vertexUniform;
	vertexUniform.cameraPosition = { 0.0f, 2.0f, 8.0f };

	Transform transform;
	transform.scale = glm::vec3{ 4.0f };

	LightUniform lightUniform;

	lightUniform.lights[0] = light1;
	lightUniform.lights[1] = light2;
	lightUniform.numLights = 2;
	lightUniform.ambient = glm::vec3{ 0.2f };

	bool quit = false;
	while (!quit) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
			}
			if (e.type == SDL_EVENT_KEY_DOWN && e.key.scancode == SDL_SCANCODE_ESCAPE) {
				quit = true;
			}
		}

		Engine::Get().Update();

		float dt = Engine::Get().GetTime().GetDeltaTime();
		float time = Engine::Get().GetTime().GetTime();

		glm::vec3 direction{ 0 };
		// input
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)) direction.x = -1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D)) direction.x = 1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W)) direction.z = -1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_S)) direction.z = 1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_Q)) direction.y = -1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_E)) direction.y = 1;

		if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_1)) 
			light1.type = LightType::DIRECTIONAL;
		if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_2))
			light1.type = LightType::POINT;
		if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_3)) {
			light1.type = LightType::SPOT;
			light1.innerCutoff = glm::cos(glm::radians(10.0f));
			light1.outerCutoff = glm::cos(glm::radians(40.0f));
		}

		vertexUniform.cameraPosition += direction * 4.0f * dt;

		lightUniform.lights[1].position = glm::eulerAngleY(time * 4) * glm::vec4{ 0, 0, 10, 1 };

		auto view = glm::lookAtRH(vertexUniform.cameraPosition, 
			vertexUniform.cameraPosition + glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });

		float aspectRatio = Engine::Get().GetRenderer().GetWindowWidth() / Engine::Get().GetRenderer().GetWindowHeight();
		// FOV
		auto projection = glm::perspective(glm::radians(80.0f), aspectRatio, 0.01f, 20.0f);

		vertexUniform.model = transform.GetMatrix();
		vertexUniform.mvp = projection * view * vertexUniform.model;

		// == RENDERING ===

        Engine::Get().GetRenderer().BeginFrame();
        Engine::Get().GetRenderer().SetPipeline(*pipeline);

		Engine::Get().GetRenderer().SetVertexUniform(0, &vertexUniform, sizeof(vertexUniform));
		Engine::Get().GetRenderer().SetVertexUniform(1, &lightUniform, sizeof(lightUniform));
		Engine::Get().GetRenderer().SetVertexUniform(2, &materialUniform, sizeof(materialUniform));

		Engine::Get().GetRenderer().SetFragmentUniform(0, &vertexUniform, sizeof(vertexUniform));
		Engine::Get().GetRenderer().SetFragmentUniform(1, &lightUniform, sizeof(lightUniform));
		Engine::Get().GetRenderer().SetFragmentUniform(2, &materialUniform, sizeof(materialUniform));
		Engine::Get().GetRenderer().SetFragmentTexture(0, *texture);

		model->Draw(Engine::Get().GetRenderer());
        
        Engine::Get().GetRenderer().EndFrame();
	}

	// shut down the program cleanly upon exiting
	Engine::Get().Shutdown();

	return 0;

}
