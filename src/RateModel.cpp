//
//  RateModel.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//
// 所有的模型都需要核对计算公式是否正确
// 当r D sigma单纯是t的函数时，BSM偏微分方程仍完全成立。
/*
 | Model                 | Description                                                        | Advantages                                                    | Disadvantages                                                |
 |-----------------------|--------------------------------------------------------------------|---------------------------------------------------------------|--------------------------------------------------------------|
 | Vasicek Model         | Describes interest rate movements using a mean-reverting process.  | Simple, mean-reversion property.                              | Can produce negative interest rates.                         |
 | CIR Model             | Extends Vasicek by ensuring non-negative rates.                    | Mean-reversion, non-negative rates.                    | More complex calibration.                                    |
 | Ho-Lee Model          | Arbitrage-free model using normal distribution.                    | Simple, arbitrage-free.                        | Can produce negative rates, unrealistic long-term behaviors. |
 | Hull-White Model      | Extends Vasicek for better calibration.                            | Good fit to market data, flexible.                        | Can produce negative rates, more complex.                    |
 | Black-Karasinski Model| Log-normal distribution for interest rates.                        | Allows for log-normal distribution, non-negative rates.      | Complex calibration, less tractable analytically.            |
 | Libor Market Model    | Models forward rates using a log-normal process.                   | Directly models market-observed rates, arbitrage-free. | High dimensionality, complex calibration.                    |
 | Longstaff-Schwartz Model | Multi-factor model capturing stochastic behavior of short rates. | Captures term structure dynamics well.               | Requires complex numerical methods.                          |
 | HJM Model             | Describes entire yield curve dynamics.                              | Flexibility in capturing yield curve dynamics.        | High dimensionality, computationally intensive.              |

*/

# include "RateModel.hpp"
# include <cmath>
# include <random>
# include "Parameters.hpp"

// 恒定利率模型实现
ConstantRateModel::ConstantRateModel(const Parameters& params) {}

double ConstantRateModel::getRate(const Parameters& params) const {
    return params.get<double>("rt");
}

// Vasicek Model 均值回归随机游走，适用于短期利率，横盘随机游走
// dr = (v - gamma * r)dt + sigma dW      gamma 是回归速率，v / gamma是均值率
// return v + (r - v) * exp(- gamma * t) + sigma(Wt - gamma 积分0～t{e^(gamma * (s - t)) * W(s) ds} )

// Cox Ingersoll Ross CIR model dr = (v - gamma * r)dt + sigma r^(1/2) dW             没有解析解的时候则可以采用这里的式子直接用欧拉马尔科夫方法模拟

// Hull-White模型实现
HullWhiteModel::HullWhiteModel(const Parameters& params)
    : a_HWM_(params.get<double>("a_HWM")), sigma_HWM_(params.get<double>("sigma_HWM")), dt_(params.get<double>("dt")) {}

double HullWhiteModel::getRate(const Parameters& params) const {
    return params.get<double>("rt") * std::exp(-a_HWM_ * dt_ + sigma_HWM_ * params.get<double>("dW_rate")); // 示例实现
}


// 1. ratemodel是否做dt模型
// 2. MCS中delta gamma vega theta rho的编写。以及是否加入pricepath
// 3. main函数编写
// 4. 实际上可以构造更复杂的模型，例如HWM的sigma用GARCH等模型模拟，这个模型可以作为新的模型输入
// 而不用考虑模型之间的不断嵌套
