workspace "ForgeCraft"
    architecture "x64"
    startproject "Game"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

output_directory = "%{cfg.buildcfg}=%{cfg.system}-%{cfg.architecture}"


    project "Engine"
        location "Engine"

        language "C++"
        kind "StaticLib"
        staticruntime "On"

        targetdir("bin/" .. output_directory .. "/%{prj.name}")
        objdir("bin/int/" .. output_directory .. "/%{prj.name}")

        files
        {
            "Engine/**.h", 
            "Engine/**.c",
            "Engine/**.hpp",
            "Engine/**.cpp",
            "external/**.h", 
            "external/**.c",
            "external/**.hpp",
            "external/**.cpp"
        }

        VULKAN_SDK = os.getenv("VULKAN_SDK")
        includedirs
        {
            "%{VULKAN_SDK}/Include",
            "Game",
            "external",
            "Engine/include",
            "external/VulkanWrapper/include",
            "external/VkMemAlloc"
        }
        links
        {
            "%{VULKAN_SDK}/Lib/vulkan-1.lib"
        }

        filter "system:windows"
            systemversion "latest"
            cppdialect "c++20"

            filter "configurations:Debug"
                defines "DEBUG"
                symbols "On"
            filter "configurations:Release"
                defines "NDEBUG"
                optimize "On"
            filter "configurations:Dist"
                defines "NDIST"
                runtime "Release"
                optimize "On"



    project "Game"
        location "Game"
    
        language "C++"
        kind "ConsoleApp"
        cppdialect "c++20"
        staticruntime "On"
    
        targetdir("bin/" .. output_directory .. "/%{prj.name}")
        objdir("bin/int/" .. output_directory .. "/%{prj.name}")
    
        files
        {
            "Game/**.h", 
            "Game/**.c",
            "Game/**.hpp",
            "Game/**.cpp"
        }
    
        includedirs
        {
            "%{VULKAN_SDK}/Include",
            "external",
            "Engine/include",
            "external/VulkanWrapper/include",
            "external/VkMemAlloc"
        }
        links
        {
            "Engine"
        }
    
        filter "system:windows"
            systemversion "latest"
            cppdialect "c++20"
    
            filter "configurations:Debug"
                defines "DEBUG"
                symbols "On"
            filter "configurations:Release"
                defines "NDEBUG"
                optimize "On"
            filter "configurations:Dist"
                defines "NDIST"
                runtime "Release"
                optimize "On"

    