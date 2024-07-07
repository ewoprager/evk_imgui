#pragma once

#include <imgui.h>
#include <evk/ShaderProgram.hpp>

namespace EVK_Imgui {

namespace Pipeline {

namespace VertexShader {

static constexpr char vertexFilename[] = "../../imgui/backends/vulkan/glsl_shader.vert.u32";

struct PushConstantType {
	vec<2> uScale;
	vec<2> uTranslate;
};
using PCS = EVK::PushConstants<0, PushConstantType>;
static_assert(EVK::pushConstants_c<PCS>);

using Attributes = EVK::Attributes<EVK::BindingDescriptionPack<
VkVertexInputBindingDescription{
	0, // binding
	20, // stride
	VK_VERTEX_INPUT_RATE_VERTEX // input rate
}
>, EVK::AttributeDescriptionPack<
VkVertexInputAttributeDescription{
	0, 0, VK_FORMAT_R32G32_SFLOAT, 0
},
VkVertexInputAttributeDescription{
	1, 0, VK_FORMAT_R32G32_SFLOAT, 8
},
VkVertexInputAttributeDescription{
	2, 0, VK_FORMAT_R8G8B8A8_UNORM, 16
}
>>;

using type = EVK::VertexShader<vertexFilename, PCS, Attributes>;

static_assert(EVK::vertexShader_c<type>);

} // namespace VertexShader

namespace FragmentShader {

static constexpr char fragmentFilename[] = "../../imgui/backends/vulkan/glsl_shader.frag.u32";

using type = EVK::Shader<VK_SHADER_STAGE_FRAGMENT_BIT, fragmentFilename, EVK::NoPushConstants,
EVK::CombinedImageSamplersUniform<0, 0>
>;
static_assert(EVK::shader_c<type>);

} // namespace FragmentShader

using type = EVK::RenderPipeline<VertexShader::type, FragmentShader::type>;

} // namespace Pipeline

class Renderer {
public:
	Renderer(std::shared_ptr<EVK::Devices> _devices, VkRenderPass renderPassHandle);
	
	void Render(ImDrawData *drawData, const EVK::CommandEnvironment &ci);
	
private:
	std::shared_ptr<EVK::Devices> devices;
	std::shared_ptr<Pipeline::type> pipeline;
};

} // namespace EVK_Imgui
