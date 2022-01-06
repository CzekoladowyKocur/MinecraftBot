local RootDirectory = "../"

workspace "MinecraftBot"
	location(RootDirectory)
	entrypoint "wWinMainCRTStartup"  

	configurations 
	{
		"Debug",
		"Release",
	}

	platforms 
	{
		"x64", 
	}

	filter "configurations:Debug"
		defines "APP_DEBUG"

project "MiningBot"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir (RootDirectory) -- where the output binary goes.
    targetname "MiningBot" -- the name of the executable saved to targetdir

	local ProjectSourceDirectory = RootDirectory .. "MiningBot/"
	files
	{
		ProjectSourceDirectory .. "**.h",
		ProjectSourceDirectory .. "**.hpp",
		ProjectSourceDirectory .. "**.c",
		ProjectSourceDirectory .. "**.cpp",
	}

	includedirs
	{
		ProjectSourceDirectory,
		ProjectSourceDirectory .. "vendor/opencv/",
	}

	libdirs
    {
      -- add dependency directories here
    }

    links
    {
		RootDirectory .. "MiningBot/vendor/opencv/lib/opencv_core454.lib",
		RootDirectory .. "MiningBot/vendor/opencv/lib/opencv_features2d454.lib",
		RootDirectory .. "MiningBot/vendor/opencv/lib/opencv_flann454.lib",
		RootDirectory .. "MiningBot/vendor/opencv/lib/opencv_highgui454.lib",
		RootDirectory .. "MiningBot/vendor/opencv/lib/opencv_imgcodecs454.lib",
		RootDirectory .. "MiningBot/vendor/opencv/lib/opencv_imgproc454.lib",
		RootDirectory .. "MiningBot/vendor/opencv/lib/opencv_ml454.lib",
		RootDirectory .. "MiningBot/vendor/opencv/lib/opencv_objdetect454.lib",
		RootDirectory .. "MiningBot/vendor/opencv/lib/opencv_video454.lib",
		RootDirectory .. "MiningBot/vendor/opencv/lib/opencv_videoio454.lib",
    }