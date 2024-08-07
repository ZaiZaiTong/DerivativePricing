//
//  VolatilityModel.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# ifndef VolatilityModel_hpp
# define VolatilityModel_hpp

# include <iostream>              // 避免在hpp中用标准库，否则应用前向声明、guard pragma等防止重复包含
# include <random>
# include <cmath>

class Parameters;

class VolatilityModel {
public:
    virtual ~VolatilityModel() = default;
    virtual double getVolatility(const Parameters& params) const = 0;
};

class ConstantVolatilityModel : public VolatilityModel {
public:
    explicit ConstantVolatilityModel(const Parameters& params);
    double getVolatility(const Parameters& params) const override;
};

class HestonModel : public VolatilityModel {
public:
    HestonModel(const Parameters& params);
    double getVolatility(const Parameters& params) const override;

private:
    double kappa_HM_;
    double theta_HM_;
    double xi_HM_;
    double rho_HM_;
    double dt_;
};

class SABRModel : public VolatilityModel {
public:
    SABRModel(const Parameters& params);
    double getVolatility(const Parameters& params) const override;

private:
    double alpha_SABRM_;
    double beta_SABRM_;
    double rho_SABRM_;
    double nu_SABRM_;
    double dt_;
};

class GARCHModel : public VolatilityModel {
public:
    GARCHModel(const Parameters& params);
    double getVolatility(const Parameters& params) const override;

private:
    double alpha0_GARCHM_;
    double alpha1_GARCHM_;
    double beta_GARCHM_;
    double dt_;
};

class JumpDiffusionModel : public VolatilityModel {
public:
    JumpDiffusionModel(const Parameters& params);
    double getVolatility(const Parameters& params) const override;

private:
    double jumpMean_JDM_;
    double jumpVol_JDM_;
    double dt_;
};

# endif // VOLATILITYMODEL_HPP
