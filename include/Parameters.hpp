//
//  Parameters.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/8.
//
// 所有AssetPriceModel中模型会用到的参数
// 注意：为了同模型多个实现之间的dW等参数冲突，应该为不同模型构造不同的Parameters。即便是不同的模型
// 最好也用不同的Parameters实现

# ifndef PARAMETERS_HPP
# define PARAMETERS_HPP

# include <unordered_map>
# include <string>
# include <any>
# include <stdexcept>

class Parameters {
public:
    Parameters();
    virtual ~Parameters() = default;

    template<typename T>
    void set(const std::string& key, const T& value);

    template<typename T>
    T get(const std::string& key) const;

    bool contains(const std::string& key) const;

    void updateWithRandomness(double dW1, double dW2, double dW3);

private:
    std::unordered_map<std::string, std::any> data_;
};

template<typename T>     // template<class T>也是正确的。同时还有方法...能同时产生多个模版类，让输入的参数可以不局限于一个模版类T。可以用方法template<class A, class B, class C>这样后面就有三种类型可以使用
// 比如构造操作Function(const A& a1, const A& a2, const A& a3, const B& b, const C& c)
// main中创建该class的时候，例如Class<double, double, int>需要分别在对应位置输入数据类型。
void Parameters::set(const std::string& key, const T& value) {
    data_[key] = value;
}

template<typename T>
T Parameters::get(const std::string& key) const {
    // 函数前的const T：对于const double没有效果，但是对于指针返回值等意味着这个返回值不可以在函数外部被修改（地址对应的那个数据不可修改）
    // 参数const：不能在函数内修改该参数（函数外可以修改）
    // const {}：仅限制在函数内不能修改任何成员变量，不限制全局变量和局部变量
    auto it = data_.find(key);
    if (it != data_.end()) {
        return std::any_cast<T>(it->second);
    } else {
        throw std::runtime_error("Key not found: " + key);
    }
}
// 模版类的操作的实现必须在hpp中完成，否则在cpp中实现的话，有些时候就会出现链接不到的情况
# endif // PARAMETERS_HPP
