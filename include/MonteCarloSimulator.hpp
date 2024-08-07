//
//  MonteCarloSimulator.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

#ifndef MONTE_CARLO_SIMULATOR_HPP
#define MONTE_CARLO_SIMULATOR_HPP

# include <Eigen/Dense>
# include "Parameters.hpp"
# include "PricingModel.hpp"

class MonteCarloSimulator {
public:
    MonteCarloSimulator(const Parameters& params, const PricingModel& pricingModel);
    void run_simulation();
    void generate_paths();
    const Eigen::MatrixXd& get_price_paths() const;
    // 最后一个const表示该函数内的内容都不能修改。但是private中mutable的成员变量是可以修改的
    const Eigen::MatrixXd& get_rate_paths() const;
    const Eigen::MatrixXd& get_volatility_paths() const;

private:
    Parameters params_;
    PricingModel pricingModel_;
    // 在成员变量中出现别的类class，则必须在hpp中# include，因为编译器需要知道每个成员变量的大小，如果用的是指针或者地址，则不需要include，因为指针和地址的大小是固定的。但是当操作函数或者返回类型中出现别的class时，则可以不用include而用class前向声明
    int numSteps_;
    // double errorThreshold_;

    Eigen::MatrixXd pricePaths_;
    Eigen::MatrixXd ratePaths_;
    Eigen::MatrixXd volatilityPaths_;

    
    double get_random_number();
    // bool check_convergence();
};

#endif // MONTE_CARLO_SIMULATOR_HPP
