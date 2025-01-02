#include <algorithm>
#include <iostream>
#include "Order.h"
#include <fstream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <filesystem>


std::vector<Order> buyOrders;
std::vector<Order> sellOrders;
std::vector<Order> executedOrders;
float previousTransactionPrice;


bool compareByArrivalTime(const Order& a, const Order& b) {
    return a.getArrivalTime() < b.getArrivalTime();
}


void printUnexecutedOrders(const std::vector<Order>& buyOrders, const std::vector<Order>& sellOrders, std::ofstream& outFile) {//called after program is done checks for left over shares

    std::vector<Order> unexecutedOrders;
    for (const auto& order : buyOrders) {
        if (order.getTargetQuantity() > 0) {//make sure trade quantity isnt fully executed
            unexecutedOrders.push_back(order);
        }
    }
    for (const auto& order : sellOrders) {
        if (order.getTargetQuantity() > 0) {
            unexecutedOrders.push_back(order);
        }
    }
    std::sort(unexecutedOrders.begin(), unexecutedOrders.end(), compareByArrivalTime);//sorts all unexecuted by arrival time to uphold rules
    for(const auto& order : unexecutedOrders) {
        outFile << "order " << order.getOrderID() << " " << order.getTargetQuantity() << " shares unexecuted\n";
    }
}


void printExecutedOrders(const std::vector<Order> &executedOrders, std::ofstream& outFile) {
    for (const auto& order : executedOrders) {
        std::string str;
        isBuyOrder(order)? str="purchased": str="sold";
        outFile<< "order " << order.getOrderID() << " " << order.getExecutedQuantity()<< " shares "+ str+" at price "<< order.getExecutionPrice()<< std::endl;
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
    //one market order or both market orders or buy order's limit price is higher than sell's limit price means orders are matchable
}


bool isOrderExecutable(const Order& buyOrder, const Order& sellOrder) {
    return buyOrder.getTargetQuantity() > 0 && sellOrder.getTargetQuantity() > 0 && canMatchOrder(buyOrder, sellOrder);// check if there are shares left to be traded
}


void updateOrderAfterExecution(Order& buyOrder, Order& sellOrder, int executedQuantity, float executionPrice) {//update order so it can be used again and set executioin price and traded shares to be in output file
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


void printPendingOrders(const std::vector<Order> &buyOrders, const std::vector<Order> &sellOrders, float previousTransactionPrice) {
    std::cout << "-------------------\n";
    std::cout << "Last trading price: " << previousTransactionPrice << "\n";
    std::cout << "\n";
    std::cout << "Buy Orders\n";

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


void executeOrders(std::vector<Order>& buyOrders, std::vector<Order>& sellOrders) {

    for (auto& buyOrder : buyOrders) {
        for (auto& sellOrder : sellOrders) {

            if (isOrderExecutable(buyOrder, sellOrder)) {

                processOrderExecution(buyOrder, sellOrder);
                std::cout << "Executed Orders\n";
                std::cout<< "order " << buyOrder.getOrderID() << " " << buyOrder.getExecutedQuantity()<< " shares purchased at price "<< buyOrder.getExecutionPrice()<< std::endl;
                std::cout<< "order " << sellOrder.getOrderID() << " " << sellOrder.getExecutedQuantity()<< " shares sold at price "<< sellOrder.getExecutionPrice()<< std::endl;
                std::cout << "\n";

            }
        }
    }
}


bool compareOrders(const Order& a,const Order& b,bool isBuyOrder) {
    if (isMarketOrder(a) && !isMarketOrder(b)) { //priority is given to market order over limit order
        return true;
    }
    if (!isMarketOrder(a) && isMarketOrder(b)) { //priority is given to market order over limit order
        return false;
    }
    if (a.getLimitPrice() == b.getLimitPrice()) {
        return compareByArrivalTime(a,b); //if price is the same priority is given to the earliest arrival time
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


void readOrdersFromFile(std::ifstream& inFile) {

    inFile >> previousTransactionPrice;

    std::string orderID, orderTypeStr;
    int arrivalDateTime=1;
    long targetQuantity;
    float limitPrice;


    std::string line;
    while (std::getline (inFile, line) ) {

        std::istringstream stringStream(line);
        if (stringStream >> orderID >> orderTypeStr >> targetQuantity) {

            OrderType orderType = (orderTypeStr == "B") ? OrderType::BUYING_ORDER : OrderType::SELLING_ORDER;
            Order order;

            if (stringStream >> limitPrice) {
                order = Order(arrivalDateTime++, orderID, orderType, OrderPricingType::LIMIT, targetQuantity, limitPrice);
            } else {
                order = Order(arrivalDateTime++, orderID, orderType, OrderPricingType::MARKET, targetQuantity, 0.0);// market has no limit price
            }
            if (isBuyOrder(order)) {
                buyOrders.push_back(order);

            } else {
                sellOrders.push_back(order);
            }
        }else {
            continue;
        }
        sortOrderPriority(buyOrders, sellOrders);
        printPendingOrders(buyOrders, sellOrders, previousTransactionPrice);
        executeOrders(buyOrders, sellOrders);
    }
    printPendingOrders(buyOrders, sellOrders, previousTransactionPrice);//print
}


int main(int argc, char* argv[]) {
    std::cout << std::fixed << std::setprecision(2);

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <inFileName>\n";
        return 1;
    }

    std::string inFileName = argv[1];
    std::ifstream inFile(inFileName);

    if (!inFile) {
        std::cerr << "Error: Could not open file " << inFileName <<"\n";
        return 1;
    }
    readOrdersFromFile(inFile);

    std::string baseName = std::filesystem::path(inFileName).filename().string();

    std::string fileNumber;
    for(char c : baseName) {
        if(isdigit(c)) {
            fileNumber+=c;
        }
    }

    std::string outFileName= "output"+fileNumber+".txt";
    std::ofstream outFile(outFileName);
    outFile << std::fixed << std::setprecision(2);
    printExecutedOrders(executedOrders, outFile);
    printUnexecutedOrders(buyOrders, sellOrders, outFile);

    inFile.close();
    outFile.close();

    return 0;
}