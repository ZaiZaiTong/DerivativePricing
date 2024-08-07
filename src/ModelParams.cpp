//
//  ModelParams.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/11.
//
// 基本完成，后续添加新模型时需要修改
// 增加每个名称对应什么模型以及每个变量代表什么的注释
// template的推广，同时增加注释
// 还要补充Payoff需要的各种参数

# include "ModelParams.hpp"
# include "Parameters.hpp"

ModelParams::ModelParams() {
    initialize();
}

void ModelParams::initialize() {
    // 手动输入模型名称和对应的模型参数 此处是新增模型出现时需要修改的地方之一
    // 这里所有的参数都必须是用户手动输入的，防止缺失的情况，不能包括dW等
    // 增加每个名称对应什么模型以及每个变量代表什么的注释
    modelParams["ConstantRateModel"] = {"rate"};
    modelParams["HullWhiteModel"] = {"rate", "a_HWM", "sigma_HWM", "dt"};
    modelParams["GeometricBrownianMotionModel"] = {"spot", "dt"};
    modelParams["JumpDiffusionPriceModel"] = {"spot", "dt", "jumpVol_JDPM", "jumpMean_JDPM"};
    modelParams["ConstantVolatilityModel"] = {"volatility"};
    modelParams["HestonModel"] = {"volatility", "kappa_HM", "theta_HM", "xi_HM", "rho_HM", "dt"};
    modelParams["SABRModel"] = {"alpha_SABRM", "beta_SABRM", "rho_SABRM", "nu_SABRM","dt"};
    modelParams["GARCHModel"] = {"alpha0_GARCHM", "alpha1_GARCHM", "beta_GARCHM", "volatility", "dt"};
    modelParams["JumpDiffusionModel"] = {"volatility", "jumpMean_JDM", "jumpVol_JDM", "dt"};
}

std::vector<std::string> ModelParams::getAvailableModels() const {
    std::vector<std::string> models;
    for (const auto& pair : modelParams) {
        models.push_back(pair.first);
    }
    return models;
}

std::vector<std::string> ModelParams::getModelParams(const std::string& modelName) const {
    auto it = modelParams.find(modelName);
    if (it == modelParams.end()) {
        throw std::runtime_error("Model not found: " + modelName);
    }
    return it->second;
}

std::vector<std::string> ModelParams::checkParams(const std::string& modelName, const Parameters& params) const {
    auto it = modelParams.find(modelName);
    if (it == modelParams.end()) {
        throw std::runtime_error("Model not found: " + modelName);
    }
    const auto& requiredParams = it->second;
    std::vector<std::string> missingParams;
    for (const auto& param : requiredParams) {
        if (!params.contains(param)) {
            missingParams.push_back(param);
        }
    }
    return missingParams;
}
