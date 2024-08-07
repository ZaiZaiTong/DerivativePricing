//
//  SensitivityAnalysis.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# ifndef SensitivityAnalysis_hpp
# define SensitivityAnalysis_hpp

class PricingModel;
class RateModel;
class VolatilityModel;

enum class CalculationMethod {
    FiniteDifference,
    MonteCarlo
};

class SensitivityAnalysis {
public:
    // 构造函数，初始化定价模型和期权参数
    SensitivityAnalysis(const PricingModel& model, double spot, double strike, double maturity, double volatility);

    // 计算Delta
    double computeDelta(CalculationMethod method) const;

    // 计算Gamma
    double computeGamma(CalculationMethod method) const;

    // 计算Vega
    double computeVega(CalculationMethod method) const;

private:
    const PricingModel& model;
    double spot;
    double strike;
    double maturity;
    double volatility;
};

# endif // SENSITIVITYANALYSIS_HPP


