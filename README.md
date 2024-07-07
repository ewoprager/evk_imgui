# evk_imgui

A simple implementation of the ImGui render pipeline using evk.

## Usage

*Note: As a result of a cmake skill issue, this library requires that the ImGui spirv shaders are at the locations specified in the string literals found in evk_imgui.h. This will be the case automatically if your project has the ImGui repo one directory out of the cmake build directory.*

- Add the ImGui repository to your project, either by copying it in or adding it as a git submodule.
- Configure your build to include the `.h` and `.cpp` files in `imgui/`, and whatever backend implementation `.cpp` you need for your window display (e.g. `imgui/backends/imgui_impl_sdl.cpp`).
- You only need to include in your code `evk_imgui/evk_imgui.h` and whatever ImGui implementation you need for your window display (e.g. `imgui/backends/imgui_impl_sdl2.h`).

To initialise:

- After creating each of `EVK::Devices` and `EVK::Interface`, and calling the appropriate ImGui 'Init' function according to your window display, create an instance of `EVK_ImGui::Renderer`.
- E.g.:
```c++
std::shared_ptr<EVK::Devices> devices = std::make_shared<EVK::Devices>(...);
...
std::shared_ptr<EVK::Interface> interface = std::make_shared<EVK::Interface>(devices);
...
ImGui_ImplSDL2_InitForVulkan(window);
...
EVK_ImGui::Renderer guiRenderer {devices, interface->GetRenderPassHandle()};
```

In your render loop:

- Begin with the appropriate ImGui 'NewFrame' function according to your window display.
- Define your gui layout as normal with `ImGui::NewFrame()` and `ImGui::Render()`.
- At the point where you are recording your command buffer for your swap chain render pass, get the draw data from ImGui and call `Render(...)` on your renderer object.
- E.g.:
```c++
ImGui_ImplSDL2_NewFrame();
		
if(std::optional<EVK::CommandEnvironment> ci = interface->BeginFrame(); ci.has_value()){
	...
	interface->BeginSwapChainRenderPass({{clearColour.x, clearColour.y, clearColour.z, 1.0f}});
	...
	ImGui::NewFrame();
	
	...
	
	ImGui::Render();
	ImDrawData *drawData = ImGui::GetDrawData();
	guiRenderer.Render(drawData, ci.value());
	...
	interface->EndFrame();
}
```
