//
//  Pricing.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/14.
//
// 调用MCS三个链条进行定价
// 折现因子的计算需要匹配对应的RateModel。对于HWM等r随时间变化的模型，折现的r是历史所有r的某种积分。

# include "Pricing.hpp"
# include "MonteCarloSimulator.hpp"
# include "Payoff.hpp"
# include "Parameters.hpp"
# include "PricingModel.hpp"
# include <unordered_map>
# include <functional>
# include <memory>
# include <numeric>
# include <cmath>
# include <iostream>
# include <algorithm>
# include <boost/math/distributions/normal.hpp>
# include <thread>
# include <mutex>

double Pricing::calculate_mean(const Eigen::VectorXd& values) {
    return values.mean();
}

double Pricing::calculate_stdev(const Eigen::VectorXd& values, double mean) {
    Eigen::VectorXd diff = values.array() - mean;
    double variance = diff.squaredNorm() / (values.size() - 1);
    return std::sqrt(variance);
}

double Pricing::get_z_value(double confidence_level) {
    boost::math::normal_distribution<> dist(0, 1);
    return boost::math::quantile(dist, 1 - (1 - confidence_level) / 2);
}

bool Pricing::is_converged(const std::vector<Eigen::VectorXd>& chains, double tolerance) {
    double R_hat = calculate_gelman_rubin(chains);
    std::cout << "Gelman-Rubin 统计量: " << R_hat << std::endl;
    return std::abs(R_hat - 1) < tolerance;
}
// 静态成员函数可以通过类名直接调用而不需要类的实例。只要函数不依赖类的任何实例，则可以将其定义为静态成员函数static
// 在研究蒙特卡罗模拟的收敛性时，以下是一些重要的方法和标准：
// 1. Gelman-Rubin Statistic：这种方法通常用于检查Markov Chain Monte Carlo (MCMC)方法的收敛性。Gelman-Rubin统计量通过比较多条独立链的方差来评估收敛性。当各链方差之间的差异减小到接近于1时，表示模拟达到了收敛。
// 2. Trace Plots：绘制多个MCMC链的轨迹图也是一种常用的视觉方法，通过观察轨迹图是否稳定，可以判断是否达到了收敛。多个起始点的轨迹图最终收敛到相同的分布区域，说明模拟收敛良好。
// 3. Effective Sample Size：计算有效样本量也能帮助判断收敛性。有效样本量反映了模拟中独立样本的数量。如果ESS较低，可能需要更多的迭代来达到收敛。
// 4. Convergence Band：一种新的收敛标准是收敛带方法（convergence band）。该方法通过确定一个带宽和长度，使得蒙特卡罗样本均值在该带内的概率几乎为零，从而判断收敛性。
// 5. Graphical Methods：一些研究者推荐使用图形方法，比如绘制样本均值随迭代次数的变化图，观察其是否趋于平稳。
// 我感觉还可以用qqplot等查看均值分布是否服从正态分布，也许可行。

double Pricing::calculate_gelman_rubin(const std::vector<Eigen::VectorXd>& chains) {
    size_t n_chains = chains.size(); // 用size_t接收size()的返回值
    size_t n_samples = chains[0].size();
    
    Eigen::VectorXd means(n_chains);
    Eigen::VectorXd variances(n_chains);
    
    for (int i = 0; i < n_chains; ++i) {
        means(i) = calculate_mean(chains[i]);
        variances(i) = calculate_stdev(chains[i], means(i));
    }
    
    double mean_of_means = means.mean();
    
    double B = n_samples * (means.array() - mean_of_means).square().sum() / (n_chains - 1);
    double W = variances.mean();
    
    double V_hat = ((n_samples - 1) * W + B) / n_samples;
    
    double R_hat = std::sqrt(V_hat / W);
    
    return R_hat;
}

double Pricing::calculatePrice(const PricingModel& pricingModel, const Parameters& params) {
    std::vector<Eigen::VectorXd> all_chains(8);  // params.get<int>("chain_num")创建一个有chain_num * 8个动态大小的VectorXd的vector
    int num_simulations = 0;

    double confidence_level = params.get<double>("confidenceLevel");
    double tolerance = params.get<double>("tolerance");  // 允许设置的精度阈值

    while (num_simulations < params.get<int>("maxSimulations")) {
        std::vector<std::thread> threads;
        std::vector<Eigen::VectorXd> payoffs(8, Eigen::VectorXd(200));
        
        // 使用8个线程并行生成路径
        for (int i = 0; i < 8; ++i) {
            threads.emplace_back([&payoffs, i, &pricingModel, &params]() {  // 这句语法意味着每个for循环对应着一个新建的线程，
                Parameters local_params = params; // 在每个线程中创建params的副本
                MonteCarloSimulator simulator(local_params, pricingModel);
                simulator.generate_paths();
                const Eigen::MatrixXd& pricePaths = simulator.get_price_paths();
                const Eigen::MatrixXd& ratePaths = simulator.get_rate_paths();
                payoffs[i] = pricingModel.getPayoff()(local_params, pricePaths);

                Eigen::VectorXd discountFactors = Eigen::VectorXd::Ones(200);
                if (ratePaths.size() > 1 && (ratePaths.array() == ratePaths(0, 0)).all()) {  // 判断是否是constant rate的方法
                    double rate = ratePaths(0, 0);
                    discountFactors = Eigen::VectorXd::Constant(200, std::exp(-rate * pricePaths.cols() * local_params.get<double>("dt")));
                } else {
                    Eigen::VectorXd accumulatedRate = ratePaths.rowwise().sum() * local_params.get<double>("dt");
                    discountFactors = accumulatedRate.array().exp().matrix();
                }

                payoffs[i] = payoffs[i].array() * discountFactors.array();
            });
        }

        // 等待所有线程完成
        for (auto& thread : threads) {
            thread.join();
        }

/*        // 将所有线程生成的 payoffs 合并到 all_chains
        for (int chain_idx = 0; chain_idx < params.get<int>("chain_num"); ++chain_idx) {
            for (int i = 0; i < 8; ++i) {
                all_chains[chain_idx].conservativeResize(all_chains[chain_idx].size() + payoffs[i].size());
                all_chains[chain_idx].tail(payoffs[i].size()) = payoffs[i];
            }
        }
*/
        for (int i = 0; i < 8; ++i) {
            all_chains[i].conservativeResize(all_chains[i].size() + payoffs[i].size());
            all_chains[i].tail(payoffs[i].size()) = payoffs[i];
        }

        num_simulations += 1600;  // 每次增加800个样本

        if (is_converged(all_chains, tolerance)) {
            std::cout << "Converged after " << num_simulations << " simulations." << std::endl;
            break;
        }
    }

    if (num_simulations >= params.get<int>("maxSimulations")) {
        std::cout << "Reached maximum number of simulations without convergence." << std::endl;
    }

    // GR方法可以设置多组蒙特卡罗模拟，一般至少要3组。通过每次给每组增加200个样本，逐渐找到某个精度水平的收敛所要求的最小样本数量。主要的思想依据是组内方差和组间方差相似之后代表着样本量足够大了。一般要求R<=1.1，此时认为收敛，否则认为不收敛。
    // 汇总所有样本
    std::vector<double> all_payoffs;
    for (const auto& chain : all_chains) {
        all_payoffs.insert(all_payoffs.end(), chain.data(), chain.data() + chain.size());
    }

    Eigen::Map<Eigen::VectorXd> all_payoffs_map(all_payoffs.data(), all_payoffs.size());
    double mean_price = calculate_mean(all_payoffs_map);
    double variance = calculate_stdev(all_payoffs_map, mean_price);
    double z = get_z_value(confidence_level);
    double half_width = z * variance / std::sqrt(all_payoffs_map.size());

    // 打印调试信息。confident interval是所有样本的均值在正态分布假设下的置信区间，可以对定价作出更可靠的范围估计
    double lower_bound = mean_price - half_width;
    double upper_bound = mean_price + half_width;
    std::cout << "Mean Price: " << mean_price << std::endl;
    std::cout << "Confidence Interval: [" << lower_bound << ", " << upper_bound << "]" << std::endl;
    
    return mean_price;
}


// 因为理论上随着数据增加数据的均值和方差对应的正态分布应该可以找到数据的95%数据量的区间，那么如何设计这个收敛条件？正式的方法是GR
// 主要编写thread内容
