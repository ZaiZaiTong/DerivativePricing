//
//  TransactionCost.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//
// 卖空股票存在利息费用时，可以得到另一个偏微分方程。当Delta>0时需要支付买入股票的费用。当Delta<0时需要支付融券的利息。

# include "TransactionCost.hpp"


// 零交易成本实现
double ZeroTransactionCost::getCost(double transactionSize) const {
    return 0.0;
}

// 固定交易成本实现
FixedTransactionCost::FixedTransactionCost(double cost) : cost(cost) {}

double FixedTransactionCost::getCost(double transactionSize) const {
    return cost;
}
