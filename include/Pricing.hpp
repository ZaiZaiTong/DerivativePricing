//
//  Pricing.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/14.
//

# ifndef Pricing_hpp
# define Pricing_hpp

class MonteCarloSimulator;
class Payoff;
class Parameters;
class PricingModel;
# include <vector>
# include <Eigen/Dense>
/*
class Pricing {
public:
    virtual ~Pricing() = default;
    static double calculate_mean(const std::vector<double>& values);
    static double calculate_stddev(const std::vector<double>& values, double mean);
    static double get_z_value(double confidence_level);
    static bool is_converged(const std::vector<std::vector<double>>& chains, double tolerance);
    static double calculate_gelman_rubin(const std::vector<std::vector<double>>& chains);
    static double calculatePrice(MonteCarloSimulator& simulator, const PricingModel& pricingModel, const Parameters& params);
};
*/
class Pricing {
public:
    virtual ~Pricing() = default;
    static double calculate_mean(const Eigen::VectorXd& values);
    static double calculate_stdev(const Eigen::VectorXd& values, double mean);
    static double get_z_value(double confidence_level);
    static bool is_converged(const std::vector<Eigen::VectorXd>& chains, double tolerance);
    static double calculate_gelman_rubin(const std::vector<Eigen::VectorXd>& chains);
    static double calculatePrice(const PricingModel& pricingModel, const Parameters& params);
};

# endif /* Pricing_hpp */

