# 3D_rendering
This is a work in progress but I will supply updates at each commit through the devLog.md

OpenGL library framework being accessed by GLFW and the math is handled by GLM.
GLAD is being used as the loader.


The standalone files in the root of this repo are needed to run the openGL file.

  -  one is compile flags so nvim doesn't mistakenly say there is an error in the #includes
     at the top of the main.cpp file. 

  -   The other is a zsh file to compile the program without needing to explicitly state all
      parameters of compilation every time I want to compile the files. Just a quality of life thing.
