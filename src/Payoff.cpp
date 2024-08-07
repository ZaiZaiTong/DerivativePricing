//
//  Payoff.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/11.
//
// 应该尽量保证payoff返回的值都是期末的，而不能随意设置为期中和期末都存在等情况
// 直接使用Parameters中参数计算payoff。还需要增加更多payoff模型，此外不需要改动
// 在资产价格大于一些价格水平后触发相应的额外期权费支付
// 对于t的一阶导数和St的二阶导数在偏微分方程等式的同一侧时有相同符号，则为后向方程，此时必须给出终值条件final condition，而不是初值条件initial condition。
//
// 奇异期权的分类依据：时变性（例如特定时间点行权，如百慕大期权）、现金流（离散或连续，根据合约规定定期或连续支付给持有者一定现金流，对应着跳跃条件。如果是连续支付，则不需要跳跃条件而是对BSM进行修改，增加红利率项）、路径依赖性（强或弱、离散或连续）、维度（多因子如双标的资产、强路径依赖如增加标的资产平均价格的新变量）、阶数（香草期权、路径依赖期权都属于一阶的，如果期权回报依赖于另一个期权价值，则为二阶的。高阶期权对基础定价模型更加敏感）、嵌入式抉择（例如可转债或美式期权等，一般对应着FDM方法定价。对于合约持有人来说，目标是让期权价值对Delta对冲的期权卖方来说尽可能高，搜索持有人所有可能抉择策略中让期权价值最大化的一个）
// 应该在很多Payoff前面加上系数，在后面加上常数。这样可以用于为某些倍数支付型衍生品定价。

# include "Payoff.hpp"
# include "Parameters.hpp"
# include <algorithm>
# include <numeric>
# include <cfloat>

// 欧式看涨期权
EuropeanCallPayoff::EuropeanCallPayoff() {}

Eigen::VectorXd EuropeanCallPayoff::operator()(const Parameters& params, const Eigen::MatrixXd& paths) const {
    Eigen::VectorXd finalPrices = paths.col(paths.cols() - 1);
    Eigen::VectorXd payoffs = (finalPrices.array() - params.get<double>("strike")).max(0.0);
    return payoffs;
}

std::string EuropeanCallPayoff::getName() const {
    return "EuropeanCallPayoff";
}

// 欧式看跌期权
EuropeanPutPayoff::EuropeanPutPayoff() {}

Eigen::VectorXd EuropeanPutPayoff::operator()(const Parameters& params, const Eigen::MatrixXd& paths) const {
    Eigen::VectorXd finalPrices = paths.col(paths.cols() - 1);
    Eigen::VectorXd payoffs = (params.get<double>("strike") - finalPrices.array()).max(0.0);
    return payoffs;
}

std::string EuropeanPutPayoff::getName() const {
    return "EuropeanPutPayoff";
}

// 障碍期权：弱路径依赖。在触及障碍前期权仍然可以正常对冲并获得无风险收益，所以满足BSM方程。仅仅是BSM成立的边界条件有差异。
// 对未来趋势有判断，但是认为变动幅度有限，不愿为全部变化幅度付费，于是会选择障碍期权。

// BarrierPayoff 构造函数
BarrierPayoff::BarrierPayoff() {}

// operator() 函数
Eigen::VectorXd BarrierPayoff::operator()(const Parameters& params, const Eigen::MatrixXd& paths) const {
    std::string payoffType = params.get<std::string>("payoff");
    double barrier = params.get<double>("barrier");
    bool isUpIn = params.get<bool>("isUpIn");
    bool isDownOut = params.get<bool>("isDownOut");
    bool isUpOut = params.get<bool>("isUpOut");
    bool isDownIn = params.get<bool>("isDownIn");

    // 根据payoff类型创建对应的对象
    std::unique_ptr<Payoff> payoffTypeObj;
    if (payoffType == "EuropeanCallPayoff") {
        payoffTypeObj = std::make_unique<EuropeanCallPayoff>();
    } else if (payoffType == "EuropeanPutPayoff") {
        payoffTypeObj = std::make_unique<EuropeanPutPayoff>();
    } else {
        throw std::runtime_error("Unknown payoff type");
    }

    // 确定每条路径是否满足障碍条件
    Eigen::Array<bool, Eigen::Dynamic, 1> barrierBreached;
    if (isUpIn || isUpOut) {
        barrierBreached = (paths.rowwise().maxCoeff().array() >= barrier);
    } else if (isDownIn || isDownOut) {
        barrierBreached = (paths.rowwise().minCoeff().array() <= barrier);
    }

    // 创建一个条件掩码，选择符合条件的路径
    Eigen::Array<bool, Eigen::Dynamic, 1> conditionsMet(paths.rows());
    if (isUpIn) {
        conditionsMet = barrierBreached;
    } else if (isDownOut) {
        conditionsMet = !barrierBreached;
    } else if (isUpOut) {
        conditionsMet = !barrierBreached;
    } else if (isDownIn) {
        conditionsMet = barrierBreached;
    }

    // 过滤符合条件的路径
    Eigen::MatrixXd selectedPaths(paths.rows(), paths.cols());
    long selectedCount = (conditionsMet == true).count();
    Eigen::MatrixXd filteredPaths(selectedCount, paths.cols());
    int index = 0;
    for (int i = 0; i < paths.rows(); ++i) {
        if (conditionsMet(i)) {
            filteredPaths.row(index++) = paths.row(i);
        }
    }

    // 计算符合条件的路径的payoff
    Eigen::VectorXd payoffValues(paths.rows());
    Eigen::VectorXd selectedPayoffs = Eigen::VectorXd::Zero(selectedCount);
    if (selectedCount > 0) {
        selectedPayoffs = (*payoffTypeObj)(params, filteredPaths);
    }

    // 使用条件掩码恢复payoffValues
    int payoffsIndex = 0;
    for (int i = 0; i < paths.rows(); ++i) {
        if (conditionsMet(i)) {
            payoffValues(i) = selectedPayoffs(payoffsIndex++);
        } else {
            payoffValues(i) = 0.0;
        }
    }

    return payoffValues;
}

// getName() 函数
std::string BarrierPayoff::getName() const {
    return "BarrierPayoff";
}


// out option敲出期权：到期日前没达到障碍水平才产生支付。通常敲出期权存在部分退款rebate，在障碍被触及时产生作为补偿。
// in option敲入期权：到期日前达到障碍水平才产生支付。敲出和敲入期权的和是一个普通期权（例如UpIn UpOut组成普通期权，当不考虑Out期权一般存在补偿的情况下），敲入期权是一个二阶合约，如果用二者做差求敲入期权价值，则需要两次模拟。
// 根据障碍高于还是低于标的资产的初始价值，分为向上期权和向下期权。同时障碍是可以时变的，通常障碍是时间的分段常数函数。障碍的参数取值应该保持可以输入的状态。。如果有两个障碍，则称为double barrier。
// 受保护pretected或部分partial障碍期权：存在障碍有效区间（仅在此时间区间上存在障碍）。第一种是当资产价格在障碍有效的时段超过障碍时就会触发，即真实超越障碍的时间点可以在有效区间外或者任何地方，只要在有效区间内满足条件就触发。第二种要求必须在有效区间内上穿或者下穿障碍才算满足触发条件，否则即便一直都高于障碍（假设Up情况），也不算满足触发条件。
// 双障碍期权可以要求两个障碍都触发才触发。
// roll-up roll-down滚动期权：在某个时间点之前触及障碍则得到新的障碍期权，在时点后触及则得到普通期权等。
// outside or rainbow barrier option: 回报和触发分别依赖于不同的资产，属于多因子合约。
// soft barrier option: 设置两个障碍，根据超过一个障碍的数额占两个障碍差值的比重确定敲出或者敲入多少期权。
// 障碍期权的定价方法是不统一的，流动性不高，买卖差价比较大。

// 亚式期权：强路径依赖
AsianPayoff::AsianPayoff() {}

Eigen::VectorXd AsianPayoff::operator()(const Parameters& params, const Eigen::MatrixXd& paths) const {
    // 这里的计算转移到MCS中double avgSpot = std::accumulate(spots_.begin(), spots_.end(), 0.0) / spots_.size();
    Eigen::VectorXd avgSpots = paths.rowwise().mean();
    Eigen::VectorXd payoffs = (avgSpots.array() - params.get<double>("strike")).max(0.0);
    return payoffs;
}

std::string AsianPayoff::getName() const {
    return "AsianPayoff";
}

// 回溯期权
LookbackPayoff::LookbackPayoff() {} // : minSpot_(DBL_MAX), maxSpot_(DBL_MIN)

Eigen::VectorXd LookbackPayoff::operator()(const Parameters& params, const Eigen::MatrixXd& paths) const {
    Eigen::VectorXd maxSpots = paths.rowwise().maxCoeff();
    Eigen::VectorXd payoffs = (maxSpots.array() - params.get<double>("strike")).max(0.0);
    return payoffs;
}

std::string LookbackPayoff::getName() const {
    return "LookbackPayoff";
}

// 两值看涨期权
// H(S - K)

// 两值看跌期权
// Heaviside function在自变量为负时取0，为正时取1。H(K - S)




// 巴黎期权：在资产价格超过障碍达到指定时间长度才会触发。


// 所有期权的定价一定仍然用风险中性测度计算。但是对于障碍期权到期日前被触发的可能性应该用真实资产价格随机过程计算概率。


