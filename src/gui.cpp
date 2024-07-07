#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>

#include "gui.h"

std::shared_ptr<EVK::TextureImage> ImGui_CreateFontsTexture(std::shared_ptr<EVK::Devices> devices){
	ImGuiIO& io = ImGui::GetIO();
	
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	
	if(!ImGui::GetCurrentContext()){
		throw std::runtime_error("No live ImGui context.");
	}
	
	// Store our identifier
	io.Fonts->SetTexID((ImTextureID)1/*bd->FontDescriptorSet*/);
	
	return std::make_shared<EVK::TextureImage>(devices, EVK::DataImageBlueprint{
		.data = pixels,
		.width = uint32_t(width),
		.height = uint32_t(height),
		.pitch = uint32_t(width * 4),
		.format = VK_FORMAT_R8G8B8A8_UNORM
	});
}

std::shared_ptr<EVK::TextureSampler> ImGui_CreateSampler(std::shared_ptr<EVK::Devices> devices){
	VkSamplerCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.magFilter = VK_FILTER_LINEAR;
	info.minFilter = VK_FILTER_LINEAR;
	info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	info.minLod = -1000;
	info.maxLod = 1000;
	info.maxAnisotropy = 1.0f;
	return std::make_shared<EVK::TextureSampler>(devices, info);
}
