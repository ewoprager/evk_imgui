#include "evk_imgui.h"

#include <imgui.h>
#include <mattresses.h>

#include <evk/ShaderProgram.hpp>
#include <evk/Resources.hpp>

namespace EVK_Imgui {

namespace Pipeline {

std::shared_ptr<type> Build(std::shared_ptr<EVK::Devices> devices, VkRenderPass renderPassHandle){
	
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	
	//If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped to them as opposed to discarding them. This is useful in some special cases like shadow maps.
	
	//Using this requires enabling a GPU feature.
	
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	
	//The polygonMode determines how fragments are generated for geometry. The following modes are available:
	
	//VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
	//VK_POLYGON_MODE_LINE: polygon edges are drawn as lines
	//VK_POLYGON_MODE_POINT: polygon vertices are drawn as points
	
	//Using any mode other than fill requires enabling a GPU feature.
	
	rasterizer.lineWidth = 1.0f;
	//The lineWidth member is straightforward, it describes the thickness of lines in terms of number of fragments. The maximum line width that is supported depends on the hardware and any line thicker than 1.0f requires you to enable the wideLines GPU feature.
	
	//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	//The cullMode variable determines the type of face culling to use. You can disable culling, cull the front faces, cull the back faces or both. The frontFace variable specifies the vertex order for faces to be considered front-facing and can be clockwise or counterclockwise.
	
	//rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
	
	
	// ----- Multisampling behaviour -----
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	//#ifdef MSAA
	//multisampling.rasterizationSamples = device.GetMSAASamples();
	//#else
	//multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	//#endif
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional
	// This is how to achieve anti-aliasing.
	// Enabling it requires enabling a GPU feature
	
	
	// ----- Colour blending behaviour -----
	VkPipelineColorBlendStateCreateInfo colourBlending{};
	VkPipelineColorBlendAttachmentState colourBlendAttachment{};
	colourBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colourBlendAttachment.blendEnable = VK_TRUE;
	colourBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colourBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colourBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colourBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colourBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colourBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	colourBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	
	colourBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colourBlending.attachmentCount = 1;
	colourBlending.pAttachments = &colourBlendAttachment;
	
	
	// ----- Depth stencil behaviour -----
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	
	// ----- Dynamic state -----
	VkPipelineDynamicStateCreateInfo dynamicState{};
	VkDynamicState dynamicStates[2] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;
	
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.depthBiasEnable = VK_FALSE;
#ifdef MSAA
	multisampling.rasterizationSamples = devices.GetMSAASamples();
#else
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
#endif
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	
	const EVK::RenderPipelineBlueprint blueprint = {
		.primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.pRasterisationStateCI = &rasterizer,
		.pMultisampleStateCI = &multisampling,
		.pDepthStencilStateCI = &depthStencil,
		.pColourBlendStateCI = &colourBlending,
		.pDynamicStateCI = &dynamicState,
		.renderPassHandle = renderPassHandle
	};
	
	return std::make_shared<type>(devices, &blueprint);
}

} // namespace Pipeline

std::shared_ptr<EVK::TextureImage> CreateFontsTexture(std::shared_ptr<EVK::Devices> devices){
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

std::shared_ptr<EVK::TextureSampler> CreateSampler(std::shared_ptr<EVK::Devices> devices){
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

bool RenderDrawData(std::shared_ptr<EVK::Devices> devices, ImDrawData* draw_data, const EVK::CommandEnvironment &ci, std::shared_ptr<Pipeline::type> pipeline){
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	//	int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	//	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	//	if (fb_width <= 0 || fb_height <= 0)
	//		return;
	
	//	ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
	//	ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
	//	if (pipeline == VK_NULL_HANDLE)
	//		pipeline = bd->Pipeline;
	
	// Allocate array to store enough vertex/index buffers
	//	ImGui_ImplVulkan_WindowRenderBuffers* wrb = &bd->MainWindowRenderBuffers;
	//	if (wrb->FrameRenderBuffers == nullptr)
	//	{
	//		wrb->Index = 0;
	//		wrb->Count = v->ImageCount;
	//		wrb->FrameRenderBuffers = (ImGui_ImplVulkan_FrameRenderBuffers*)IM_ALLOC(sizeof(ImGui_ImplVulkan_FrameRenderBuffers) * wrb->Count);
	//		memset(wrb->FrameRenderBuffers, 0, sizeof(ImGui_ImplVulkan_FrameRenderBuffers) * wrb->Count);
	//	}
	//	IM_ASSERT(wrb->Count == v->ImageCount);
	//	wrb->Index = (wrb->Index + 1) % wrb->Count;
	//	ImGui_ImplVulkan_FrameRenderBuffers* rb = &wrb->FrameRenderBuffers[wrb->Index];
	
	static std::shared_ptr<EVK::VertexBufferObject> vbo = std::make_shared<EVK::VertexBufferObject>(devices);
	static std::shared_ptr<EVK::IndexBufferObject> ibo = std::make_shared<EVK::IndexBufferObject>(devices);
	
	if (draw_data->TotalVtxCount > 0)	{
		//		// Create or resize the vertex/index buffers
		//		size_t vertex_size = AlignBufferSize(draw_data->TotalVtxCount * sizeof(ImDrawVert), bd->BufferMemoryAlignment);
		//		size_t index_size = AlignBufferSize(draw_data->TotalIdxCount * sizeof(ImDrawIdx), bd->BufferMemoryAlignment);
		//		if (rb->VertexBuffer == VK_NULL_HANDLE || rb->VertexBufferSize < vertex_size)
		//			CreateOrResizeBuffer(rb->VertexBuffer, rb->VertexBufferMemory, rb->VertexBufferSize, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		//		if (rb->IndexBuffer == VK_NULL_HANDLE || rb->IndexBufferSize < index_size)
		//			CreateOrResizeBuffer(rb->IndexBuffer, rb->IndexBufferMemory, rb->IndexBufferSize, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		
		// Upload vertex/index data into a single contiguous GPU buffer
		//		ImDrawVert* vtx_dst = nullptr;
		//		ImDrawIdx* idx_dst = nullptr;
		//		VkResult err = vkMapMemory(v->Device, rb->VertexBufferMemory, 0, vertex_size, 0, (void**)&vtx_dst);
		//		check_vk_result(err);
		//		err = vkMapMemory(v->Device, rb->IndexBufferMemory, 0, index_size, 0, (void**)&idx_dst);
		//		check_vk_result(err);
		//		for (int n = 0; n < draw_data->CmdListsCount; n++)		{
		//			const ImDrawList* cmd_list = draw_data->CmdLists[n];
		//			memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		//			memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		//			vtx_dst += cmd_list->VtxBuffer.Size;
		//			idx_dst += cmd_list->IdxBuffer.Size;
		//		}
		//		VkMappedMemoryRange range[2] = {};
		//		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		//		range[0].memory = rb->VertexBufferMemory;
		//		range[0].size = VK_WHOLE_SIZE;
		//		range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		//		range[1].memory = rb->IndexBufferMemory;
		//		range[1].size = VK_WHOLE_SIZE;
		//		err = vkFlushMappedMemoryRanges(v->Device, 2, range);
		//		check_vk_result(err);
		//		vkUnmapMemory(v->Device, rb->VertexBufferMemory);
		//		vkUnmapMemory(v->Device, rb->IndexBufferMemory);
		
		std::vector<EVK::Devices::DeviceMemory> vtx_dm(draw_data->CmdListsCount);
		std::vector<EVK::Devices::DeviceMemory> idx_dm(draw_data->CmdListsCount);
		for(int n = 0; n < draw_data->CmdListsCount; n++){
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			vtx_dm[n] = {cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert)};
			idx_dm[n] = {cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx)};
		}
		vbo->Fill(vtx_dm);
		ibo->Fill(sizeof(ImDrawIdx), idx_dm);
	}
	
	// Setup desired Vulkan state
	//	ImGui_ImplVulkan_SetupRenderState(draw_data, pipeline, ci.commandBuffer, rb, fb_width, fb_height);
	pipeline->CmdBind(ci.commandBuffer);
	
	if(!vbo->CmdBind(ci.commandBuffer, 0)){
		return false;
	}
	if(!ibo->CmdBind(ci.commandBuffer, sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32)){
		return false;
	}
	
	{
		Pipeline::VertexShader::PushConstantType pcs;
		pcs.uScale.x = 2.0f / draw_data->DisplaySize.x;
		pcs.uScale.y  = 2.0f / draw_data->DisplaySize.y;
		pcs.uTranslate.x = -1.0f - draw_data->DisplayPos.x * pcs.uScale.x;
		pcs.uTranslate.y = -1.0f - draw_data->DisplayPos.y * pcs.uScale.y;
		pipeline->CmdPushConstants<0>(ci.commandBuffer, &pcs);
	}
	
	// Will project scissor/clipping rectangles into framebuffer space
	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
	
	// Render command lists
	// (Because we merged all buffers into a single one, we maintain our own offset into them)
	int global_vtx_offset = 0;
	int global_idx_offset = 0;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback != nullptr)
			{
				// User callback, registered via ImDrawList::AddCallback()
				// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
					//					ImGui_ImplVulkan_SetupRenderState(draw_data, pipeline, ci.commandBuffer, rb, fb_width, fb_height);
					throw std::runtime_error("Hullo there");
				else
					pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				// Project scissor/clipping rectangles into framebuffer space
				ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
				ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
				
				// Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
				//				if (clip_min.x < 0.0f) { clip_min.x = 0.0f; }
				//				if (clip_min.y < 0.0f) { clip_min.y = 0.0f; }
				//				if (clip_max.x > fb_width) { clip_max.x = (float)fb_width; }
				//				if (clip_max.y > fb_height) { clip_max.y = (float)fb_height; }
				//				if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
				//					continue;
				
				// Apply scissor/clipping rectangle
				//				VkRect2D scissor;
				//				scissor.offset.x = (int32_t)(clip_min.x);
				//				scissor.offset.y = (int32_t)(clip_min.y);
				//				scissor.extent.width = (uint32_t)(clip_max.x - clip_min.x);
				//				scissor.extent.height = (uint32_t)(clip_max.y - clip_min.y);
				//				vkCmdSetScissor(ci.commandBuffer, 0, 1, &scissor);
				
				// Bind DescriptorSet with font or user texture
				//				VkDescriptorSet desc_set[1] = { (VkDescriptorSet)pcmd->TextureId };
				//				if (sizeof(ImTextureID) < sizeof(ImU64))
				//				{
				//					// We don't support texture switches if ImTextureID hasn't been redefined to be 64-bit. Do a flaky check that other textures haven't been used.
				//					IM_ASSERT(pcmd->TextureId == (ImTextureID)bd->FontDescriptorSet);
				//					desc_set[0] = bd->FontDescriptorSet;
				//				}
				void(pipeline->CmdBindDescriptorSets(ci));
				//				vkCmdBindDescriptorSets(ci.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, bd->PipelineLayout, 0, 1, desc_set, 0, nullptr);
				
				// Draw
				vkCmdDrawIndexed(ci.commandBuffer, pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
			}
		}
		global_idx_offset += cmd_list->IdxBuffer.Size;
		global_vtx_offset += cmd_list->VtxBuffer.Size;
	}
	
	// Note: at this point both vkCmdSetViewport() and vkCmdSetScissor() have been called.
	// Our last values will leak into user/application rendering IF:
	// - Your app uses a pipeline with VK_DYNAMIC_STATE_VIEWPORT or VK_DYNAMIC_STATE_SCISSOR dynamic state
	// - And you forgot to call vkCmdSetViewport() and vkCmdSetScissor() yourself to explicitly set that state.
	// If you use VK_DYNAMIC_STATE_VIEWPORT or VK_DYNAMIC_STATE_SCISSOR you are responsible for setting the values before rendering.
	// In theory we should aim to backup/restore those values but I am not sure this is possible.
	// We perform a call to vkCmdSetScissor() to set back a full viewport which is likely to fix things for 99% users but technically this is not perfect. (See github #4644)
	//	VkRect2D scissor = { { 0, 0 }, { (uint32_t)fb_width, (uint32_t)fb_height } };
	//	vkCmdSetScissor(ci.commandBuffer, 0, 1, &scissor);
	return true;
}

Renderer::Renderer(std::shared_ptr<EVK::Devices> _devices, VkRenderPass renderPassHandle) : devices(_devices) {
	pipeline = Pipeline::Build(devices, renderPassHandle);
	pipeline->iDescriptorSet<0>().iDescriptor<0>().Set({{{CreateFontsTexture(devices), CreateSampler(devices)}}});
}

void Renderer::Render(ImDrawData *drawData, const EVK::CommandEnvironment &ci){
	RenderDrawData(devices, drawData, ci, pipeline);
}

} // namespace EVK_Imgui
