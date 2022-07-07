# 顶点输入描述

## 简介

在接下来的几章里，我们将要用一个放在内存中的顶点缓冲替换掉硬编码在顶点着色器里的顶点数据。我们会从一个最简单的方法，即创建一个CPU可见的缓冲区然后使用`memcpy`来把顶点数据直接复制过去，然后我们会介绍如何使用一个暂存缓冲（staging buffer）来把顶点数据复制到高性能内存上。

## 顶点着色器

首先从顶点着色器里面删掉代码里的顶点数据。顶点着色器通过`in`关键字来接受从顶点缓冲输入的数据。

```glsl
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}
```

`inPosition`和`inColor`变量都是“顶点属性”（vertex attributes）。这些属性是在顶点缓冲中为每个顶点都指定了的属性，就像我们使用两个数组为每个顶点指定了一个坐标和颜色。记住要重新编译顶点着色器！

就像`fragColor`一样，`layout(location = x)`注释为输出分配了索引，这样我们就可以在后面通过索引来引用它们了。了解一些类型是很有必要的，比如`dvec3`64位向量，使用多个“槽”（slot）。这意味着在此之后的索引至少要为2或者更高：

```glsl
layout(location = 0) in dvec3 inPosition;
layout(location = 2) in vec3 inColor;
```

你可以在[OpenGL wiki](https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL))里找到更多关于布局限定符的信息。

## 顶点数据

我们会把顶点数据从着色器代码里移到我们的程序的代码里。我们从引入GLM库开始，这个库给我们提供了关于线性代数的类型，如向量和矩阵。我们将要使用这些类型来指定位置和颜色向量。

```c++
#include <glm/glm.hpp>
```

创建一个名为`Vertex`的、带有两个属性的新结构体，我们会在顶点着色器中使用它：

```c++
struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};
```

