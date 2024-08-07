//
//  AssetPriceModel.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//
// 所有的模型都需要核对计算公式是否正确
// 多维股价模型、dW的位置换成其他的鞅dM（可以产生更多的模型，但是这些模型不适用于伊藤引理）
// 底层资产价格模型：单期、单轨迹
// Wiener过程就是标准正态分布epsilon * (delta t) ^ (1/2)
// 只要标的资产遵循对数正态随机游走，且sigma仅为时间t的函数，如果sigma同时是St的函数，则可能出现复杂显式解或者只能用数值方法求解。另外要求r是t的确定性函数（显式解要求）。

# include "AssetPriceModel.hpp"
# include "Parameters.hpp"

// 几何布朗运动模型实现
GeometricBrownianMotionModel::GeometricBrownianMotionModel(const Parameters& params): dt_(params.get<double>("dt")) {}
// dW_spot_(params.get<double>("dW_spot"))，由于随机变量都是在MonteCarloSimulator中生成的，所以dW不能进入Model的初始化列表，否则将导致无法实现相关Class的构造函数

double GeometricBrownianMotionModel::simulatePrice(const Parameters& params) const {
    return params.get<double>("St") * std::exp((params.get<double>("rt") - 0.5 * params.get<double>("vt") * params.get<double>("vt")) * dt_ + params.get<double>("vt") * params.get<double>("dW_spot"));          // checked，其中rate就是mu
}
// 这里没有更新成员变量的函数，所以这里用的都是params.get，只要增加了更新函数，那么就可以用spot_等，两种方法哪个计算效率等更高有待考量。如果这里直接用spot_，会始终用一个取值不变的spot_，而不更新。如果增加更新，则可以将spot_ volatility rate dW_spot_等都拉入成员变量中。现在的格式则不允许这么做，即便做了simulate这里也必须要到params中get最新的取值

// 跳跃扩散模型实现
JumpDiffusionPriceModel::JumpDiffusionPriceModel(const Parameters& params):  dt_(params.get<double>("dt")),  jumpMean_JDPM_(params.get<double>("jumpMean_JDPM")), jumpVol_JDPM_(params.get<double>("jumpVol_JDPM")), jumpIntensity_JDPM_(params.get<double>("jumpIntensity_JDPM")), jumpSize_JDPM_(params.get<double>("jumpSize_JDPM")) {} //, dW_spot_(params.get<double>("dW_spot"))

double JumpDiffusionPriceModel::simulatePrice(const Parameters& params) const {
    double jumpComponent_ = exp(jumpMean_JDPM_ * jumpSize_JDPM_ + jumpVol_JDPM_ * sqrt(jumpSize_JDPM_) * params.get<double>("dW_spot"));
    return params.get<double>("St") * exp((params.get<double>("rt") - 0.5 * params.get<double>("vt") * params.get<double>("vt")) * dt_ + params.get<double>("vt")) * jumpComponent_;
}


/*
DoubleGBMM::DoubleGBMM(const Parameters& params): dt_(params.get<double>("dt")) {}

double DoubleGBMM::simulatePrice(const Parameters& params) const {
    return // dS1 = a1(S1, S2, t)dt + b1(S1, S2, t)dX1
 // dS2 = a2(S1, S2, t)dt + b2(S1, S2, t)dX2
 // dX1dX2 = rho dt
}


 */



// 一般的衍生品定价只需要风险中性世界的资产价格模型，即经过Girsanov定理转换后的模型，因为风险中性利率折现后的风险中性世界的价值和现实世界的定价完全一致（二者仅是在价格分布上不一致）。但是对于需要现实世界价格路径作出判断的例如障碍期权等，则需要真实世界的底层资产价格路径作为障碍触发判断依据，也就需要真实世界mu和与Girsanov dW公式对应的真实世界dW作为资产价格模型。




// 前向和后向方程：根据广义随机微分方程可以得到一组方程。大致步骤为用三叉树模拟连续的广义随机微分方程，保证二者均值和方差一致，分别求出上中下移动的概率。根据概率按照3->1或者1->3的不同方法求出相邻两期转移概率之间的关系。用Taylor展开法得到两个方程。如果作出跳跃模型假设或者别的dX随机变量假设，只需要修改均值和方差取值，其余部分都一样。最终将得到不同的两个方程。
// 两个方程可以在各种资产价格模型基础上推导，包括现实世界和风险中性世界等，得到的概率密度方程对应着不同测度世界的概率密度函数。



// 测度转换：改变的是每个取值对应的概率，也就是数据分布会发生变化。无论进行什么测度转换，计算得到的终值都是一样的，只是每个值分配的概率不一样。也就对应着不同的均值（分布不同）
// 只有在风险中性测度下，折现过程才是鞅。并且，如果不是鞅（假设是上鞅或者下鞅），那么折现以后（无论用什么折现率折现mu或者r）得到的现值都不会等于理论的当前价格。
// 对于障碍期权来说，首先要证明V的折现过程在风险中性测度下是鞅，然后其折现的价值一定是定价。
