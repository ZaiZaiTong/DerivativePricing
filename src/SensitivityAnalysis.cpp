//
//  SensitivityAnalysis.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//
// 调用MCS三个链条进行敏感性分析

/* 当前计算希腊字母的前沿方法有许多，除了有限差分法和蒙特卡洛法，以下是一些被广泛研究和应用的方法：

1. 自动微分 (Automatic Differentiation)
自动微分是一种通过将计算图中的每个操作自动应用链式法则来计算导数的技术。它的优势在于能够高效、准确地计算导数，而不会像有限差分法那样面临舍入误差问题。

2. 双重蒙特卡洛 (Adjoint Monte Carlo)
双重蒙特卡洛是一种改进的蒙特卡洛方法，它通过同时模拟一个金融资产的价格路径及其对参数的敏感性来计算希腊字母。与传统蒙特卡洛方法相比，双重蒙特卡洛具有更高的计算效率。

3. 梯度增强树 (Gradient Boosted Trees)
梯度增强树是一种机器学习技术，通过集成多棵决策树来提高预测精度。在计算希腊字母时，可以训练梯度增强树模型来拟合期权价格对输入参数的响应，从而计算其导数。

4. 深度学习 (Deep Learning)
深度学习尤其是神经网络近年来在金融领域得到了广泛应用。通过训练神经网络模型来逼近期权价格的复杂非线性关系，可以通过反向传播算法高效计算导数。

5. 局部波动率模型 (Local Volatility Models)
局部波动率模型假设波动率是资产价格和时间的函数。通过这些模型，可以更准确地捕捉期权价格的动态变化，并计算相应的希腊字母。

6. 随机波动率模型 (Stochastic Volatility Models)
随机波动率模型如Heston模型假设波动率本身是一个随机过程。通过这些模型，可以更好地反映市场中的波动性聚集现象，并计算更精确的希腊字母。

7. 基于偏微分方程 (PDE) 的数值方法
除了有限差分法，其他数值解PDE的方法如有限元法 (Finite Element Method, FEM) 和谱方法 (Spectral Methods) 也可以用于计算希腊字母。它们通常具有更高的精度和稳定性。

8. 长记忆模型 (Long Memory Models)
长记忆模型如分数布朗运动（Fractional Brownian Motion）考虑到市场价格的长记忆特性，通过这些模型可以更准确地捕捉市场的长期依赖性，并计算相应的希腊字母。

这些前沿方法在不同的应用场景中各有优势，选择合适的方法取决于具体问题的需求和约束条件。*/

# include "SensitivityAnalysis.hpp"
# include "PricingModel.hpp"
# include "RateModel.hpp"
# include "VolatilityModel.hpp"

/*
// 构造函数实现
SensitivityAnalysis::SensitivityAnalysis(const PricingModel& model, double spot, double strike, double maturity, double volatility)
    : model(model), spot(spot), strike(strike), maturity(maturity), volatility(volatility) {}

// 计算Delta
double SensitivityAnalysis::computeDelta(CalculationMethod method) const {
    if (method == CalculationMethod::FiniteDifference) {
        return model.computeDeltaFiniteDifference(spot, strike, maturity, volatility);
    } else {
        return model.computeDeltaMonteCarlo(spot, strike, maturity, volatility);
    }
}

// 计算Gamma
double SensitivityAnalysis::computeGamma(CalculationMethod method) const {
    if (method == CalculationMethod::FiniteDifference) {
        return model.computeGammaFiniteDifference(spot, strike, maturity, volatility);
    } else {
        return model.computeGammaMonteCarlo(spot, strike, maturity, volatility);
    }
}

// 计算Vega
double SensitivityAnalysis::computeVega(CalculationMethod method) const {
    if (method == CalculationMethod::FiniteDifference) {
        return model.computeVegaFiniteDifference(spot, strike, maturity, volatility);
    } else {
        return model.computeVegaMonteCarlo(spot, strike, maturity, volatility);
    }
}

*/
