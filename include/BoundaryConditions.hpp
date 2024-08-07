//
//  BoundaryConditions.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# ifndef BoundaryConditions_hpp
# define BoundaryConditions_hpp

class BoundaryConditions {
public:
    virtual ~BoundaryConditions() = default;
    virtual double applyCondition(double price) const = 0;
};

class ReflectiveBoundary : public BoundaryConditions {
public:
    double applyCondition(double price) const override;
};

class AbsorptiveBoundary : public BoundaryConditions {
public:
    double applyCondition(double price) const override;
};

# endif /* BoundaryConditions_hpp */
