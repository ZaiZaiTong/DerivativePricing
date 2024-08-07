//
//  MonteCarloSimulator.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//
// 并行运算
// 所有的函数尽量用&或*引入参数，用const防止变更。这样可以防止数据的临时复制，降低运行效率
// 从大类来说，衍生品定价有三个求解思路：显式解析解、有限差分法、蒙特卡罗模拟
// 三者的时间和空间复杂度依次递增（有限差分法FDM以衍生品价格的偏微分方程为出发点，通过计算而非模拟得到离散状态的定价，但是必须计算整张表格才能确定某一个点的定价。蒙特卡洛因为需要模拟多条路径而需要更多时间和空间，更适合亚式期权等复杂衍生品）。
// 如果能获得衍生品价格的方程（例如Delta对冲和无风险套利方法得到BSM偏微分方程），则应该采用FDM，而不是蒙特卡罗模拟。
// 分红股票期权、货币期权、商品期权、期货期权都可以推导出偏微分方程（不涉及定义具体的看涨看跌或者K和T，都可以用FDM。相关公式查看Notes。
// 显式解析解：涉及S二阶导数和t一阶导数的方程称为抛物型方程，又称为热传导方程或扩散方程。在高维模型中，尤其是引入了随机波动率的模型中，一般用傅立叶变换或拉普拉斯变换将偏微分方程的每个部分转换成频域形式，化简方程为只有dt的形式，在求解后再逆变换。得到最终解析解。除此之外还可以用格林函数求解。

// 待研究的几个点：
// 1. sobol可以用于生成均匀分布，相比于随机生成的数好像更稳定，收敛更快；
// 2. 并行运算还是要做，有很多库可以做thread等
// 3. 最新：已经做到MCS的修改了，需要增加独立的seed（每个模型对应一个），且seed需要外部输入
// 要对ratemodel volatility模型等都匹配一个独立的seed
// 4. 需要看下main怎么写
// 5. 模型的仅提供模型和一次性的计算dW dt；模拟提供重复运算和价格链条以及随机random。要不要让MCS
// 直接获得PricingModel的模型，让PricingModel把所有模型整合在一起？
// 6. 设置一个modelparams文档存储模型的名称到成员变量（并且模型的名称可以自动从ratemodel、volatility model、assetpricemodel中提取，不用我手动输入），三个模型文档中补充每个模型所需要的参数这个成员变量。modelparams中包括的操作有展示所有可用的模型名称、在输入模型后展示模型的全部需要的参数、当params中参数与当前三模型组合不匹配的时候返回缺失哪些参数


# include "MonteCarloSimulator.hpp"
# include "RateModel.hpp"
# include "PricingModel.hpp"
# include "VolatilityModel.hpp"
# include "AssetPriceModel.hpp"
# include "Parameters.hpp"
# include <random>
# include <iostream>
# include <functional>
# include <algorithm>

MonteCarloSimulator::MonteCarloSimulator(const Parameters& params, const PricingModel& pricingModel)
    : params_(params), pricingModel_(pricingModel), numSteps_(params.get<double>("numSteps")) {
    pricePaths_.resize(200, numSteps_ + 1);
    ratePaths_.resize(200, numSteps_ + 1);
    volatilityPaths_.resize(200, numSteps_ + 1);
}

double MonteCarloSimulator::get_random_number() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<> dis(0.0, 1.0);
    return dis(gen);
}

void MonteCarloSimulator::generate_paths() {
    double sqrt_dt = std::sqrt(params_.get<double>("dt"));
    Eigen::MatrixXd random_numbers1 = Eigen::MatrixXd::NullaryExpr(100, numSteps_, [&]() { return get_random_number(); });
    Eigen::MatrixXd antithetic_numbers1 = -random_numbers1;
    Eigen::MatrixXd dw_spot(200, numSteps_);
    dw_spot << sqrt_dt * random_numbers1, sqrt_dt * antithetic_numbers1;

    Eigen::MatrixXd random_numbers2 = Eigen::MatrixXd::NullaryExpr(100, numSteps_, [&]() { return get_random_number(); });
    Eigen::MatrixXd antithetic_numbers2 = -random_numbers2;
    Eigen::MatrixXd dw_volatility(200, numSteps_);
    dw_volatility << sqrt_dt * random_numbers2, sqrt_dt * antithetic_numbers2;
    
    Eigen::MatrixXd random_numbers3 = Eigen::MatrixXd::NullaryExpr(100, numSteps_, [&]() { return get_random_number(); });
    Eigen::MatrixXd antithetic_numbers3 = -random_numbers3;
    Eigen::MatrixXd dw_rate(200, numSteps_);
    dw_rate << sqrt_dt * random_numbers3, sqrt_dt * antithetic_numbers3;
    
    params_.set<double>("St", params_.get<double>("spot"));
    params_.set<double>("rt", params_.get<double>("rate"));
    params_.set<double>("vt", params_.get<double>("volatility"));

    pricePaths_.col(0).setConstant(params_.get<double>("St"));
    ratePaths_.col(0).setConstant(params_.get<double>("rt"));
    volatilityPaths_.col(0).setConstant(params_.get<double>("vt"));
    
    // 使用矩阵运算更新params_并计算路径
    for (int col = 1; col <= numSteps_; ++col) {
        // 获取矩阵的第 col 列和 resultMatrix 的第 col-1 列
        Eigen::VectorXd prev_price = pricePaths_.col(col - 1);
        Eigen::VectorXd current_dwprice = dw_spot.col(col - 1);

        // 使用 std::transform 和 lambda 函数计算 simulatePrice 结果并直接存储到结果矩阵中
        std::transform(prev_price.data(), prev_price.data() + prev_price.size(), current_dwprice.data(), pricePaths_.col(col).data(), [this](double prevspot, double currentspot) {
            params_.set<double>("St", prevspot); // spot
            params_.set<double>("dW_spot", currentspot);
            return pricingModel_.getAssetPriceModel().simulatePrice(params_);
        });

        Eigen::VectorXd prev_rate = ratePaths_.col(col - 1);
        Eigen::VectorXd current_dwrate = dw_rate.col(col - 1);

        // 使用 std::transform 和 lambda 函数计算 simulatePrice 结果并直接存储到结果矩阵中
        std::transform(prev_rate.data(), prev_rate.data() + prev_rate.size(), current_dwrate.data(), ratePaths_.col(col).data(), [this](double prevrate, double currentrate) {
            params_.set<double>("rt", prevrate); // rate
            params_.set<double>("dW_rate", currentrate);
            return pricingModel_.getRateModel().getRate(params_);
        });
        
        Eigen::VectorXd prev_volatility = volatilityPaths_.col(col - 1);
        Eigen::VectorXd current_dwvolatility = dw_volatility.col(col - 1);

        // 使用 std::transform 和 lambda 函数计算 simulatePrice 结果并直接存储到结果矩阵中
        std::transform(prev_volatility.data(), prev_volatility.data() + prev_volatility.size(), current_dwvolatility.data(), volatilityPaths_.col(col).data(), [this](double prevvolatility, double currentvolatility) {
            params_.set<double>("vt", prevvolatility); // volatility
            params_.set<double>("dW_volatility", currentvolatility);
            return pricingModel_.getVolatilityModel().getVolatility(params_);
        });
    }
}

const Eigen::MatrixXd& MonteCarloSimulator::get_price_paths() const {
    return pricePaths_;
}

const Eigen::MatrixXd& MonteCarloSimulator::get_rate_paths() const {
    return ratePaths_;
}

const Eigen::MatrixXd& MonteCarloSimulator::get_volatility_paths() const {
    return volatilityPaths_;
}

/*

1. Variance Reduction Techniques
减少方差技术可以提高模拟的效率和精度，常见的方法包括：
- Antithetic Variates: 通过同时使用正向和反向的随机变量来减少方差。每次模拟生成z，然后分别计算f(z)和f(-z)，将两个数据都放入终值数据集中。

### 2. **Quasi-Monte Carlo Methods**
使用准蒙特卡洛方法代替标准的蒙特卡洛方法，可以提高模拟的收敛速度：

- **Low Discrepancy Sequences**: 使用低差异序列（如Sobol序列、Halton序列等）生成样本，这些序列比标准的随机序列更均匀地覆盖样本空间。

### 3. **Parallel Computing**
利用并行计算可以显著加快蒙特卡洛模拟的速度：

- Multithreading: 在单台计算机上利用多线程并行执行模拟任务。
- **Distributed Computing**: 在多台计算机上分布式并行执行模拟任务，例如使用MPI或Hadoop等框架。
- **GPU Computing**: 利用图形处理器（GPU）的强大并行计算能力进行模拟计算，使用CUDA或OpenCL等技术。

4. Algorithmic Improvements
在算法层面进行优化，提升模拟效率：
- Early Stopping Rules: 根据预设的条件提前停止模拟，从而节省计算资源。
- **Adaptive Methods**: 动态调整模拟参数或抽样策略，以提高模拟效率。
- **Vectorization and SIMD**: 使用SIMD指令集进行向量化计算。
- **Efficient Random Number Generation**: 选择高效的随机数生成器，如PCG或Xoroshiro128+。

### 5. **Efficient Programming Techniques**
采用高效的编程技术和优化策略：

- Vectorization: 使用向量化操作代替循环，以利用CPU的向量处理能力。
- **Efficient Data Structures**: 使用高效的数据结构进行数值计算。
- 减少中间变量，让计算一次完成，减少变量。
- Transform:
 在性能方面，std::transform 和显式 for 循环通常是相似的，因为它们在底层都实现了元素的逐一遍历和处理。然而，std::transform 可能会略快一些，尤其是在启用了编译器优化的情况下，因为它是标准库函数，编译器对其进行了高度优化。

 要决定哪个更快，可以通过以下几点来考虑：

 编译器优化：标准库函数如 std::transform 通常会在编译时得到更好的优化。
 代码可读性：使用 std::transform 可以使代码更简洁和易读。
 并行化：C++17 引入了并行算法，可以用 std::transform 的并行版本 std::transform(std::execution::par, ...) 来利用多线程，提高性能。
 
*/
// Profiling and Optimization: 对代码进行性能分析，找出瓶颈并进行针对性优化。如何做
