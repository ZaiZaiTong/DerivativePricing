//
//  ModelParams.hpp
//  DerivativesPricing
//
//  Created by 俊延 on 2024/7/11.
//

# ifndef ModelParams_hpp
# define ModelParams_hpp

# include <string>
# include <vector>
# include <unordered_map>
# include <any>

class Parameters;

class ModelParams {
public:
    ModelParams();

    void initialize();
    std::vector<std::string> getAvailableModels() const;
    std::vector<std::string> getModelParams(const std::string& modelName) const;
    std::vector<std::string> checkParams(const std::string& modelName, const Parameters& params) const;

private:
    std::unordered_map<std::string, std::vector<std::string>> modelParams;
};

#endif // ModelParams_hpp



