#include "Order.h"
#include <iostream>

// Constructor
Order::Order(int arrivalDateTime, const std::string& orderID, OrderType orderType,
             OrderPricingType pricingType, OrderState orderState, long targetQuantity, float limitPrice)
    : arrivalDateTime(arrivalDateTime), orderID(orderID), orderType(orderType), pricingType(pricingType), 
      orderState(orderState), targetQuantity(targetQuantity), limitPrice(limitPrice) {}

// Getters
int Order::getArrivalDateTime() const { return arrivalDateTime; }
std::string Order::getOrderID() const { return orderID; }
OrderType Order::getOrderType() const { return orderType; }
OrderPricingType Order::getPricingType() const { return pricingType; }
OrderState Order::getOrderState() const { return orderState; }
long Order::getTargetQuantity() const { return targetQuantity; }
float Order::getLimitPrice() const { return limitPrice; }

// Setters
void Order::setArrivalDateTime(const int arrivalDateTime) { this->arrivalDateTime = arrivalDateTime; }
void Order::setOrderID(const std::string& orderID) { this->orderID = orderID; }
void Order::setOrderType(OrderType orderType) { this->orderType = orderType; }
void Order::setPricingType(OrderPricingType pricingType) { this->pricingType = pricingType; }
void Order::setOrderState(OrderState orderState) { this->orderState = orderState; }
void Order::setTargetQuantity(long targetQuantity) { this->targetQuantity = targetQuantity; }
void Order::setLimitPrice(float limitPrice) { this->limitPrice = limitPrice; }
