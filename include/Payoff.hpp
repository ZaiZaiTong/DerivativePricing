//
//  Payoff.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/11.
//

# ifndef Payoff_hpp
# define Payoff_hpp

# include <string>
# include <memory>
# include <vector>
# include <Eigen/Dense>

class Parameters;

class Payoff {
public:
    virtual ~Payoff() = default;
    virtual Eigen::VectorXd operator()(const Parameters& params, const Eigen::MatrixXd& paths) const = 0;
    virtual std::string getName() const = 0;
};

// 欧式看涨期权
class EuropeanCallPayoff : public Payoff {
public:
    EuropeanCallPayoff();
    virtual ~EuropeanCallPayoff() = default;
    Eigen::VectorXd operator()(const Parameters& params, const Eigen::MatrixXd& paths) const override;
    std::string getName() const override;
};

// 欧式看跌期权
class EuropeanPutPayoff : public Payoff {
public:
    EuropeanPutPayoff();
    virtual ~EuropeanPutPayoff() = default;
    Eigen::VectorXd operator()(const Parameters& params, const Eigen::MatrixXd& paths) const override;
    std::string getName() const override;
};

// 障碍期权
class BarrierPayoff : public Payoff {
public:
    BarrierPayoff();     // double barrier, bool isKnockIn
    virtual ~BarrierPayoff() = default;
    Eigen::VectorXd operator()(const Parameters& params, const Eigen::MatrixXd& paths) const override;
    std::string getName() const override;
/*private:
    double barrier_;
    bool isKnockIn_;*/
};

// 亚式期权
class AsianPayoff : public Payoff {
public:
    AsianPayoff();
    virtual ~AsianPayoff() = default;
    Eigen::VectorXd operator()(const Parameters& params, const Eigen::MatrixXd& paths) const override;
    std::string getName() const override;
    // void addSpot(const Parameters& params);
/*private:
    std::vector<double> spots_;*/
};

// 回溯期权
class LookbackPayoff : public Payoff {
public:
    LookbackPayoff();
    virtual ~LookbackPayoff() = default;
    Eigen::VectorXd operator()(const Parameters& params, const Eigen::MatrixXd& paths) const override;
    std::string getName() const override;
    // void addSpot(const Parameters& params);
/*private:
    mutable double minSpot_;            // 想在const{}中修改成员变量，必须保证成员变量为mutable。const{}应该是可以让其中所有没有mutable的变量都变成const不可更改
    mutable double maxSpot_;*/
};

// 其他期权类型的声明...

# endif // Payoff_hpp
