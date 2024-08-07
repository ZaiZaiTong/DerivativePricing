//
//  DividendYield.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# ifndef DividendYield_hpp
# define DividendYield_hpp

class DividendYield {
public:
    virtual ~DividendYield() = default; 
    virtual double getYield(double time) const = 0;
};

class FixedDividendYield : public DividendYield {
public:
    explicit FixedDividendYield(double yield);
    double getYield(double time) const override;

private:
    double yield;
};

# endif // DIVIDENDYIELD_HPP
