//
//  TransactionCost.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/5.
//

# ifndef TransactionCost_hpp
# define TransactionCost_hpp

class TransactionCost {
public:
    virtual ~TransactionCost() = default;
    virtual double getCost(double transactionSize) const = 0;
};

class ZeroTransactionCost : public TransactionCost {
public:
    double getCost(double transactionSize) const override;
};

class FixedTransactionCost : public TransactionCost {
public:
    explicit FixedTransactionCost(double cost);
    double getCost(double transactionSize) const override;

private:
    double cost;
};

# endif // TRANSACTIONCOST_HPP
