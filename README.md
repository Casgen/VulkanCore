# Vulkan Core Library

This repo serves as a framework for working with Vulkan and faster development.

## Prerequisites

Make sure you have installed these libraries:
- Vulkan SDK (or Vulkan `libvulkan-dev` if you are on Ubuntu on Linux Mint)
- Bear (This is applicable to developers who want to use clangs `compile_commands.json`)

Also setup your own `$VULKAN_SDK` Environment variable where the path will point to the `1.X.X.X/x86_64` (for ex. `/home/username/Dev/VulkanSDK/1.X.X.X/x86_64`) dir on Linux or
`1.X.X.X` on Windows (for ex. `C:\VulkanSDK\1.X.X.X`)

## Building VulkanCore
---

The repo can be built with Premake5 (included in the `Vendor/premake/` dir).

There can be two ways to build it.

### Having source files directly in the repo

If you are going to create your own source files in the `Src` directory, you can run `premake <your project type>` directly. This will generate the project files.
After that you can build it with your own IDE or other desired toolchain (for ex. `make`)

### Another project located outside VulkanCore

In order to build it, you need to create a new `premake5.lua` file in the project, in which this repo or VulkanCore exists, and include the `VulkanCore`
premake file in that newly created. You also may need to pass into arguments the newly created .lua file when executing premake command.

For ex.

```lua
---@diagnostic disable: undefined-global
workspace("ExampleProject")

    -- Configurations have to be defined first before including any other premake.lua files
    configurations({ "Debug", "Release" })

    include("ExampleProject")
    include("VulkanCore")

```

This `premake5.lua` file is located outside VulkanCore directory. (`/home/username/dev/Cpp/repo/VulkanCore/`, `/home/username/dev/Cpp/repo/ExampleProject/`, `/home/username/dev/Cpp/repo/premake5.lua/`)


