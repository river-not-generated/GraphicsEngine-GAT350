#include "pch.h"
#include "VertexBuffer.h"
#include "Renderer.h"

namespace nu
{
	VertexBuffer::~VertexBuffer()
	{
		if (m_gpuBuffer && m_gpuDevice) {
			SDL_ReleaseGPUBuffer(m_gpuDevice, m_gpuBuffer);
		}
	}

	bool VertexBuffer::Create(uint32_t vertexCount, uint32_t vertexSize, const uint8_t* data, const Renderer& renderer)
	{
		m_vertexCount = vertexCount;
		m_gpuDevice = renderer.m_gpuDevice;

		uint32_t verticesSize = m_vertexCount * vertexSize;

		// --- create the vertex buffer ---
		// this buffer lives in gpu memory, the cpu can't write to it directly

		SDL_GPUBufferCreateInfo vertexBufferCreateInfo{
			.usage = {SDL_GPU_BUFFERUSAGE_VERTEX},
			.size = verticesSize
		};

		m_gpuBuffer = SDL_CreateGPUBuffer(m_gpuDevice, &vertexBufferCreateInfo);

		if (m_gpuBuffer == nullptr)
		{
			std::cerr << "Could not create vertex buffer: " << SDL_GetError() << std::endl;

			return false;
		}

		// --- create the transfer buffer ---
		// a transfer buffer is a staging area the cpu can write to, it is used to copy data into the vertex buffer

		SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo{
			.usage = {SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD}, // todo: this buffer uploads data from the cpu to the gpu (SDL_GPU_TRANSFERBUFFERUSAGE_...)
			.size = {verticesSize} // todo: the total size in bytes of the vertices
		};

		// create the transfer buffer with SDL_CreateGPUTransferBuffer(device, &createInfo)
		SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_gpuDevice, &transferBufferCreateInfo);
		if (transferBuffer == nullptr)
		{
			std::cerr << "Could not create transfer buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUBuffer(m_gpuDevice, m_gpuBuffer);
			m_gpuBuffer = nullptr;

			return false;
		}

		// --- copy the vertex data into the transfer buffer ---


		uint8_t* transferData = static_cast<uint8_t*>(SDL_MapGPUTransferBuffer(m_gpuDevice, transferBuffer, false));
		if (transferData == nullptr)
		{
			std::cerr << "Could not map transfer buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);
			SDL_ReleaseGPUBuffer(m_gpuDevice, m_gpuBuffer);
			m_gpuBuffer = nullptr;

			return false;
		}

		memcpy(transferData, data, verticesSize);

		SDL_UnmapGPUTransferBuffer(m_gpuDevice, transferBuffer);

		// --- upload the transfer buffer to the vertex buffer ---
		// the gpu does the copy, so it has to be recorded in a command buffer and submitted

		SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(m_gpuDevice);
		if (commandBuffer == nullptr)
		{
			std::cerr << "Could not acquire GPU command buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);
			SDL_ReleaseGPUBuffer(m_gpuDevice, m_gpuBuffer);
			m_gpuBuffer = nullptr;

			return false;
		}


		SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);

		SDL_GPUTransferBufferLocation bufferLocation{
			.transfer_buffer = {transferBuffer},
			.offset = 0
		};

		SDL_GPUBufferRegion bufferRegion{
			.buffer = {m_gpuBuffer}, 
			.offset = 0,
			.size = verticesSize
		};

		SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, false);

		// end the copy pass with SDL_EndGPUCopyPass(copyPass)
		SDL_EndGPUCopyPass(copyPass);

		// submit the command buffer so the gpu runs the copy
		if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
		{
			std::cerr << "Could not submit GPU command buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);
			SDL_ReleaseGPUBuffer(m_gpuDevice, m_gpuBuffer);
			m_gpuBuffer = nullptr;

			return false;
		}

		// the transfer buffer is no longer needed,
		// release it with SDL_ReleaseGPUTransferBuffer(device, transferBuffer)
		SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);

		// vertex buffer created and uploaded successfully
		return true;
	}
}