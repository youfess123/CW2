#include <algorithm>
#include <iostream>
#include "Order.h"
#include <fstream>
#include <iomanip>
#include <vector>
#include <sstream>
void readOrdersFromFile(std::ifstream& file);
void sortOrderPriority(std::vector<Order>& buyOrders, std::vector<Order>& sellOrders);
void printPendingOrders(const std::vector<Order>& buyOrders, const std::vector<Order>& sellOrders, float lastTransactionPrice);

std::vector<Order> buyOrders;
std::vector<Order> sellOrders;
std::vector<Order> executedOrders;
std::vector<Order> unexecutedOrders;
float previousTransactionPrice;

void readOrdersFromFile(std::ifstream& file) {

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
            if (checkOrderType(order)) {
                buyOrders.push_back(order);
            } else {
                sellOrders.push_back(order);
            }
            sortOrderPriority(buyOrders, sellOrders);
            printPendingOrders(buyOrders, sellOrders, previousTransactionPrice);
        }
    }
}

bool compareOrders(const Order& a,const Order& b,bool isBuyOrder) {

    if (checkOrderPricingType(a) && !checkOrderPricingType(b)) {
        return true;
    }

    if (!checkOrderPricingType(a) && checkOrderPricingType(b)) {
        return false;
    }

    if (a.getLimitPrice() == b.getLimitPrice()) {
        return a.getArrivalDateTime() < b.getArrivalDateTime();
    }

    return isBuyOrder ? a.getLimitPrice() > b.getLimitPrice() : a.getLimitPrice() < b.getLimitPrice();
}

void sortOrderPriority(std::vector<Order>& buyOrders, std::vector<Order>& sellOrders) {

    std::sort(buyOrders.begin(), buyOrders.end(), [](const Order& a, const Order& b) {
        return compareOrders(a, b, true);
    });

    std::sort(sellOrders.begin(), sellOrders.end(), [](const Order& a, const Order& b) {
        return compareOrders(a, b, false);
    });
}

void printPendingOrders(const std::vector<Order> &buyOrders, const std::vector<Order> &sellOrders, float lastTransactionPrice) {

    std::cout << "Last trading price: " << lastTransactionPrice << "\n";
    std::cout << "Buy \n";
    std::cout << "-------------------\n";

    for (const auto& order : buyOrders) {
        if (order.getPricingType() == OrderPricingType::MARKET) {
            std::cout << order.getOrderID() << " M " << order.getTargetQuantity() << std::endl;
        } else {
            std::cout << order.getOrderID() << " "
                      << order.getLimitPrice() << " "
                      << order.getTargetQuantity() << std::endl;
        }
    }
    std::cout << "\n";

    std::cout << "Sell \n";
    std::cout << "-------------------\n";
    for (const auto& order : sellOrders) {
        if (order.getPricingType() == OrderPricingType::MARKET) {
            std::cout << order.getOrderID() << " M " << order.getTargetQuantity() << std::endl;
        } else {
            std::cout << order.getOrderID() << " "
                      << order.getLimitPrice() << " "
                      << order.getTargetQuantity() << std::endl;
        }
    }
    std::cout << "\n";
}

void matchOrders(const std::vector<Order> &buyOrders, const std::vector<Order> &sellOrders) {

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

    readOrdersFromFile(file);

    file.close();

    return 0;
}