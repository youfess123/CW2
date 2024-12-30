#ifndef ORDER_H
#define ORDER_H

#include <string>

enum class OrderType {
    BUYING_ORDER,
    SELLING_ORDER
};

enum class OrderPricingType {
    MARKET,
    LIMIT
};



class Order {
private:

    int arrivalTime;
    std::string orderID;
    OrderType orderType;
    OrderPricingType pricingType;
    long targetQuantity;
    long executedQuantity;
    float limitPrice;
    float executionPrice;


public:
    Order() :
    arrivalTime(0),
    orderID(""),
    orderType(OrderType::BUYING_ORDER),
    pricingType(OrderPricingType::MARKET),
    targetQuantity(0),
    limitPrice(0.0) {}

    Order(
        int arrivalDateTime,
        const std::string& orderID,
        OrderType orderType,
        OrderPricingType pricingType,
        long targetQuantity,
        float limitPrice
        );

    int getArrivalTime() const;
    std::string getOrderID() const;
    OrderType getOrderType() const;
    OrderPricingType getPricingType() const;
    long getTargetQuantity() const;
    long getExecutedQuantity() const;
    float getLimitPrice() const;
    float getExecutionPrice() const;

    void setArrivalTime( int arrivalDateTime);
    void setOrderID(const std::string& orderID);
    void setOrderType(OrderType orderType);
    void setPricingType(OrderPricingType pricingType);
    void setTargetQuantity(long targetQuantity);
    void setExecutedQuantity(long executedQuantity);
    void setLimitPrice(float limitPrice);
    void setExecutionPrice(float executionPrice);

};
std::ostream& operator<<(std::ostream& os, const Order& order);
bool isBuyOrder(const Order& order);
bool isMarketOrder(const Order& order);
#endif//ORDER_H

