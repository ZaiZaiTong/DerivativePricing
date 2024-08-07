//
//  DividendYield.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//
// 随机分红模型、固定分红、柏松跳跃模型
// 通常美国股票市场的红利每个季度发放一次。英国则是每半年或每个季度发放一次。红利数量是按照美元支付的，和股价是没有关系的。并且只有在分红前一小段时间才可知。分红是离散的，而不是连续的。
// 假设：红利数量已知，且为付息日当天某些变量的函数（例如资产价值等）；付息日已知，且红利在支付日离散地支付。

# include "DividendYield.hpp"

// 固定股息收益率实现
FixedDividendYield::FixedDividendYield(double yield) : yield(yield) {}

double FixedDividendYield::getYield(double time) const {
    return yield;
}
