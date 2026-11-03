# 3D_rendering
This is a work in progress but I will supply updates at each commit through the devLog.md

OpenGL library framework being accessed by GLFW and the math is handled by GLM.
GLAD is being used as the loader.


The standalone files in the root of this repo are needed to run the openGL file.

  -  one is compile flags so nvim doesn't mistakenly say there is an error in the #includes
     at the top of the main.cpp file. 

  -   The other is a zsh file to compile the program without needing to explicitly state all
      parameters of compilation every time I want to compile the files. Just a quality of life thing.



# **Features**

    * file contains basic OpenGL functions for basic rendering such as 3d camera movement and frame tracking as well as fov changing
    * Entity.h header contains methods to generate an object in 3d space
        **Other Methods Inside header aside from object generation**
            - 'setVeloicty': takes 3 floats that represent the velocity of the object in each 3d axis (x, y and z)
            - 'calcPosDueToGravity' calculates the change in position of an object after the gravity of a specified object at a certain position with a certain mass is imposed on it.
                * This one is fairly limited as you have to explicitly state the position of both objects and the mass of the attractor. Will work on changing this in the future. 
