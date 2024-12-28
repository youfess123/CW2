#include <iostream>
#include "Order.h"
#include <fstream>
#include <iomanip>
#include <vector>
#include <sstream>

// class Order {
//
//     // std::string orderId;
//     // char orderType;
//     // int orderSize;
//     // float price = 0.0;
//     // int priority;
//
// };
//
//

// void printPendingOrder(const std::vector<Order> &buyOrders, const std::vector<Order> &sellOrders, float lastTransactionPrice) {
//
//     std::cout << "Last trading price: " << std::fixed << std::setprecision(2) << lastTransactionPrice << "\n";
//     std::cout << "Buy                       Sell\n";
//     std::cout << "------------------------------------------\n";
//
//
// }
//
// void processOrder(const std::vector<Order> &pendingOrders) {
//     std::vector<Order> executedOrders;
//     std::vector<Order> unexecutedOrders;
//
//     for (const auto& order : executedOrders) {
//         std::string type;
//         if(order.orderType =='B') {
//             type="purchased";
//         }else {
//             type="sold";
//         }
//         std::cout << "order " << order.orderId << " " << order.orderSize << " shares "+type+" at price " << order.price << std::endl;
//     }
//     for (const auto& order : unexecutedOrders) {
//         std::cout << "order " << order.orderId +" " << order.orderSize + " shares unexecuted" << std::endl;
//     }
// }
std::vector<Order> pendingOrders;
std::vector<Order> buyOrders;
std::vector<Order> sellOrders;


void processFile(std::ifstream& file) {

    float previousTransactionPrice;
    file >> previousTransactionPrice;

    std::string orderID, orderTypeStr;
    int arrivalDateTime=1;
    long targetQuantity;
    float limitPrice;


    std::string line;
    while (std::getline (file, line)) {
        std::istringstream stringStream(line);
        if (stringStream >> orderID >> orderTypeStr >> targetQuantity) {
            OrderType orderType = (orderTypeStr == "B") ? OrderType::BUYING_ORDER : OrderType::SELLING_ORDER;
            limitPrice = previousTransactionPrice;
            Order order;

            if (stringStream >> limitPrice) {
                order = Order(arrivalDateTime++, orderID, orderType, OrderPricingType::LIMIT, targetQuantity, limitPrice);
            } else {
                order = Order(arrivalDateTime++, orderID, orderType, OrderPricingType::MARKET, targetQuantity, limitPrice);
            }

            pendingOrders.push_back(order);
        }
    }

    for(const auto &order : pendingOrders) {
        std::cout << order << std::endl;
        if (order.getOrderType()==OrderType::BUYING_ORDER) {
            buyOrders.push_back(order);
        }else {
            sellOrders.push_back(order);
        }
    }

}

int main(int argc, char* argv[]) {
    std::cout << std::fixed << std::setprecision(2);

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Error: Could not open file " << filename <<"\n";
        return 1;
    }

    processFile(file);
    // processOrder(pendingOrders);

    file.close();

    return 0;
}