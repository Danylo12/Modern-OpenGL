# Modern OpenGL Learning Project

This repository contains my personal journey and project focused on learning **Modern OpenGL**. Instead of using the deprecated fixed-function pipeline, this project focuses on a shader-based approach using **GLSL**, **Vertex Buffer Objects (VBOs)**, and **Vertex Array Objects (VAOs)**.

## Key Features
- **Shader-based Rendering**: Custom implementation of Vertex and Fragment shaders.
- **MVP Matrix Pipeline**: Full implementation of Model, View, and Projection matrices to handle 3D transformations.
- **Perspective Projection**: Real-world camera simulation including perspective division.
- **Smooth Interpolation**: Using the rasterizer to interpolate colors and positions across primitives.

## Tech Stack
- **Language**: C++
- **Graphics API**: OpenGL 3.3 Core Profile
- **Libraries**: 
  - **GLEW**: OpenGL Extension Wrangler for loading functions.
  - **Win32 API**: For window management and input handling.
- **Shading Language**: GLSL (OpenGL Shading Language)

## Mathematical Concepts Implemented
One of the main focuses of this project was mastering the 3D coordinate transformation pipeline:
1. **Model Space**: Local coordinates of the vertices.
2. **World Space**: Placing objects in the scene using a World Matrix.
3. **Eye (View) Space**: Transforming the scene relative to the camera position.
4. **Clip Space**: Applying the Projection Matrix (Perspective/Orthographic).
5. **NDC (Normalized Device Coordinates)**: The final step after the $w$-division where coordinates range from -1.0 to 1.0.
