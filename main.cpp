#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

struct Order {
    std::string orderId;
    char orderType;
    int orderSize;
    float price = 0.0;
    int priority;
};

void processFile(const std::string &fileName) {
    std::ifstream inputFile(fileName);
    std::vector<Order> orders;
    int priority=1;

    float previousTransactionPrice;
    inputFile >> previousTransactionPrice;


    std::string line;
    while (std::getline (inputFile, line)) {
        std::istringstream stringStream(line);
        Order order;
        if (stringStream >> order.orderId >> order.orderType >> order.orderSize) {
            order.price = previousTransactionPrice;
            if (stringStream >> order.price){
            }
            order.priority = priority++;
            orders.push_back(order);
        }
    }
    for (const auto& order : orders) {
        std::cout << "Order ID: " << order.orderId << ", Type: " << order.orderType
                  << ", Size: " << order.orderSize << ", Price: " << order.price
                  << ", Priority: " << order.priority << std::endl;
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