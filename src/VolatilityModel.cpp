//
//  VolatilityModel.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//
// 所有的模型都需要核对计算公式是否正确
// 可以用各种方法判断当前散户在一只股票中的占比（例如ETF规模或者成交单的模式等），进而判断当前股价的波动率大小以及对风险的敏感性
//
//一般所说的记忆指的是对超过当期数据的更久远的数据的依赖，如果只依赖当期数据则为马尔科夫性质。大多数金融模型都具有马尔科夫性质。

/*
| Model                     | Description                                                                             | Advantages                                                           | Disadvantages                                                              |
|---------------------------|-----------------------------------------------------------------------------------------|----------------------------------------------------------------------|----------------------------------------------------------------------------|
| Constant Volatility Model | Assumes constant volatility over time.                                                  | Simplicity, analytical tractability.                     | Ignores volatility clustering, unrealistic.                                |
| Local Volatility Model    | Volatility is a function of underlying asset price and time.                            | Fits market prices well, captures smile.                            | Complex calibration, computationally intensive.                            |
| Heston Model              | Stochastic volatility model with mean-reverting volatility.                             | Captures volatility clustering, closed-form solution for options. | Parameter estimation can be complex.                                       |
| SABR Model                | Stochastic volatility model with a CEV process for the underlying.                      | Captures skew and smile effects well.                             | Calibration can be complex, parameter sensitivity.                         |
| GARCH Model               | Generalized Autoregressive Conditional Heteroskedasticity, volatility as a time series. | Captures volatility clustering, widely used in finance.           | Requires historical data, assumes past patterns predict future volatility. |
| Jump-Diffusion Model      | Incorporates jumps in asset prices to model sudden movements.                           | Captures sudden market movements, realistic for certain assets.     | Complex calibration, less tractable analytically.                          |

*/

# include "VolatilityModel.hpp"
# include <cmath>
# include <random>
# include "Parameters.hpp"

// ConstantVolatilityModel implementation
ConstantVolatilityModel::ConstantVolatilityModel(const Parameters& params) {}

double ConstantVolatilityModel::getVolatility(const Parameters& params) const {
    return params.get<double>("vt");
}

// HestonModel implementation
HestonModel::HestonModel(const Parameters& params)
    : kappa_HM_(params.get<double>("kappa_HM")), theta_HM_(params.get<double>("theta_HM")), xi_HM_(params.get<double>("xi_HM")), rho_HM_(params.get<double>("rho_HM")), dt_(params.get<double>("dt")) {}

double HestonModel::getVolatility(const Parameters& params) const {
    double volatility = params.get<double>("vt") + kappa_HM_ * (theta_HM_ - params.get<double>("vt")) * dt_ + xi_HM_ * std::sqrt(params.get<double>("vt")) * params.get<double>("dW_volatilit");
    return volatility;
}

// SABRModel implementation
SABRModel::SABRModel(const Parameters& params)
    : alpha_SABRM_(params.get<double>("alpha_SABRM")), beta_SABRM_(params.get<double>("beta_SABRM")), rho_SABRM_(params.get<double>("rho_SABRM")), nu_SABRM_(params.get<double>("nu_SABRM")), dt_(params.get<double>("dt")) {}

double SABRModel::getVolatility(const Parameters& params) const {
    return alpha_SABRM_ * std::pow(dt_, beta_SABRM_) * std::exp(nu_SABRM_ * params.get<double>("dW_volatility"));
}

// GARCH模型实现
GARCHModel::GARCHModel(const Parameters& params)
    : alpha0_GARCHM_(params.get<double>("alpha0_GARCHM")), alpha1_GARCHM_(params.get<double>("alpha1_GARCHM")), beta_GARCHM_(params.get<double>("beta_GARCHM")),  dt_(params.get<double>("dt")) {}

double GARCHModel::getVolatility(const Parameters& params) const {
    double vol = std::sqrt(alpha0_GARCHM_ + alpha1_GARCHM_ * std::pow(params.get<double>("dW_volatility"), 2) + beta_GARCHM_ * std::pow(params.get<double>("vt"), 2)); // 示例实现
    return vol;
}

// JumpDiffusionModel implementation
JumpDiffusionModel::JumpDiffusionModel(const Parameters& params)
    : jumpMean_JDM_(params.get<double>("jumpMean_JDM")), jumpVol_JDM_(params.get<double>("jumpVol_JDM")), dt_(params.get<double>("dt")) {}

double JumpDiffusionModel::getVolatility(const Parameters& params) const {
    return params.get<double>("vt") * (1 + jumpMean_JDM_ * dt_) + jumpVol_JDM_ * params.get<double>("dW_volatility");
}


