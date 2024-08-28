#include <iostream>
#include <string>
#include <vector>

class OrganicCompound {
public:
    OrganicCompound(const std::string& name, const std::string& formula)
        : name(name), formula(formula) {}

    void addProperty(const std::string& property) {
        properties.push_back(property);
    }

    void displayInfo() const {
        std::cout << "Compound Name: " << name << std::endl;
        std::cout << "Molecular Formula: " << formula << std::endl;
        std::cout << "Properties: " << std::endl;
        for (const auto& prop : properties) {
            std::cout << "- " << prop << std::endl;
        }
    }

private:
    std::string name;             // 化合物名称
    std::string formula;          // 分子式
    std::vector<std::string> properties; // 特性
};

int main() {
    // 创建几个有机化合物实例
    OrganicCompound ethanol("Ethanol", "C2H6O");
    ethanol.addProperty("Boiling Point: 78.37 °C");
    ethanol.addProperty("Density: 0.789 g/cm^3");
    ethanol.addProperty("Solubility in Water: Miscible");

    OrganicCompound glucose("Glucose", "C6H12O6");
    glucose.addProperty("Boiling Point: Decomposes above 146 °C");
    glucose.addProperty("Solubility in Water: Highly Soluble");

    // 显示信息
    ethanol.displayInfo();
    std::cout << std::endl;
    glucose.displayInfo();

    return 0;
}
