#include <iostream>
#include <fstream>
#include <vector>

struct Order {
    std::string orderId;
    char orderType;
    int orderSize;
    float price;
    int priority;

};
void processFile(const std::string &fileName) {
    std::ifstream inputFile(fileName);

    float previousTransactionPrice;
    inputFile >> previousTransactionPrice;


    std::vector<Order> orders;
    std::string myText;
    std::string line;
    while (std::getline (inputFile, line)) {
        myText += line + "\n";
    }


    inputFile.close();

}

int main() {
    std::string fileName;
    std::cout << "Enter the file name: ";
    std::cin>>fileName;
    processFile(fileName);
    return 1;

}