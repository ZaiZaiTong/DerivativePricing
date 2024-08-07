//
//  Parameters.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/9.
//
// 整个Project会用到的所有参数都来自于这里，包括ratemodel MCS等，还要包括payoff类的选项
// 所有函数的返回值也要尽量用&或*，即函数定义前面的const double& function() {}。防止调用函数的时候产生返回值的复制
// 初始化列表可以减少数据复制，只需要输入的参数是&或*，则成员变量可以保持非&或*，即可防止临时复制。相对的，如果用=传输参数，则会产生复制，除非成员变量定义为&或*
// 注意：为了同模型多个实现之间的dW等参数冲突，应该为不同模型构造不同的Parameters。即便是不同的模型
// 最好也用不同的Parameters实现
// 参数放在一起方便多线程时统一创建共享数据副本，防止线程之间干扰

# include "Parameters.hpp"
# include <stdexcept>

Parameters::Parameters() = default;

bool Parameters::contains(const std::string& key) const {
    return data_.find(key) != data_.end();
}

// 需要在这里定义模板函数的实例化，否则链接时可能会出现未定义的引用错误
template void Parameters::set<double>(const std::string&, const double&);
template void Parameters::set<bool>(const std::string&, const bool&);
template double Parameters::get<double>(const std::string&) const;
template bool Parameters::get<bool>(const std::string&) const;
