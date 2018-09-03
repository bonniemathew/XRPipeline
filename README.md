# XRPipeline
Simple XR Pipeline in OpenGL

Visual Studio Version - Visual Studio Community 2015

Folder Structure
-----------------
Include Files - XRPipeline/include <br />
Libs - XRPipeline/libraries <br />
Src - XRPipeline/XRPipeline/src <br />
Images - XRPipeline/XRPipeline/res <br />
Shader codes - XRPipeline/XRPipeline/shaders <br />

Final Application Binary - XRPipeline/binary



Input Controls
---------------
W, A, S, D - For moving forward, left, backward and right. <br />
Mouse scroll for Zoom <br />
'V' - For toggling ON/OFF distortion <br />


Description
-------------

Used OpenGL to develop this application. Other libraries used are glm for matrices and vectors, stb_image for jpg image loading, glfw for window creation.

The entry point of the application and the renderloop is in XRPipeline/XRPipeline/src/main.cpp. 

The scene is rendered in the RenderScene(). For this application, the scene consists of one cylindrical dome and the eye buffer texture mapped to it. 
Each renderable object is derived from the base class RenderPrimitive. So Cylinder and Quad class are derived from the RenderPrimitive class.
There are two lists maintained  - one is the ScenePrimitivesList and PipelinePrimitivesList. 
ScenePrimitivesList holds the rendering elements present in the scene.
PipelinePrimitivesList holds the rendering elements needed for the pipeline. 

In the scene rendering, the cylinder is drawn with the texture cordinates mapped from 0 to 1, for the 225 to 225 + 80 deg FOV. These are drawn to the EyeBuffers and then drawn on the final display
after applying the Barrel distortion. 

