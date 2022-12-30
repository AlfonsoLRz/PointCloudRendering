# Large Point Cloud Rendering   ![c++](https://img.shields.io/github/languages/top/AlfonsoLRz/PointCloudRendering) ![numpy](https://img.shields.io/badge/imgui-1.82-green.svg) ![license](https://img.shields.io/badge/license-MIT-blue.svg)
This work is intended to provide a fast rendering of large point clouds in OpenGL using compute shaders. It is built over an interface based on ImGui for easing the opening and visualization of large point clouds with a high rate of frames per second (FPS). This work is implemented from the research of Markus Schütz.

<p align="center">
  <img src="Assets/Images/AbstractComposition.png" width="800px" alt="PointCloudRenderingIntro">
</p>

___
## Features

* The provided project opens large point clouds limited by the memory size of the GPU. The maximum allowed size of a Shader Storage Buffer Objects (SSBO) is far behind the GPU capabilities, thus the point clouds are partitioned into several chunks. Nevertheless, point clouds of several GBs may not be rendered in this application.

* Beyond opening large point clouds, the base work of Markus Schütz allows improving the visualization of point clouds through the last extensions of compute shaders. See the image below to compare the enhancement with respect to GL_POINTS rendering mode.

<p align="center">
  <img src="Assets/Images/HQRComparison.png" width="800px" alt="HQRComparison">
</p>

* As an alternative for large point clouds that cannot be completely loaded into the GPU, we provide a tool for reducing the size of a point cloud. For that purpose, points are sorted along a _z_-curve to narrow surrounding points into a single one.

<p align="center">
  <img src="Assets/Images/ReductionComparison.png" width="800px" alt="ReductionComparison">
</p>

* Take screenshots of higher dimensions than the current canvas size. By default, they are saved as Screenshot.png under the root directory, though both the size multiplier and path destination are customizable.

* Graphical User Interface (GUI) provided by the ImGui library with the aim of configuring the visualization of the environment as well as the loading stage.

<p align="center">
  <img src="Assets/Images/GUI.png" width="800px" alt="GUI">
</p>

___
## Stats

The following graphics reports FPS rates for point clouds of different complexity rendered in two graphic cards (Nvidia GTX 1070 and Nvidia RTX 2080 Ti).
___
## TODO

* Include a more refined reduction algorithm, as the neighbourhood reduction given by Morton codes is mainly driven by the precision of such values for representing close points.

* Translate code to Vulkan, as it might severely improve the performance.

___
### Dependencies

`imgui: 1.82`
`glm: 0.9.9`
`glew`
`glfw: 3.3.0`
`openGL: ≥ 4.5`

___
### References

> Markus Schütz and Bernhard Kerbl and Michael Wimmer. 2021. **Rendering Point Clouds with Compute Shaders and Vertex Order Optimization**. [arXiv:2104.07526](https://arxiv.org/abs/2104.07526)

> Markus Schutz and Michael Wimmer. 2019. **Rendering Point Clouds with Compute Shaders**. In SIGGRAPH Asia 2019 Posters (SA '19). Association for Computing Machinery, New York, NY, USA, Article 32, 1–2. DOI: https://doi.org/10.1145/3355056.3364554
