# MinecraftBot ⛏️
A minecraft mining bot made with openCV and win32 API. Currently, the bot mines a straight tunnel and avoids burning in lava. 💎 detection might be added. 
## 🛠️ Build 🧰
Requirements: Windows only, VS 2019 (You can downgrade by modifying the generate projects batch file) 
To build the project, navigate to the build directory and run the setup batch file. 
### How it works
Each iteration, the program captures the game's framebuffer and copies it to an openCV mat class. Then, a HSV color mask is applied to a copy of the image (as show below) to determine if lava is in player's sight - if the amount of masked pixels (0 - 100%) exceeds a given threshold, the bot takes a few steps back and rotates the camera to continue mining in a different direction.
![MaskImage](https://github.com/CzekoladowyKocur/MinecraftBot/blob/master/showcase/Comparision.png)
### Input:
#### [O] - Toggle On/Off
#### [T] - Shutdown (works only when the game's window is unfocused) 
