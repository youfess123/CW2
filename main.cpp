#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <sstream>

struct Order {
    std::string orderId;
    char orderType;
    int orderSize;
    float price = 0.0;
    int priority;
};

std::vector<Order> processFile(const std::string &fileName) {
    std::ifstream inputFile(fileName);
    std::vector<Order> pendingOrders;
    int priority=1;

    float previousTransactionPrice;
    inputFile >> previousTransactionPrice;

    std::string line;
    while (std::getline (inputFile, line)) {
        std::istringstream stringStream(line);
        Order order;
        if (stringStream >> order.orderId >> order.orderType >> order.orderSize) {
            order.price = previousTransactionPrice;  //this might be incorrect find out later.
            if (stringStream >> order.price){
            }
            order.priority = priority++;
            pendingOrders.push_back(order);
        }
    }

    std::vector<Order> buyOrders;
    std::vector<Order> sellOrders;

    for(const auto &order : pendingOrders) {
        if (order.orderType=='B') {
            buyOrders.push_back(order);
        }else {
            sellOrders.push_back(order);
        }
    }

    inputFile.close();
    return pendingOrders;

}
void printOrder(const std::vector<Order> &buyOrders, const std::vector<Order> &sellOrders, float lastTransactionPrice) {

    std::cout << "Last trading price: " << std::fixed << std::setprecision(2) << lastTransactionPrice << "\n";
    std::cout << "Buy                       Sell\n";
    std::cout << "------------------------------------------\n";
    

}

void processOrder(const std::vector<Order> &pendingOrders) {
    std::vector<Order> executedOrders;
    std::vector<Order> unexecutedOrders;

    for (const auto& order : executedOrders) {
        std::string type;
        if(order.orderType =='B') {
            type="purchased";
        }else {
            type="sold";
        }
        std::cout << "order " << order.orderId << " " << order.orderSize << " shares "+type+" at price " << order.price << std::endl;
    }
    for (const auto& order : unexecutedOrders) {
        std::cout << "order " << order.orderId +" " << order.orderSize + " shares unexecuted" << std::endl;
    }
}

int main() {
    std::string fileName;
    std::cout << "Enter the file name: "<< std::endl;
    std::cin>>fileName;
    std::vector<Order> pendingOrders = processFile(fileName);
    processOrder(pendingOrders);

    return 1;

}