\# Vulkan Renderer



A Vulkan renderer written in C, built from scratch: swapchain, pipelines,

descriptor sets, and shaders all hand-written, no engine underneath.



> Work in progress. Core rendering is functional; organization and

> optimization are ongoing.



\## What it does



\- Textured 3D rendering with a hand-written Vulkan backend

&#x20; (`src/vulkan\_backend`), windowing via GLFW (`src/platform`).

\- Real-time shadow mapping (depth-only pass, PCF filtering).

\- Normal mapping and parallax (offset) mapping.

\- Two lighting paths: a physically based (Cook-Torrance) pipeline for

&#x20; PBR materials, and a Blinn-Phong pipeline for simpler/background

&#x20; geometry — chosen per-object.

\- Free-fly camera.



\## Layout



src/

app/ entry point, main loop

platform/ GLFW window + input

renderer/ render orchestration

vulkan\_backend/ raw Vulkan: instance, device, swapchain, pipeline, etc.

include/ vendored cglm (MIT) math library

shaders/ GLSL source + compiled .spv

assets/ textures and example geometry





\## Building (Windows)



You'll need MSVC (`cl.exe` on PATH), the \[Vulkan SDK](https://vulkan.lunarg.com/),

and \[GLFW](https://www.glfw.org/) 3.4.



1\. Set your local SDK paths at the top of `build.bat`:

```bat

&#x20;  set VULKAN\_SDK=C:\\VulkanSDK\\1.4.350.0

&#x20;  set GLFW\_DIR=C:\\path\\to\\glfw-3.4

```

2\. Compile the shaders once (and again whenever you edit one):

```bat

&#x20;  cd shaders \&\& compile.bat \&\& cd ..

```

3\. Build:

```bat

&#x20;  build.bat

```

4\. Run it from inside `build\\`:

```bat

&#x20;  cd build \&\& app.exe

```



\## Known rough edges



\- Project structure is still being reorganized.

\- No tests yet.

\- Not yet optimized (texture loading, descriptor usage).



\## Preview



!\[image 1](screenshots/barrel.png)

!\[image 2](screenshots/Sting.png)



\## License



TBD.

