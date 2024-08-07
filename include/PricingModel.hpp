//
//  PricingModel.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# ifndef PricingModel_hpp
# define PricingModel_hpp

// 前向声明
class AssetPriceModel;
class RateModel;
class VolatilityModel;
class ExecutionStyle;
class TransactionCost;
class DividendYield;
class BoundaryConditions;
class Payoff;
class Parameters;

class PricingModel {
public:
    PricingModel(const RateModel& rateModel,
                 const VolatilityModel& volModel,
                 const AssetPriceModel& assetModel,
                 const Payoff& payoff,
                 //const ExecutionStyle& executionStyle,
                 const TransactionCost& transactionCost
                 //const DividendYield& dividendYield,
                 //const BoundaryConditions& boundaryConditions,
                 );

    void update(const Parameters& params);
    const Payoff& getPayoff() const;
    const RateModel& getRateModel() const;
    const VolatilityModel& getVolatilityModel() const;
    const AssetPriceModel& getAssetPriceModel() const;
    //const ExecutionStyle& getExecutionStyle() const;
    const TransactionCost& getTransactionCost() const;
    //const DividendYield& getDividendYield() const;

private:
    const RateModel& rateModel_;
    const VolatilityModel& volModel_;
    const AssetPriceModel& assetModel_;
    //const ExecutionStyle& executionStyle_;
    const TransactionCost& transactionCost_;
    //const DividendYield& dividendYield_;
    //const BoundaryConditions& boundaryConditions_;
    const Payoff& payoff_;
};

# endif // PricingModel_hpp

