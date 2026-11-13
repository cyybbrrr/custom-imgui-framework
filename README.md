# Custom ImGui Framework

A customized **Dear ImGui** codebase for building modern, lightweight Windows GUI interfaces with a DirectX 11 rendering backend and a collection of custom UI widgets.

This repository includes the Dear ImGui core, Win32 and DirectX 11 backends, custom `ImAdd` widgets, and bundled font resources.

## Features

- **Dear ImGui core** — based on Dear ImGui `v1.90.9 WIP`.
- **Windows support** — Win32 platform backend.
- **DirectX 11 rendering** — included DX11 renderer backend.
- **Custom widget library (`ImAdd`)**, including:
  - Animated buttons and checkboxes
  - Frame-style radio buttons and selectable items
  - Custom child regions and separators
  - Color buttons and color editors
  - Integer and floating-point sliders
  - Combo boxes and selectable entries
  - Keybind input
  - Text and color helper functions
- **Font resources** — bundled font files and generated font headers, including Font Awesome resources.
- **Customizable styling** — use Dear ImGui's style system alongside the additional widgets.

## Repository Structure

```text
.
├── fonts/
│   ├── Font Awesome font files
│   ├── generated font headers
│   └── FONT_AWESOME_README.txt
│
└── imgui_d11/
    ├── addons/
    │   ├── imgui_addons.cpp
    │   └── imgui_addons.h
    ├── imgui.cpp
    ├── imgui.h
    ├── imgui_draw.cpp
    ├── imgui_widgets.cpp
    ├── imgui_tables.cpp
    ├── imgui_impl_win32.cpp
    ├── imgui_impl_win32.h
    ├── imgui_impl_dx11.cpp
    ├── imgui_impl_dx11.h
    └── ...
```

## Requirements

To integrate this codebase into a Windows application, you will generally need:

- Windows 10 or Windows 11
- Visual Studio 2022 or another compatible C++ toolchain
- A Windows SDK
- DirectX 11-capable graphics hardware and drivers
- A host application that creates a Win32 window and a Direct3D 11 device/context

The project files in this repository provide the ImGui sources and backends; they do not constitute a complete standalone application. You must provide the application entry point, window creation, Direct3D device and swap-chain setup, and render loop.

## Integration

### 1. Add the source files

Add the Dear ImGui core source files to your project:

- `imgui.cpp`
- `imgui_draw.cpp`
- `imgui_widgets.cpp`
- `imgui_tables.cpp`

Add the Windows and DirectX 11 backend source files:

- `imgui_impl_win32.cpp`
- `imgui_impl_dx11.cpp`

Also add:

- `addons/imgui_addons.cpp`

Make sure the corresponding directories are included in your project's C++ include paths.

### 2. Include the headers

For a typical Win32 + DirectX 11 application:

```cpp
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "addons/imgui_addons.h"
```

### 3. Initialize ImGui

After creating your Win32 window and Direct3D 11 device/context, create the ImGui context and initialize the platform and renderer backends:

```cpp
IMGUI_CHECKVERSION();
ImGui::CreateContext();

ImGuiIO& io = ImGui::GetIO();
(void)io;

ImGui::StyleColorsDark();

ImGui_ImplWin32_Init(hwnd);
ImGui_ImplDX11_Init(device, device_context);
```

Here, `hwnd`, `device`, and `device_context` refer to objects created by your host application.

### 4. Render each frame

Call the backend frame functions before building your interface:

```cpp
ImGui_ImplDX11_NewFrame();
ImGui_ImplWin32_NewFrame();
ImGui::NewFrame();

// Build your interface here.

ImGui::Render();
ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
```

Your application must also handle presenting the Direct3D swap chain and resizing its render targets when the window changes size. See the official Dear ImGui Win32/DX11 examples for a complete application lifecycle.

### 5. Use the custom widgets

Include `addons/imgui_addons.h` and call the functions in the `ImAdd` namespace:

```cpp
#include "addons/imgui_addons.h"

void RenderSettings()
{
    static bool enabled = true;
    static int quality = 2;
    static float opacity = 0.75f;

    ImGui::Begin("Settings");

    ImAdd::CheckBox("Enable feature", &enabled);
    ImAdd::SliderInt("Quality", &quality, 0, 5);
    ImAdd::SliderFloat("Opacity", &opacity, 0.0f, 1.0f);

    if (ImAdd::Button("Apply"))
    {
        // Apply your settings.
    }

    ImGui::End();
}
```

## Custom Widget Reference

The additional widgets are declared in `imgui_d11/addons/imgui_addons.h`.

| Function | Description |
|---|---|
| `ImAdd::Button()` | Custom button |
| `ImAdd::CheckBox()` | Custom checkbox |
| `ImAdd::RadioFrame()` | Frame-style radio button |
| `ImAdd::RadioFrameGradient()` | Gradient radio button |
| `ImAdd::RadioFrameText()` | Text-style radio button |
| `ImAdd::SelectableFrame()` | Custom selectable frame |
| `ImAdd::BeginChild()` / `ImAdd::EndChild()` | Custom child region |
| `ImAdd::SeparatorText()` | Separator with a label |
| `ImAdd::DoubleText()` | Draw two text labels with separate colors |
| `ImAdd::ColorButton()` | Custom color button |
| `ImAdd::ColorEdit4()` | Four-component color editor |
| `ImAdd::SliderInt()` | Integer slider |
| `ImAdd::SliderFloat()` | Floating-point slider |
| `ImAdd::Combo()` | Combo box overloads |
| `ImAdd::KeyBind()` | Keybind input |
| `ImAdd::HexToColorVec4()` | Convert a hexadecimal color to `ImVec4` |

The widgets use Dear ImGui's current context and style. Call them during the normal ImGui frame-building phase, between `ImGui::NewFrame()` and `ImGui::Render()`.

## Fonts

The `fonts/` directory contains font assets and generated headers. Font Awesome 7 resources are included for icon usage.

If you load external font files at runtime, ensure the paths are valid relative to your application's working directory, or use absolute paths. If you embed fonts through the generated headers, follow the loading approach used by the corresponding header/source integration in your application.

Font Awesome is a third-party project. See the Font Awesome licensing information before redistributing its font files.

## Notes

- This repository is a customized ImGui codebase and widget collection, not a complete GUI application or visual editor.
- The custom widgets are implemented in `imgui_d11/addons/`.
- The included Win32 and DirectX 11 backends are intended for a Windows desktop integration.
- Keep the Dear ImGui core and backend versions in sync when updating or replacing source files.
- Some custom widgets use Dear ImGui internal APIs. These may need adjustments when upgrading to a different Dear ImGui version.

## Credits & Licensing

This codebase is built on **[Dear ImGui](https://github.com/ocornut/imgui)** by Omar Cornut and contributors.

Dear ImGui is available under the MIT License. Review the original project and its license before redistributing modified copies.

The custom `ImAdd` widget code and bundled third-party font assets may have separate authorship or licensing terms. Preserve applicable copyright notices and check each asset's license before redistribution.

- Dear ImGui: https://github.com/ocornut/imgui
- Dear ImGui documentation: https://github.com/ocornut/imgui/wiki
- Font Awesome: https://fontawesome.com/license

---

**Built for developers who want a customized ImGui foundation for Windows desktop interfaces.**
