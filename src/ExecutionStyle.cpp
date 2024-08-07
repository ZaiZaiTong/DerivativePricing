//
//  ExecutionStyle.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# include "ExecutionStyle.hpp"
# include <iostream>

// 欧式期权实现：大部分指数期权
bool EuropeanOption::canExercise(double time, double maturity) const {
    return time == maturity;
}

// 美式期权实现：大部分可交易股权期权和期货期权
bool AmericanOption::canExercise(double time, double maturity) const {
    return time <= maturity;
}

// 百慕大期权实现
BermudanOption::BermudanOption(const std::vector<double>& exerciseDates) : exerciseDates(exerciseDates) {}

bool BermudanOption::canExercise(double time, double maturity) const {
    return std::find(exerciseDates.begin(), exerciseDates.end(), time) != exerciseDates.end();
}

// 通过每一期缴纳期权费，不断延长期权的到期日。类似于美式期权。

// 任何衍生品合约都可能存在紧急终止条款，类似于美式合约，可以在到期日前随时或者St处于某个范围内时，停止合约并获得部分退款R，R可能依赖于S或者按照别的方式进行时变。
