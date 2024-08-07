//
//  RateModel.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# ifndef RateModel_hpp
# define RateModel_hpp

# include <vector>

class Parameters;

class RateModel { // 基类
public:
    virtual ~RateModel() = default;
    virtual double getRate(const Parameters& params) const = 0;
};

class ConstantRateModel : public RateModel {    // 派生类
public:
    ConstantRateModel(const Parameters& params);
    virtual ~ConstantRateModel() = default;
    double getRate(const Parameters& params) const override;
};

class HullWhiteModel : public RateModel {    // 派生类
public:
    HullWhiteModel(const Parameters& params);
    virtual ~HullWhiteModel() = default;
    double getRate(const Parameters& params) const override;

private:
    double a_HWM_;
    double sigma_HWM_;
    double dt_;
};

# endif // RATEMODEL_HPP
