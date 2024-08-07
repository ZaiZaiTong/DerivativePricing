//
//  HedgeStrategy.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
// 包含一切降低不确定性的行为
// 限制交易次数、限制交易时间间隔、市场流动性不足
// 静态对冲、动态对冲（随着时间不断改变对冲比率的一种方法）
// Binary options or Digital options: 当预期标的资产价格上涨或者下跌的幅度不会很大的时候，可以购买二值期权。在标的资产价格大幅变动的情况下，Vanilla的杠杆比二值期权的杠杆更大。
// Bull spread or Call spread: 相同到期日，买入低K CO，卖出高K CO。回报近似于二值期权，当预期标的资产价格上涨不会很大的时候，可以用更低的成本获得相同的收益。
// Bear spread or Put spread: 相同到期日，卖出低K PO，买入高K PO。
// Straddle: 相同到期日，买入相同K的CO和PO。在重大信息公布前，买入跨式期权意味着预期标的资产价格要么上涨要么下跌，而不会停留在原值。正常的涨跌应该已经被定价，超涨超跌且覆盖成本才能获利。主要是波动率交易者的投资品。很少被持有到期。
// Strangle: 相同到期日，买入不同K的CO和PO，买入低K PO，卖出高K CO。宽跨式期权一般在标的资产价格处于两个K之间时买入，价格比straddle更低，即杠杆更大。主要是波动率交易者的投资品。很少被持有到期。押注价格稳定性则卖出两个期权。
// Risk reversal: 买入相同到期日的高于St的CO并卖出低于St的PO。风险逆转组合价值通常非常小，与波动率变动的市场预期有关。
// Butterfly spread: 相同到期日，买入高K和低K的两份CO，卖出2份中间K的CO。头寸相对便宜，盈利潜力也比较低，Payoff三角形，押注资产价格不会上涨也不会下跌。
// Condor:类似butterfly spread，相同到期日，但是用四种不同K的CO组合。回报图Payoff呈现梯形。
//
// 对冲频率一般取决于对冲成本，成本越低对冲越频繁，离散对冲也会影响衍生品定价
// 买卖价差和交易成本也会影响衍生品定价
//
// 崩盘对冲Crash hedging或白金对冲Platinum hedging：在市场崩盘期间，市场变动大且迅速，凸性的影响增大，同时通常所有的市场相关性都会变成1或者-1。通常有两种形式，组合的账面价值对冲和保证金需求对冲。
//
// 瞬时Delta和Gamma都等于0，可以减少每次对冲的规模，并增加再次对冲的时间间隔降低成本。
// 瞬时Delta和Vega都等于0，可以对冲不易估计的波动率。一般用常数波动率模型估计敏感性Vega，这种方法在实践中比较令人满意，但是理论上自相矛盾。可以考虑把常数参数作为自变量建模（即像St一样有模型可变），则没有矛盾出现。

# include "HedgeStrategy.hpp"

// Delta对冲策略的实现
double DeltaHedge::hedge(double spotPrice, double optionPrice, double delta, double gamma) const {
    return -delta * spotPrice;
}

// Delta-Gamma对冲策略的实现
double DeltaGammaHedge::hedge(double spotPrice, double optionPrice, double delta, double gamma) const {
    return -delta * spotPrice - 0.5 * gamma * spotPrice * spotPrice;
}

// 本地对冲策略的实现
double LocalHedge::hedge(double spotPrice, double optionPrice, double delta, double gamma) const {
    return -delta * spotPrice - gamma * spotPrice * spotPrice;
}

// 动态对冲策略的实现
double DynamicHedge::hedge(double spotPrice, double optionPrice, double delta, double gamma) const {
    // 这里我们可以实现更加复杂的动态对冲策略
    return -delta * spotPrice - 0.5 * gamma * spotPrice * spotPrice + 0.1 * (optionPrice - spotPrice);
}
