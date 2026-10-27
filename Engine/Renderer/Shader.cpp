#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Core/File.h"

namespace nu
{
	Shader::~Shader()
	{
		if (m_gpuDevice && m_gpuShader) {
			SDL_ReleaseGPUShader(m_gpuDevice, m_gpuShader);
		}
	}


	bool Shader::Load(const std::string& filename, Renderer& renderer, uint32_t uniformCount, uint32_t samplerCount)
	{
		m_gpuDevice = renderer.GetGPUDevice();

		SDL_GPUShaderStage stage{};
		if (filename.find(".vert") != std::string::npos)
		{
			stage = SDL_GPU_SHADERSTAGE_VERTEX;
		}
		else if (filename.find(".frag") != std::string::npos)
		{
			stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
		}
		else
		{
			std::cerr << "Shader filename must contain either .vert or .frag" << std::endl;
			return false;
		}


		SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
		std::string shaderFilename = filename;

		const char* entrypoint = "main";


		SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(m_gpuDevice);

		if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL)
		{
			shaderFilename += ".dxil";
			format = SDL_GPU_SHADERFORMAT_DXIL;
		}
		else if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV)
		{
			shaderFilename += ".spv";
			format = SDL_GPU_SHADERFORMAT_SPIRV;
		}
		else
		{
			std::cerr << "No supported shader format found" << std::endl;
			return false;
		}


		std::vector<uint8_t> bytes;
		bytes = ReadBinaryFile(shaderFilename);
		if (bytes.empty()) {
			std::cerr << "Shader file was empty: " << shaderFilename << std::endl;
			return false;
		}

		
		SDL_GPUShaderCreateInfo shaderInfo{
			.code_size = {bytes.size()},
			.code = {bytes.data()},
			.entrypoint = {entrypoint},
			.format = {format},
			.stage = {stage},
			.num_samplers = samplerCount,
			.num_uniform_buffers = uniformCount
		};

		m_gpuShader = SDL_CreateGPUShader(m_gpuDevice, &shaderInfo);

		if (!m_gpuShader) {
			std::cerr << SDL_GetError() << std::endl;
			return false;
		}
		
		std::cout << "Shader loaded successfully: " << shaderFilename << std::endl;
		return true;
	}
}