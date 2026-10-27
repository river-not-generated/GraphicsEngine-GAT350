#include "Engine.h"

#include <iostream>
#include <vector>
#include <random>
#include <vector>
#include <map>
#include <memory>
#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Pipeline.h"

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

struct LightUniform {
	glm::vec3 position;
	LightType type;
	glm::vec3 direction;
	float pad1;
	glm::vec3 color;
	float intensity;
	glm::vec3 ambient;
	float pad2;
};

glm::float4 colour{ 0,0,0,1 };

struct MaterialUniform {
	glm::vec2 tiling;
	glm::vec2 offset;
};

MaterialUniform materialUniform{
	.tiling{1.0f, 1.0f},
	.offset{0.0f, 0.0f}
};

LightUniform lightUniform{
	.position{10, 10, -10},
	.type{LightType::DIRECTIONAL},
	.direction{0,-1,0},
	.color{1.0f, 0.9f, 0.7f},
	.intensity{1.0f},
	.ambient{0.2f}
};

std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

int main()
{   
	SetWorkingDirectory("assets");

	// initialize the engine
	if (Engine::Get().Initialize(WIN_WIDTH, WIN_HEIGHT) == false) return 0;

	auto model = Resources().Get<Model>("Models/spot.obj", Engine::Get().GetRenderer());
	auto texture = Resources().Get<Texture>("Textures/spot_diffuse.png", Engine::Get().GetRenderer());

	auto vshader = Resources().Get<nu::Shader>("shaders/goraud.vert", Engine::Get().GetRenderer(), 3);
	auto fshader = Resources().Get<nu::Shader>("shaders/goraud.frag", Engine::Get().GetRenderer(), 1, 1);

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
	vertexUniform.cameraPosition = { 0.0f, 0.5f, 2.0f };
	float rotation = 0.0f;

	Transform transform;

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
			lightUniform.type = (lightUniform.type == LightType::DIRECTIONAL ? LightType::POINT : LightType::DIRECTIONAL);

		vertexUniform.cameraPosition += direction * 4.0f * dt;
		//materialUniform.tiling = glm::vec2(sin(time) + 1, cos(time) + 1);

		transform.rotation.y = Engine::Get().GetTime().GetTime() * 30.0f;

		auto view = glm::lookAtRH(vertexUniform.cameraPosition, 
			vertexUniform.cameraPosition + glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });

		float aspectRatio = Engine::Get().GetRenderer().GetWindowWidth() / Engine::Get().GetRenderer().GetWindowHeight();
		auto projection = glm::perspective(glm::radians(100.0f), aspectRatio, 0.01f, 20.0f);

		vertexUniform.model = transform.GetMatrix();
		vertexUniform.mvp = projection * view * vertexUniform.model;

		// cout << materialUniform.tiling.x << ", " << materialUniform.tiling.y << endl;

		// == RENDERING ===

        Engine::Get().GetRenderer().BeginFrame();
        Engine::Get().GetRenderer().SetPipeline(*pipeline);

		Engine::Get().GetRenderer().SetVertexUniform(0, &vertexUniform, sizeof(vertexUniform));
		Engine::Get().GetRenderer().SetVertexUniform(1, &lightUniform, sizeof(lightUniform));
		Engine::Get().GetRenderer().SetVertexUniform(2, &materialUniform, sizeof(materialUniform));
		Engine::Get().GetRenderer().SetFragmentUniform(0, &colour, sizeof(colour));
		Engine::Get().GetRenderer().SetFragmentTexture(0, *texture);

		model->Draw(Engine::Get().GetRenderer());
        
        Engine::Get().GetRenderer().EndFrame();
	}

	// shut down the program cleanly upon exiting
	Engine::Get().Shutdown();

	return 0;

}
