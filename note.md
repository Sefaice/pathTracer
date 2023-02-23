## environment

### 编译embree

mac:
 * compiler用的clang，ispc会自动编译对应的SIMD部分？
 * TBB for multi-threading, tbb path: /usr/local/onetbb
 * ispc path: /Users/sefaice/apps/CG/pathTracer/ispc-v1.17.0-macOS/bin/ispc
 * 最后embree编译好后安装在/usr/local下，/usr/local/lib和/usr/local/bin为对应目录

pc: 
 * 直接下载编译好的embree，放到c:/Program Files后cmake就能直接找到

mac和pc的embree都在系统目录下，即环境变量，不用在项目中显式写路径

### 编译项目

编译一个项目的流程:
 * 参考https://github.com/oneapi-src/oneTBB/blob/master/INSTALL.md
 * 先config：`cmake .`
 * `cmake --debug-find .`用于debug find_package的过程，可以查看找到的.cmake文件获取信息
 * 在/build下编译：`cmake --build .`

需要用oiio保存图片，因此要在cmakelights.txt中添加依赖

其实理论上和vs中一样，添加include文件，添加静态链接lib文件，只是需要学习cmake中的写法

首先find_package，会找一个.cmake文件来配置这个库（见文档）

然后target_include_directories，一般都是{xxx_INCLUDE_DIR}这样的变量名

然后target_link_libraries，oiio的{xxx_LIB_DIR}只是目录没有里面的文件，这里会出错，目前只能通过显式写死.lib文件来成功编译

mac: 
 * 编译器用的clang
pc: 
 * oiio用vcpkg安装，vcpkg可以给vs直接集成，也可以给cmake用(vcpkg toolchain file)
 * `cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\apps\CG\vcpkg\scripts\buildsystems\vcpkg.cmake`要加上参数以使用vcpkg中的包
 * cmakelists中target_link_libraries要加上对应的oiio以引入dll，写法在vcpkg下载oiio的时候控制台最后说了

## embree basics

实现了bvh及求交运算

rtcSetSharedGeometryBuffer有什么用？

embree迭代会更新函数，看对应的doc

## note

pathTracing.md主要记pbrt书中的理论，这里主要记实现，可能有的类没实现导致代码不同

generateray是否需要透视投影？似乎在world space直接算不需要，直接用view space构造，利用其中camera为原点

world space, camera space都是右手系

wo是相机方向，wi是光源方向

### bsdf

f()就是计算brdf的值；Sample_f()输入wo，采样wi并计算pdf和f；Pdf()不采样，给定方向计算pdf值，给MIS用

LambertianReflection中的pdf()由cos weighted采样计算pdf

### 

shape: 先实现了个disk，包括采样一个点和计算pdf

light：Sample_Li - 输入场景中一个点的位置，采样wi计算pdf，是根据点的位置和光源信息采样的；Pdf_Li - 给定一个方向，计算光源这个方向的pdf; 目前用于检测与light碰撞的scene也写在app里（似乎应该写在light类里）

bug log:
  * disk半径大了之后有条纹 - 开始猜是随机数的问题/坐标转换问题 - debug最后发现是最终写入像素值的时候没有clamp

todo: 
  * 已实现ch14的direct lighting，即integrator类，能否实现与书中对比图fig14.13相同的结果？