//
//  ExecutionStyle.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# ifndef ExecutionStyle_hpp
# define ExecutionStyle_hpp

# include <vector>


class ExecutionStyle {
public:
    virtual ~ExecutionStyle() = default;
    virtual bool canExercise(double time, double maturity) const = 0;
};

class EuropeanOption : public ExecutionStyle {
public:
    bool canExercise(double time, double maturity) const override;
};

class AmericanOption : public ExecutionStyle {
public:
    bool canExercise(double time, double maturity) const override;
};

class BermudanOption : public ExecutionStyle {
public:
    explicit BermudanOption(const std::vector<double>& exerciseDates);
    bool canExercise(double time, double maturity) const override;

private:
    std::vector<double> exerciseDates;
};

# endif // EXECUTIONSTYLE_HPP
