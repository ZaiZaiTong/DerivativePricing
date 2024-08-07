//
//  HedgeStrategy.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# ifndef HedgeStrategy_hpp
# define HedgeStrategy_hpp

// 抽象对冲策略基类
class HedgeStrategy {
public:
    virtual ~HedgeStrategy() = default;
    virtual double hedge(double spotPrice, double optionPrice, double delta, double gamma) const = 0;
};

// Delta对冲策略
class DeltaHedge : public HedgeStrategy {
public:
    double hedge(double spotPrice, double optionPrice, double delta, double gamma) const override;
};

// Delta-Gamma对冲策略
class DeltaGammaHedge : public HedgeStrategy {
public:
    double hedge(double spotPrice, double optionPrice, double delta, double gamma) const override;
};

// 本地对冲策略
class LocalHedge : public HedgeStrategy {
public:
    double hedge(double spotPrice, double optionPrice, double delta, double gamma) const override;
};

// 动态对冲策略
class DynamicHedge : public HedgeStrategy {
public:
    double hedge(double spotPrice, double optionPrice, double delta, double gamma) const override;
};

# endif // HEDGESTRATEGY_HPP
