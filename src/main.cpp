//
//  main.cpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//
//
// 经验积累：最初通过画流程图，初步确定了需要哪些文件以及对应的功能。后续在具体编写程序的过程中发现了一些需要迁移的功能。于是我开始模仿分工的方法，尽量用干净科学的方法完成每个功能，然后再思考如何让各部分组合在一起，修改其中的冲突等。逐步完成更有成就感，也不会被文件之间比较大的联系网络击垮。
// 加速方法：hpp与cpp、并行运算
//
//
// 面向对象编程
// 使用基类与派生类实际上几乎等价于在基类中引入参数以及if语句。派生类方法利于维护和分工协作。符合开闭原则Open Closed Principle，即对扩展开放，对修改关闭

// include原则：
// 1. 减少hpp中的include，减少编译时间。用前向声明代替include hpp（除非用到相应hpp成员或方法）。合法的操作包括Name&引用和Name*指针。但是不能用相关操作.
// 2. 使用ifend头文件保护以避免重复包含hpp
// 3. 避免在hpp中实现函数
// 4. 包含顺序为项目内hpp、标准库、第三方，确保项目内hpp优先解析，减少头文件冲突
// 5. 每个cpp、hpp、tpp应该包含自己需要的库，增强独立性，编译器会处理重复包含的问题

// class、struct、typedef struct：
// 三者是完全一致的（除了class默认private，struct默认public以外，三者可以完全等价互换），typedef方法是C的产物，为了初始化时不用每次都写struct name x，所以用typedef定义，初始化可以写为name x。C++中直接用struct就可以，在初始化时可以省略struct。

// 构造函数与～析构函数：
// 1. 如果类class不存在后续的派生类，则采用～足够，如果class是一个基类，后续还有派生类会调用该类，则必须用虚析构virtual ～，否则释放内存的时候只会释放最底层class，不会释放新建的派生类对应的内存。表现为哪一层不写virtual，释放就停在哪一层。而为了增加程序的开放性，所有class都可以采用虚析构，便于后续增添新的内容
// 2. c++11允许显式用 = default表示{}（即默认空的构造函数和析构函数）
// 3. c++11允许不显式写出（即完全不写）构造函数和析构函数。其中析构函数在class中有虚函数时会自动调整为虚析构函数，否则为一般析构函数。良好的习惯是显式写出虚析构函数，防止默认情况产生一般析构函数
// 4. 构造函数和析构函数的名称必须与class的名称完全保持一致
// 5. 构造函数的实现（cpp）结构一定为ClassName::ClassName，因为第一个位置是include hpp后其中的class名称，第二个位置为该class的构造函数名称，二者必须完全一致
// 6. 成员初始化列表，仅在构造函数实现中使用，一般都放在cpp中，降低可读性，但是提高编译速度。相当于rateModel_ = rateModel; 相对于 = 成员初始化列表效率更高。对于没有默认构造函数的类成员，必须用成员初始化列表初始化。此外，初始化列表不是必须将所有成员变量初始化，但是一旦参数进入初始化列表，那么在构造该class的实现以前就必须确保所有初始化列表中参数都已经完成赋值。对于没进入初始化列表的成员变量等可以在后续使用到相关操作前赋值，是完全合法的
// 7. 所有的函数尽量用&或*引入参数，用const防止变更。这样可以防止数据的临时复制，降低运行效率
// 8. 所有函数的返回值也要尽量用&或*，即函数定义前面的const double& function() {}。防止调用函数的时候产生返回值的复制
// 9. 如果成员变量是&或*，则只能用初始化列表初始化这些变量（初始化后在任何地方都不能被重新赋值），并且这些变量必须被初始化（不同于一般的成员变量可以在后续使用中初始化）
// 10. 初始化列表可以减少数据复制，只需要输入的参数是&或*，则成员变量可以保持非&或*，即可防止临时复制。相对的，如果用=传输参数，则会产生复制，除非成员变量定义为&或*

// virtual：
// virtual声明的函数都是可以在派生类中被重写的函数（被称为虚函数），而如果虚函数最终 = 0，则为纯虚函数，表明函数还没有实现，并且必须在派生类中实现。存在纯虚函数，则类为抽象类

/*
| 函数类型  | 用途及适用场景                          | 示例                        |
| ------ | -------------------------------------- | -------------------------- |
| 普通函数 | 适用于不需要多态性、实现简单且通用的操作。    | 折现计算等                    |
| 虚函数   | 存在默认实现但派生类可能会重写的功能override | 不同资产类型的定价有不同的定价模型|
| 纯虚函数 | 不能提供有意义的默认实现。即写不出虚函数      | 期权定价模型，如欧式期权、美式期权|
*/

// operator：
// 重新定义某些符号，例如(), +, <等等，对于括号来说，初始构造时的括号和后续再次使用时的括号分别对应着不同的功能。另外，只要保证括号需要输入的数据类型或者参数个数不一样，则可以多次定义括号

// ::
// 1. ::类似于Python中的.，是class和namespace专用的，用于说明操作的所属
// 2. 类似于std是一个命名空间，其下有很多标准库，包括vector、string、iostream等。# include <iostream>;可以粘贴标准库，然后就能用std::调用相关程序，这就是zainamespace中的用法。using namespace std;不适合在大型项目中使用，容易产生冲突
// 3. 此外，::只能用于cpp中成员函数的实现（表明函数所属的类class）或者main中要引用类的静态成员（属于整个类的变量或者函数static）。如果在main中正常调用成员函数等需要用.

// 基类与派生类:
// 1. 面向对象编程中，允许使用基类的&或者*来引用派生类对象。这样可以保持开放性。综上，为了保持开放性，保证后续可以随时增添新的模型，应该设置基类，并将具体模型放到派生类中。同时所用引用该类别模型的函数都要引用基类而不是具体的派生类

// private:
// 1. private中成员变量一般用_放在结尾便于区分这里的接收参数和后续的输入参数
// 2. 良好的顺序（真实顺序不会影响代码编译）：构造函数、析构函数、公有成员函数（虚函数和重载函数）、私有成员函数和成员变量
// 3. 成员变量可以公有或者私有化，比较好的做法是私有化。私有化以后，必须要单独设置更新类的的公有函数操作才能更改成员变量，而对于公有化的成员变量则可以用.进行获取和修改
// 4. 对于存在成员变量的类class，比较好的做法是始终设置一个更新成员变量的函数

# include <iostream>
# include "MonteCarloSimulator.hpp"
# include "Parameters.hpp"
# include "PricingModel.hpp"
# include "RateModel.hpp"
# include "VolatilityModel.hpp"
# include "AssetPriceModel.hpp"
# include "Payoff.hpp"
# include "TransactionCost.hpp"
# include "ModelParams.hpp"
# include "Pricing.hpp"

int main() {
    /*
    ModelParams modelParams;
    std::vector<std::string> availableModels = modelParams.getAvailableModels();
    std::cout << "Available model: ";
    for (const std::string& model : availableModels) {
        std::cout << model << " ";
    }
    std::cout << std::endl;
// 获取某个特定模型的参数
    std::string modelName = "HullWhiteModel";
    std::vector<std::string> params_required = modelParams.getModelParams(modelName);
    std::cout << "Parameters for " << modelName << ": ";
    for (const std::string& param : params_required) {
        std::cout << param << " ";
    }
    std::cout << std::endl;
    
    // 检查给定的参数列表是否包含指定模型所需的所有参数
    Parameters params;
    // 设置初始参数
    params.set<double>("spot", 100.0);
    params.set<double>("rate", 0.03);
    params.set<double>("dt", 1.0 / 252);
    params.set<double>("volatility", 0.2);
    params.set<double>("a_HWM", 0.2);
    params.set<double>("sigma_HWM", 0.2);
    params.set<double>("strike", 110.0);
    params.set<double>("errorThreshold", 0.01);
    params.set<int>("maxSimulations", 1000000); // 0.1 1000000 收敛到了36～38
    params.set<double>("confidenceLevel", 0.95);
    params.set<double>("tolerance", 1);
    params.set<int>("chain_num", 10);

    modelParams.checkParams(modelName, params);
    
    // 创建模型
    HullWhiteModel hullWhiteModel(params);
    ConstantVolatilityModel constantVolatilityModel(params);
    GeometricBrownianMotionModel geometricBrownianMotionModel(params);
    EuropeanPutPayoff europeanPutPayoff;

    PricingModel pricingModel(hullWhiteModel, constantVolatilityModel, geometricBrownianMotionModel, europeanPutPayoff, ZeroTransactionCost());

    // 创建MonteCarlo模拟器
    MonteCarloSimulator simulator(params, pricingModel, numSteps);
    
    Pricing pricing;
    pricing.calculatePrice(simulator, pricingModel, params);
    */
    // 检查给定的参数列表是否包含指定模型所需的所有参数
    Parameters params2;
    // 设置初始参数
    params2.set<double>("spot", 100.0);
    params2.set<double>("rate", 0.05);
    params2.set<double>("dt", 1.0 / 252);
    params2.set<double>("volatility", 0.2);
    params2.set<double>("strike", 100.0);
    params2.set<double>("errorThreshold", 0.01);
    params2.set<int>("maxSimulations", 1000000); // 0.1 1000000 收敛到了36～38
    params2.set<double>("confidenceLevel", 0.95);
    params2.set<double>("tolerance", 0.00001);
    params2.set<int>("chain_num", 10);
    params2.set<double>("numSteps", 25);  // 一年252个交易日。这里可以放置任何dt的倍数，表示区间长度

    // modelParams.checkParams(modelName, params2);
    
    // 创建模型
    ConstantRateModel constantRateModel(params2);
    ConstantVolatilityModel constantVolatilityModel2(params2);
    GeometricBrownianMotionModel geometricBrownianMotionModel2(params2);
    EuropeanPutPayoff europeanPutPayoff2;

    PricingModel pricingModel2(constantRateModel, constantVolatilityModel2, geometricBrownianMotionModel2, europeanPutPayoff2, ZeroTransactionCost());

    Pricing pricing2;
    pricing2.calculatePrice(pricingModel2, params2);
    
    return 0;
}

// 如果存在无风险套利的方法定价衍生品，就可以用风险中性定价法。因为存在无风险套利的情况下，无论投资者的风险偏好是怎样的，都不会影响最终的定价，因此可以只查看风险中性投资者的定价。而且风险中性测度下，贴现价值是鞅。衍生品一般都存在无风险套利方法
