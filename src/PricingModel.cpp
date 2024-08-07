//
//  PricingModel.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

// 定价模型的组合设置，同时用于底层调用相关模型的名称。如果功能不增加的话，应该也不用修改了
// 单资产期权、多资产期权
// 路径依赖衍生品、障碍期权、亚式期权、回溯期权、远期开始期权、呐喊期权、有封顶的回溯和亚式期权、回溯亚式期权、波动率期权、相关性互换、阶梯期权、巴黎期权、棘轮期权
// 权证：执行时公司发行新股
// 交易型看涨期权：执行时持有者收到已经发行的股票

# include "PricingModel.hpp"
# include <cmath>
# include <algorithm>
# include <iostream>
# include "AssetPriceModel.hpp"
// # include "MonteCarloSimulator.hpp"
# include "VolatilityModel.hpp"
# include "RateModel.hpp"
# include "ExecutionStyle.hpp"
# include "TransactionCost.hpp"
# include "DividendYield.hpp"
# include "BoundaryConditions.hpp"
# include "Parameters.hpp"
# include "Payoff.hpp"

// 构造函数实现。类的名称::构造函数名称
PricingModel::PricingModel(const RateModel& rateModel,
                           const VolatilityModel& volModel,
                           const AssetPriceModel& assetModel,
                           //const ExecutionStyle& executionStyle,
                           const Payoff& payoff,
                           const TransactionCost& transactionCost = ZeroTransactionCost()
// 有default value的参数必须放在最后面
                           //const DividendYield& dividendYield,
                           //const BoundaryConditions& boundaryConditions,
                           )
    : rateModel_(rateModel),
      volModel_(volModel),
      assetModel_(assetModel),
      //executionStyle_(executionStyle),
      transactionCost_(transactionCost),
      //dividendYield_(dividendYield),
      //boundaryConditions_(boundaryConditions),
      payoff_(payoff) {}

const Payoff& PricingModel::getPayoff() const {
    return payoff_;
}

const RateModel& PricingModel::getRateModel() const {
    return rateModel_;
}

const VolatilityModel& PricingModel::getVolatilityModel() const {
    return volModel_;
}

const AssetPriceModel& PricingModel::getAssetPriceModel() const {
    return assetModel_;
}

/*const ExecutionStyle& PricingModel::getExecutionStyle() const {
    return executionStyle_;
}*/

const TransactionCost& PricingModel::getTransactionCost() const {
    return transactionCost_;
}
/*
const DividendYield& PricingModel::getDividendYield() const {
    return dividendYield_;
}
*/
/*
// 使用有限差分法计算Delta
double PricingModel::computeDeltaFiniteDifference(double spot, double strike, double maturity, double volatility) const {
    double epsilon = 0.01;
    double priceUp = assetModel_.simulatePrice(spot + epsilon, rateModel_.getRate(maturity), volatility, maturity);
    double priceDown = assetModel_.simulatePrice(spot - epsilon, rateModel_.getRate(maturity), volatility, maturity);
    return (priceUp - priceDown) / (2 * epsilon);
}

// 使用有限差分法计算Gamma
double PricingModel::computeGammaFiniteDifference(double spot, double strike, double maturity, double volatility) const {
    double epsilon = 0.01;
    double priceUp = assetModel_.simulatePrice(spot + epsilon, rateModel_.getRate(maturity), volatility, maturity);
    double priceDown = assetModel_.simulatePrice(spot - epsilon, rateModel_.getRate(maturity), volatility, maturity);
    double price = assetModel_.simulatePrice(spot, rateModel_.getRate(maturity), volatility, maturity);
    return (priceUp - 2 * price + priceDown) / (epsilon * epsilon);
}

// 使用有限差分法计算Vega
double PricingModel::computeVegaFiniteDifference(double spot, double strike, double maturity, double volatility) const {
    double epsilon = 0.01;
    double priceUp = assetModel_.simulatePrice(spot, rateModel_.getRate(maturity), volatility + epsilon, maturity);
    double priceDown = assetModel_.simulatePrice(spot, rateModel_.getRate(maturity), volatility - epsilon, maturity);
    return (priceUp - priceDown) / (2 * epsilon);
}

// 使用蒙特卡洛方法计算Delta
double PricingModel::computeDeltaMonteCarlo(double spot, double strike, double maturity, double volatility) const {
    int numPaths = 10000;
    double epsilon = 0.01;
    double priceUp = 0.0, priceDown = 0.0;

    for (int i = 0; i < numPaths; ++i) {
        double pathUp = assetModel_.simulatePrice(spot + epsilon, rateModel_.getRate(maturity), volatility, maturity);
        double pathDown = assetModel_.simulatePrice(spot - epsilon, rateModel_.getRate(maturity), volatility, maturity);
        priceUp += pathUp;
        priceDown += pathDown;
    }

    priceUp /= numPaths;
    priceDown /= numPaths;

    return (priceUp - priceDown) / (2 * epsilon);
}

// 使用蒙特卡洛方法计算Gamma
double PricingModel::computeGammaMonteCarlo(double spot, double strike, double maturity, double volatility) const {
    int numPaths = 10000;
    double epsilon = 0.01;
    double priceUp = 0.0, priceDown = 0.0, price = 0.0;

    for (int i = 0; i < numPaths; ++i) {
        double pathUp = assetModel_.simulatePrice(spot + epsilon, rateModel_.getRate(maturity), volatility, maturity);
        double pathDown = assetModel_.simulatePrice(spot - epsilon, rateModel_.getRate(maturity), volatility, maturity);
        double path = assetModel_.simulatePrice(spot, rateModel_.getRate(maturity), volatility, maturity);
        priceUp += pathUp;
        priceDown += pathDown;
        price += path;
    }

    priceUp /= numPaths;
    priceDown /= numPaths;
    price /= numPaths;

    return (priceUp - 2 * price + priceDown) / (epsilon * epsilon);
}

// 使用蒙特卡洛方法计算Vega
double PricingModel::computeVegaMonteCarlo(double spot, double strike, double maturity, double volatility) const {
    int numPaths = 10000;
    double epsilon = 0.01;
    double priceUp = 0.0, priceDown = 0.0;

    for (int i = 0; i < numPaths; ++i) {
        double pathUp = assetModel_.simulatePrice(spot, rateModel_.getRate(maturity), volatility + epsilon, maturity);
        double pathDown = assetModel_.simulatePrice(spot, rateModel_.getRate(maturity), volatility - epsilon, maturity);
        priceUp += pathUp;
        priceDown += pathDown;
    }

    priceUp /= numPaths;
    priceDown /= numPaths;

    return (priceUp - priceDown) / (2 * epsilon);
}
*/
