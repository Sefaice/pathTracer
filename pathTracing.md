
https://pbr-book.org/3ed-2018

## ch1 intro

1.3 system overview，讲了结构、tiled相关的细节等

## ch2

### 2.10 Interactions

SurfaceInteraction类保存了ray-surface的碰撞信息，其中有surface的dx和dy，这是由differential geometry得到的（geometry = f(u, v)），这样normal就等于dx和dy的差乘，也能得到world space to local space的变换矩阵

## ch7 sampling and reconstruction

理解像素：像素是连续图像平面上的离散采样，认为像素有面积是不对的

每维的采样值要在[0, 1)中，**为什么一定要小于1**

### 7.2

#### discrepancy

用于评价n维采样点，好的采样点是low-discrepancy的，有大量用于生成low-discrepancy point sets的方法。

discrepancy的计算方法：以2维为例，一个区域内点的占比和面积占比的差，所有区域中差的最大值就是整个样本的discrepancy

直观理解就是，生成的样本在网格中尽量分布在每个小格子中，这样能减小discrepancy

### 7.3 Stratified Sampling

生成序列时，按照需要的样本数n把区间均匀分成n个区域，每个区域是一个strata，每个区域中生成一个样本，可以选择是否jitter（用uniform random number）

为什么要一次生成n个样本，而不是单独生成每一个？ - 因为会有打乱的操作来对样本进行优化，使用Latin hypercube sampling(LHS)在每个维度进行打乱（每个维度进行不同的shuffle）

## ch?

pbrt把BRDF和bidirectional transmittance distribution function (BTDF)统称BSDF，BTDF不遵循Reciprocity

## ch13

### 13.3 Sampling Random Variables

inversion method: 对于一个cdf，先求出cdf的逆(cdf是 随机变量->累计概率密度，cdf的逆是函数的逆 累计概率密度->随机变量), 用一个[0, 1)之间的随机变量作为输入，得到采样的随机变量值

rejection methd: 对于没有pdf表示的，或cdf算不出来的情况，若f < cp就接受，**不懂为什么要这样**，但是代入圆盘投针法这个方法还是很清晰的

### 13.6 2d sampling

uniformly sampling a hemisphere: 用到了二维概率密度，边缘概率和条件概率，用inversion method得到表示

#### Sampling a Unit Disk

直角坐标转换为极坐标，对应分布的关系见13.5.1，这样就得到了极坐标下的pdf

有了pdf，就能计算边缘概率密度和条件概率密度，这样就能得到两个变量的cdf，就能得到给定一个2d sample的采样表达式，得到一个直观的采样方法

**todo: 这种直观的方法会带来distortion，concentric方法可以改善**

#### 

Cosine-Weighted Hemisphere Sampling: mc方法采样如果能按与被计算分布接近的方法采，效果会比较好，而反射方程中radiance要乘cos(theta)，所以前述的采样在cos上更好，即采样的pdf和cos(theta)成比例。然后这个采样通过Malley’s Method可以正好映射到disk上，xy就是disk采样的xy，然后显然z也可得

### 13.10 importance sampling

目的：求解一个fx的积分

需要构造一个pdf，**不知为什么**构造成px = c * fx，通过pdf的积分为1解出c，然后就能用inversion method采样了

### 13.7 russian roulette

用于加快Monte Carlo采样的效率。以p的概率选择是否进行某一采样，然后在结果中乘1/p，就保证了总期望不变，而加快了采样的整体速度

## ch 14

### 14.2

点光源不用采样，给定方向计算pdf的函数Pdf_Li返回0，即采样的bsdf给到点光源计算的pdf一直是0，因为点光源无穷小

#### shape的sample和pdf

shape里有两套Sample和Pdf函数。第一套直接在shape上采样和计算；第二套是给light用的，把被光源照亮的点作为reference point，传入ref point的位置和wi，Pdf先计算ref point在wi是否与shape相交，否则返回0，是则继续根据立体角计算pdf值

根据立体角计算pdf：第一套的pdf很直观，就是`1/面积`，第二套中需要考虑ref point，因此需要用立体角计算pdf

面积下的pdf转换到立体角下的pdf，书中的计算方法是`面积pdf / (dw/dA) = 立体角pdf，其中dw是单位立体角，dA是单位面积`，我理解这么转换的原因是`dA/面积 = dw/立体角 => 面积pdf = 1/面积 = 1/立体角*dw/dA = 立体角pdf * dw/dA`。最终`立体角pdf = 1/面积 * dA/dw`

dw/dA的计算在5.5.3，这个公式通常用于在渲染方程中把立体角的计算投影到面积上，games202的rsm用到了这个，书中也没有解释为什么是这个公式

#### 

### 14.3 Direct Lighting

只计算直接光照，即光路只有相机-表面-光源，只在表面计算一次bsdf采样和一次光源采样，再用MIS计算结果