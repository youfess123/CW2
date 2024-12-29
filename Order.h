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

// enum class OrderState {
//     PENDING,
//     EXECUTED,
//     UNEXECUTED
// };

class Order {
private:

    int arrivalDateTime;
    std::string orderID;
    OrderType orderType;
    OrderPricingType pricingType;
    // OrderState orderState;
    long targetQuantity;
    float limitPrice;


public:
    Order() :
    arrivalDateTime(0),
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
        // OrderState orderState,
        long targetQuantity,
        float limitPrice
        );

    int getArrivalDateTime() const;
    std::string getOrderID() const;
    OrderType getOrderType() const;
    OrderPricingType getPricingType() const;
    // OrderState getOrderState() const;
    long getTargetQuantity() const;
    float getLimitPrice() const;

    void setArrivalDateTime( int arrivalDateTime);
    void setOrderID(const std::string& orderID);
    void setOrderType(OrderType orderType);
    void setPricingType(OrderPricingType pricingType);
    // void setOrderState(OrderState orderState);
    void setTargetQuantity(long targetQuantity);
    void setLimitPrice(float limitPrice);

};
std::ostream& operator<<(std::ostream& os, const Order& order);
bool checkOrderType(const Order& order);
bool checkOrderPricingType(const Order& order);
#endif//ORDER_H

