//
//  BoundaryConditions.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//
// 边界条件应该主要是服务于FDM方法解偏微分方程
// 边界条件包括Dirichlet边界条件（定义与边界的函数取值）、Neumann边界条件（定义与边界的导数取值）、Robin边界条件（定义与边界函数和导数线性组合的取值）。
// 终值条件Payoff指在终止时间点上的函数取值，时间序列问题。

# include "BoundaryConditions.hpp"
# include <iostream>

// 反射边界条件实现
double ReflectiveBoundary::applyCondition(double price) const {
    return std::abs(price);
}

// 吸收边界条件实现
double AbsorptiveBoundary::applyCondition(double price) const {
    return std::max(price, 0.0);
}
