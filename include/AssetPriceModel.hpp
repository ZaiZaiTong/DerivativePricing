//
//  AssetPriceModel.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# ifndef AssetPriceModel_hpp
# define AssetPriceModel_hpp

class Parameters;

class AssetPriceModel {
public:
    virtual ~AssetPriceModel() = default;
    virtual double simulatePrice(const Parameters& params) const = 0;
};

class GeometricBrownianMotionModel : public AssetPriceModel {
public:
    GeometricBrownianMotionModel(const Parameters& params);
    virtual ~GeometricBrownianMotionModel() = default;
    double simulatePrice(const Parameters& params) const override;
private:
    double dt_;
};

class JumpDiffusionPriceModel : public AssetPriceModel {
public:
    JumpDiffusionPriceModel(const Parameters& params);
    virtual ~JumpDiffusionPriceModel() = default;
    double simulatePrice(const Parameters& params) const override;

private:
    double dt_;
    double jumpMean_JDPM_;      // class定义时需要输入的参数，此外的参数都是需要后续外部引入的
    double jumpVol_JDPM_;
    double jumpIntensity_JDPM_;
    double jumpSize_JDPM_;
    // double dW_JDPM2_;
};

# endif // ASSETPRICEMODEL_HPP
