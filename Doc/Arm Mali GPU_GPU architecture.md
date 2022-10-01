# 深入探索GPU图形架构

## 一. The Rendering Pipeline
### 渲染管线中的各个阶段各自负责那些工作呢？
管线中总共有四个工作阶段参与：
![](./Image/Process_stage.png)
#### 1. CPU Application： 
CPU会执行应用程序以及图形驱动程序。这工作阶段主要负责前置作业， 比如动画或是物理运算，并将CPU所产生的数据写入DRAM之中，并且发送绘图指令到GPU。
#### 2. command processing：
指令处理阶段则是在GPU内的控制阶段，负责解译从CPU传来的绘图指令，协调 GPU 的数据处理阶段。
#### 3. geometry processing：
##### 3.1 未优化之前的Geometry Processing
几何处理阶段则是会读入网格模型， 然后送出多边形的串流，一般来说就是三角形（绿色使用者定义的shader来处理，蓝色显示固定功能的硬件来处理）。 
![](./Image/Geometry_Processing.png)
**Tessllation shader:** 镶嵌处理则是一种非必要的管线阶段,可以用来将模型座程序化的细分,可以动态的增加几何物件的细节,他本身实际上还包含三个子管线阶段,由两个可程序化的管线阶段 ,跟一个固定功能的tessellator所组成(镶嵌是非常耗效能的技术, 这技术很少出现在市面上的手游之中)
**Geometry shader:** 几何处理技术在现今已经有些过时,大部分几何处理适用的案例,都可以被现今的运算处理所取代
**Primitive Shader:** 可以使用顶点串流组合，也可以由index buffer组合成点、线或是三角形。
**Primitive Culling：** 开始处理在视野内可见的多边形，并且丢弃掉不在视野内的多边形，如此可以减少接下来像素处理的工作量。

##### 3.2 Better Geometry processing
背向摄影机一般称之为模型的反面，也可以藉由facing test来剔除，但是事实上却很没效率，因为剔除工作發生在管线最末端，在我们已经花费珍贵的电力处理完端点之后，即使我们已经在应用程序端做了完美的可视范围剔除，我们仍然可能需要丢弃一半的端点，只因为他们是背向摄影机。因此大部分的GPU事实上会重组管线顺序，让剔除工作儘量在早期执行，下方的图显示的是Mali GPU中的管线工作阶段顺序
![](./Image/Better_Geometry_processing.png)
在这个改进之中，vertex shader会被shader compiler分成两个部分
* 前半段部分负责计算端点位置，剔除工作仍然是在前半段shader执行之后才执行
* 而后半段部分则负责计算与端点位置无关的所有属性值，后半段shader则是在剔除工作之后执行。


#### 4. pixel processing： 
像素处理阶段则会取用几何处理阶段多边形的数据，光栅化这些数据以产生pixel coverage，然后执行像素运算来计算出每个像素的颜色。
![](./Image/Pixel_Processing.png)
##### 4.1 Rasterizer
像素处理的第一个阶段是光栅化，这个阶段会取得三角形的真实向量然后compares it against a per-pixel sample mask to determine which parts of the framebuffer get colored in.
* 一般情况下，每个像素应该只会产生单一的fragment shader计算，但是invocation rate有时会较高，比如使用多重取样时；计算率有有可能较低，比如使用可变速率着色(VRS，variable rate shading)
* 当光栅在产生输出时,是以2x2的像素区块为单位，称之为quad. quad形成像素渲染的基本单位.因为藉由空间相邻的四个像素, 硬件才能计算出mipmap以及非等向性过滤所需要的导数ddx。
* 太小的三角形可能只包含quad中部分的取样点,将会导致硬件中部分fragment shader通道的闲置,进而降低了shader的执行效率
![](./Image/Pixel_Processing_quad.png)

##### 4.2 Early ZS test
在shader程序码中使用gl_FragDepth来修改深度值，将会导致Early ZS的功能被关闭，进而强制像素执行Late ZS测试，而在像素shader执行之后丢弃掉像素是代价很高的工作，因此应用程序应该尽量避免。
* Late ZS测试發生在像素shader执行之后，负责处理所有像素处理之前无法处理的，深度以及stencil的测试及更新
* 为了利用到Early ZS测试的加速，应用程序必须将几何数据依照由前往后的顺序送出，如果应用程序由后往前送出几何数据，则会完全无法利用到Early ZS测试的加速。

##### 4.3 Blend
混色阶段则是固定功能的阶段，混色阶段负责根据应用程序设定的混色方式，将透明像素的颜色与画面颜色混合

##### 4.4 Better Pixel Processing

原始的pixel processing管线阶段顺序，它包含了两个没有效率的地方：
* 第一，Early ZS测试非常依赖应用程序端依照正确的顺序送出几何数据，
![](./Image/Better_Pixel_Processing.png)
  * **hidden surface removal：**
为了解决几何物件排序对Early ZS测试造成的影响，现代的硬件都使用了所谓隐藏面移除（hidden surface removal）的技术，这种技术可以剔除被遮蔽的像素，即便几何数据没有依照由前往后的次序送入。 但是由前往后渲染不透明物件仍然是最好的方式，但是这项技术可以补足渲染次序错误的部分，并且剔除掉大部分的被遮蔽像素，而Late ZS测试及更新被设计成可由shader程序码触發的功能，让Late ZS可以在程序码达到触發条件时，立刻被触發，这样可以最大幅度减少对被遮蔽的像素做不必要的运算。
* 另外一个关于管线的重要概念是parallelize处理：应用程序在CPU端建立起draw call，接着在GPU端做几何处理，再接着由GPU端做像素处理，这些阶段在管线中循序进行，因此，为了让管线中所有阶段不会閒置，我们需要让多个工作同时在硬件上执行。一旦硬件无法有效的parallelize处理工作，就会导致渲染效率降低.
![](./Image/Pipeline_Parallel_process.png)
  * 让GPU中每个管线阶段的工作可以尽可能的重叠,是图形最佳化的一个最重要的目标
  * OpenGL ES的API是一种同步的API架构，有些API（比如glFinish）会强制执行同步行为。
  * 现代图形API基本上都是非同步架构。 Vulkan则是为了现今的硬件而设计的API。所以Vulkan提供了一个非同步工作的架构。但这会需要应用程序藉由明确的安排数据写入与读取间的相依性，去定义哪些工作可以parallelize处理。


#### Other details
* Shader： 程序可以定义执行在每个端点跟像素上的运算
* Buffer： 缓冲则用来存放输入的端点数据、几何数据以及场景的资讯、几何数据以及场景的资讯，像是每个物件的转换矩阵，或是动态光源的资讯等等
* Texture： 贴图则包含影像数据，色彩或非色彩data，这些数据可以在渲染阶段作为输入使用，它们也可以被拿来当作画面缓冲使用，将渲染管线处理后的数据写入
* Descriptors： 控制资源数据组织，provide supplemental information to the GPU.  每一个资源都需要一个descriptor来记录其在内存中的位置和如何被存取（where it is in memory and how it should be accessed. ），每一个渲染指令都会引用到一个descriptor。来告诉GPU每个draw call配置，并且列出所有完成运算所需要用到的资源，当CPU送出指令到GPU，指令处理器就会读取对应的descriptor，并用这些资讯来设置好接下来的渲染管线。

### 5. Compute Processing Stage

**Compute Processing**：
运算处理并不是渲染管线必要的阶段,上面其他的工作阶段组成严格的管线（数据由一个阶段传至下一个工作阶段，直到最后输出应用程序所需要的数据）。运算处理则是在这些管线之外，直接由main memory抓取数据，然后在运算完成之后将结果写回main memory，并转换成缓冲或是贴图的格式，这使得运算处裡可以在管线的任一阶段介入。
* eg： 举例来说，可以用来动态产生程序化的网格模型 ，然后存放在缓冲中，最后在几何处理阶段中读入。

## 二. GPU Architecture
当我们在谈论GPU架构时，通常我们谈论的是GPU的宏观设计，以及渲染管线如何与应用程式以及驱动程式沟通，高阶渲染API提供了很厚的抽象层。
在现今的市场上，有着许多相当不同的GPU架构设计，在任一个GPU核心元件的设计中都包含两个主要处理阶段以及一个输出画面缓冲用来存放处理后的像素结果。
![](./Image/GPU_architecture.png)
这三个部分由某种数据传输方式串连起来。 传输方式的不同，成了不同GPU架构的设计依据。

### 1. Immediate Mode Pipeline 

接下来的动画会展示传统的立即模式渲染是如何运作, 首先多边形会依照应用程式送出的顺序依序处理, 这代表了渲染有时会在屏幕上的同一个位置,重复处理之前已经处理过的像素。
* 中间蓝色和绿色重叠的部分会重复绘制两次）。
![Immediate_Mode_Renderer](./Image/Immediate_Mode_Renderer.gif)

立即模式渲染将几何处理，以及像素处理，使用GPU中的简单的先入先出缓冲相连，或称为FIFO缓冲，几何处理会将三角形推入FIFO缓冲中。
* 缓冲用尽时暂停，然后等待像素处理单元处理。像素处理会将多边形由此缓冲中取出，并依据多边形串流的次序来依序处理。
![](./Image/Immediate_mode_Pipelining.png)
* 像素处理的工作范围涵盖整个画面，因为任何一个从FIFO缓冲中读入的三角形可能落在屏幕的任何地方，这个范围太大，所以于无法全部存放在chip之中，而是存放在主main DRAM 之中。
* 这样的架构会将三角形data存放于chip之中，所以几何处理的成本较低，但像素处理的成本较高。
* 粒度较细对API有较高的容错能力。

### 2. Tiled based Pipeline
下面动画展示的则是传统的Tile-based渲染行为，比如arm的Mali GPUs，首先，屏幕会被切割成许多较小的区块，称之为Tile，每一个Tile则会在所有fragment的渲染完成后才将结果写入main Memory，Draw call有时会在Tile之间交错进行，而Tile-based渲染会需要在fragment shader被执行前，确认每个Tile中有那些多边形需要被绘出。
* 每个render pass的几何处理的工作必须先完成，然后将处理后的resource以及intermediate state，比如Tile coverage infomation写入main Memory之中， 因为数据太大，无法暂时放置在GPU之中。
![](./Image/Tile_based_Mode_Renderer.gif)
* 像素处理会将所有Tile依序处理，根据之前几何处理写入的Tile list来决定哪些多边形需要读入，因为Tile够小，所以可以将整个Tile所对应的画面缓冲data保存在GPU之中，处理完成的Tile则会将画面缓冲的data写回Memory。 
![](./Image/Tile_Based_Pipelining.gif)
* 几何处理是第一个部分而且必须在第二部分像素处理开始前全部完成。 这样的设计由将成本较低的像素data保留在chip之中.来换取成本较高的几何处理。


#### immediate VS tile-based
Render passes会在最后一个drwa call后被产生， 所有的几何处理必须先完成，才能建立出Tile列表。 和immediate mode不同的是，Tile-based是以render passes来做区分粒度，而非draw call。
![](./Image/Tiled_based_VS_Immediate.png)
* 使用现代图形API（如：wulkan）dependency management需要更加小心的处理。 过度的依赖性，会导致不必要的顺序依赖。


### Tiled Based Rationale
因此目前大部分应用都是是像素处理要求比较高的，而Tile-based渲染的优势主要来自有效的使用in-GPU Tile Memory来写入画面缓冲的内容。许多常见的像素处理，像是深度测试、混色或是多重取样，都可以在GPU内部高速完成，而不用碰触到外部的Memory。
![](./Image/Tile_based_prosVScons.png)
**TBR pros**

* 暂时性的输出，比如只有在render pass中需要的data，可以在Tile完成时直接被丢弃，可以节省DRAM bandwidth。
* 由于可以同时处理一个Tile中所有多边形的特性，使得硬件可以非常有效的执行隐藏面的消除工作，因为GPU可以看到所有多边形的前后顺序
* 可以更进一步的利用Tile Memory来当作，渲染管线的的暂存Memory。以利用这点来实现像是deferred lighting的算法，使得原本需要将暂存data写入外部Memory以便稍后的光照阶段作为贴图读入的deferred lighting变成可以全部在GPU RAM内部完成，而不需要在外部Memory来回搬运。

**TBR cons**
Tile-based渲染的一个缺点是: 必须使用外部Memory来存放几何处理完成之后的data,这使得几何处理的成本会比,立即模式渲染的几何处理成本要高,因为硬件需要额外的Memory频宽跟空间,来暂时存放几何处理后的data 。因此应用程式同时必须遵守一些API的调用规则,才能确保Tile-based渲染可以有效的parallelize处理
* 对于tessellation以及geometry shader,都会程序化的增加几何复杂度,因此会比较适合立即模式渲染的架构,因为增加的几何data,可以简单地写入FIFO之中,但对Tile-based GPU这些增加的几何data会需要写入外部Memory



### 参考资料

1. [Arm Mali GPU Training Series](https://youtu.be/tnR4mExVClY)


