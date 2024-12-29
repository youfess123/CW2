#include <algorithm>
#include <iostream>
#include "Order.h"
#include <fstream>
#include <iomanip>
#include <vector>
#include <sstream>

std::vector<Order> buyOrders;
std::vector<Order> sellOrders;
std::vector<Order> executedOrders;

float previousTransactionPrice;

float determineExecutionPrice(const Order& sellOrder, const Order& buyOrder) {
    if (isMarketOrder(sellOrder) && isMarketOrder(buyOrder)) {
        return previousTransactionPrice;
    }
    if (!isMarketOrder(sellOrder) && !isMarketOrder(buyOrder)) {
        if(sellOrder.getArrivalDateTime()<buyOrder.getArrivalDateTime()) {
            return sellOrder.getLimitPrice();
        }else {
            return buyOrder.getLimitPrice();
        }
    }
    if (isMarketOrder(sellOrder) && !isMarketOrder(buyOrder)) {
        return buyOrder.getLimitPrice();
    }
    if (isMarketOrder(buyOrder) && !isMarketOrder(sellOrder)) {
        return sellOrder.getLimitPrice();
    }
}

bool compareOrders(const Order& a,const Order& b,bool isBuyOrder) {

    if (isMarketOrder(a) && !isMarketOrder(b)) {
        return true;
    }
    if (!isMarketOrder(a) && isMarketOrder(b)) {
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
        if (isMarketOrder(order)) {
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
        if (isMarketOrder(order)) {
            std::cout << order.getOrderID() << " M " << order.getTargetQuantity() << std::endl;
        } else {
            std::cout << order.getOrderID() << " "
                      << order.getLimitPrice() << " "
                      << order.getTargetQuantity() << std::endl;
        }
    }
    std::cout << "Executed Orders \n";
    std::cout << "-------------------\n";
    std::cout << "\n";
    for (const auto& order : executedOrders) {
        std::cout<< order.getOrderID() << " " << order.getTargetQuantity() << std::endl;
    }

}
bool canMatchOrder(const Order& buyOrder, const Order& sellOrder) {
    return (isMarketOrder(buyOrder) || isMarketOrder(sellOrder)||buyOrder.getLimitPrice() >= sellOrder.getLimitPrice());
}

void executeOrders(const std::vector<Order> &buyOrders, const std::vector<Order> &sellOrders, float lastTransactionPrice) {
    for (auto buyOrder : buyOrders) {
        if (buyOrder.getTargetQuantity() ==0) {
            continue;
        }
        for (auto sellOrder : sellOrders) {
            if (sellOrder.getTargetQuantity() == 0) {
                continue;
            }
            if (canMatchOrder(buyOrder,sellOrder)) {
                int tradedSharesQuantity = std::min(buyOrder.getTargetQuantity(), sellOrder.getTargetQuantity());

                buyOrder.setTargetQuantity(buyOrder.getTargetQuantity() - tradedSharesQuantity);
                sellOrder.setTargetQuantity(sellOrder.getTargetQuantity() - tradedSharesQuantity);
            }
        }
    }
}

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
                order = Order(arrivalDateTime++, orderID, orderType, OrderPricingType::LIMIT, OrderState::PENDING, targetQuantity, limitPrice);
            } else {
                order = Order(arrivalDateTime++, orderID, orderType, OrderPricingType::MARKET,OrderState::PENDING, targetQuantity, limitPrice);
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