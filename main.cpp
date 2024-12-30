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


void printUnexecutedOrders(const std::vector<Order>& buyOrders, const std::vector<Order>& sellOrders) {//called after program is done checks for left over shares
    for (const auto& order : buyOrders) {
        if (order.getTargetQuantity() > 0) {
            std::cout << "order " << order.getOrderID() << " " << order.getTargetQuantity() << " shares unexecuted\n";
        }
    }
    for (const auto& order : sellOrders) {
        if (order.getTargetQuantity() > 0) {
            std::cout << "order " << order.getOrderID() << " " << order.getTargetQuantity() << " shares unexecuted\n";
        }
    }
}


void printExecutedOrders(const std::vector<Order> &executedOrders) {
    std::cout << "Executed Orders \n";
    std::cout << "-------------------\n";
    for (const auto& order : executedOrders) {
        std::string str;
        isBuyOrder(order)? str="purchased": str="sold";
        std::cout<< "order " << order.getOrderID() << " " << order.getExecutedQuantity()<< " shares "+ str+" at price "<< order.getExecutionPrice()<< std::endl;
    }

}


float determineExecutionPrice(const Order& buyOrder, const Order& sellOrder) {
    if (isMarketOrder(sellOrder) && isMarketOrder(buyOrder)) {
        return previousTransactionPrice;  // Market&Market = previous transaction price used
    }

    if (!isMarketOrder(buyOrder) && !isMarketOrder(sellOrder)) { //Limit & Limit = earliest arrival time between order's price used
        return (
            sellOrder.getArrivalTime() < buyOrder.getArrivalTime())
                ? sellOrder.getLimitPrice(): buyOrder.getLimitPrice();
    }

    return isMarketOrder(sellOrder) ? buyOrder.getLimitPrice() : sellOrder.getLimitPrice(); //Non market order's limit price is used
}


bool canMatchOrder(const Order& buyOrder, const Order& sellOrder) {
    return (isMarketOrder(buyOrder) || isMarketOrder(sellOrder)||buyOrder.getLimitPrice() >= sellOrder.getLimitPrice());
    //one market order or both or buy order's limit price is higher than sell= orders are matchable
}


bool isOrderExecutable(const Order& buyOrder, const Order& sellOrder) {
    return buyOrder.getTargetQuantity() > 0 && sellOrder.getTargetQuantity() > 0 && canMatchOrder(buyOrder, sellOrder);// check if there are shares left to be traded
}


void updateOrderAfterExecution(Order& buyOrder, Order& sellOrder, int executedQuantity, float executionPrice) {//update order so it can be used again and set price and traded shares
    sellOrder.setTargetQuantity(sellOrder.getTargetQuantity() - executedQuantity);
    buyOrder.setTargetQuantity(buyOrder.getTargetQuantity() - executedQuantity);

    sellOrder.setExecutionPrice(executionPrice);
    buyOrder.setExecutionPrice(executionPrice);

    sellOrder.setExecutedQuantity(executedQuantity);
    buyOrder.setExecutedQuantity(executedQuantity);
}


void processOrderExecution(Order& buyOrder, Order& sellOrder) {
    float executionPrice = determineExecutionPrice(buyOrder, sellOrder);
    int executedQuantity = std::min(buyOrder.getTargetQuantity(), sellOrder.getTargetQuantity());

    previousTransactionPrice = executionPrice;

    updateOrderAfterExecution(buyOrder, sellOrder, executedQuantity, executionPrice);

    executedOrders.push_back(buyOrder);
    executedOrders.push_back(sellOrder);
}


void executeOrders(std::vector<Order>& buyOrders, std::vector<Order>& sellOrders) {

    for (auto& buyOrder : buyOrders) {
        for (auto& sellOrder : sellOrders) {

            if (isOrderExecutable(buyOrder, sellOrder)) {
                processOrderExecution(buyOrder, sellOrder);
            }
        }
    }
}


void printPendingOrders(const std::vector<Order> &buyOrders, const std::vector<Order> &sellOrders, float previousTransactionPrice) {

    std::cout << "Last trading price: " << previousTransactionPrice << "\n";
    std::cout << "Buy Orders\n";
    std::cout << "-------------------\n";

    for (const auto& order : buyOrders) {
        if(order.getTargetQuantity() != 0) {//only prints if order is not fully executed
            if (isMarketOrder(order)) {
                std::cout << order.getOrderID() << " M " << order.getTargetQuantity() << std::endl;
            } else {
                std::cout << order.getOrderID() << " "
                          << order.getLimitPrice() << " "
                          << order.getTargetQuantity() << std::endl;
            }
        }
    }
    std::cout << "\n";

    std::cout << "Sell Orders\n";
    std::cout << "-------------------\n";
    for (const auto& order : sellOrders) {
        if(order.getTargetQuantity() != 0) {
            if (isMarketOrder(order)) {
                std::cout << order.getOrderID() << " M " << order.getTargetQuantity() << std::endl;
            } else {
                std::cout << order.getOrderID() << " "
                          << order.getLimitPrice() << " "
                          << order.getTargetQuantity() << std::endl;
            }
        }
    }
    std::cout << "\n";
}


bool compareOrders(const Order& a,const Order& b,bool isBuyOrder) {
    if (isMarketOrder(a) && !isMarketOrder(b)) { //priority is given to market order over limit order
        return true;
    }
    if (!isMarketOrder(a) && isMarketOrder(b)) { //priority is given to market order over limit order
        return false;
    }
    if (a.getLimitPrice() == b.getLimitPrice()) {
        return a.getArrivalTime() < b.getArrivalTime(); //if price is the same priority is given to the earliest arrival time
    }
    return isBuyOrder ? a.getLimitPrice() > b.getLimitPrice() : a.getLimitPrice() < b.getLimitPrice();//sorts buy orders on highest price and sell orders on lowest price
}


void sortOrderPriority(std::vector<Order>& buyOrders, std::vector<Order>& sellOrders) {

    std::sort(buyOrders.begin(), buyOrders.end(), [](const Order& a, const Order& b) {
        return compareOrders(a, b, true);
    });
    std::sort(sellOrders.begin(), sellOrders.end(), [](const Order& a, const Order& b) {
        return compareOrders(a, b, false);
    });
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
            Order order;

            if (stringStream >> limitPrice) {
                order = Order(arrivalDateTime++, orderID, orderType, OrderPricingType::LIMIT, targetQuantity, limitPrice);
            } else {
                order = Order(arrivalDateTime++, orderID, orderType, OrderPricingType::MARKET, targetQuantity, 0.0);// market hasno limit price
            }
            if (isBuyOrder(order)) {
                buyOrders.push_back(order);

            } else {
                sellOrders.push_back(order);
            }
        }
        sortOrderPriority(buyOrders, sellOrders);
        printPendingOrders(buyOrders, sellOrders, previousTransactionPrice);
        executeOrders(buyOrders, sellOrders);
    }
    printExecutedOrders(executedOrders);
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

    printUnexecutedOrders(buyOrders, sellOrders);

    return 0;
}