#include <iostream>
#include <fstream>

struct Order {
    std::string orderId;
    int orderSize;
    float price;
};
int main() {
    std::string fileName;
    std::cin>>fileName;
    std::ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open the file " << fileName << std::endl;
        return 1;
    }
    float previousTransactionPrice;
    inputFile >> previousTransactionPrice;

    std::string myText;
    std::string line;
    while (std::getline (inputFile, line)) {
        myText += line + "\n";
    }
    inputFile.close();

    return 0;

}