#include "Customer.h"
#include <iostream>
#include <sstream>


// Customer Class Implementation
Customer::Customer(int id, const std::string &name, int locationDistance, int maxOrders)
    : id(id), name(name), locationDistance(locationDistance), maxOrders(maxOrders),orderCount(0), ordersId() {
    vector <int> ordersId ; 

    }

int Customer::getOrdersCount(){
    return orderCount;
}

void Customer::setOrdersCount(int count){
    orderCount = count;
}

const std::string &Customer::getName() const {
    return name;
}

int Customer::getId() const {
    return id;
}

int Customer::getCustomerDistance() const {
    return locationDistance;
}

int Customer::getMaxOrders() const {
    return maxOrders;
}

int Customer::getNumOrders() const {
    return ordersId.size();
}

bool Customer::canMakeOrder() const {
    return orderCount < getMaxOrders();
}
//return the list of the cus' orders!
 std::vector<int> Customer::getOrdersIds()  {
    return ordersId;
}
//send -1 if we cant add the order!
int Customer::addOrder(int orderId) {
    if (canMakeOrder()) {
        ordersId.push_back(orderId);
        return orderId;
    } else {
        return -1;
    }
}

// SoldierCustomer Class Implementation
SoldierCustomer::SoldierCustomer(int id, const std::string &name, int locationDistance, int maxOrders)
    : Customer(id, name, locationDistance, maxOrders) {}

SoldierCustomer *SoldierCustomer::clone() const {
    return new SoldierCustomer(*this);
}


// CivilianCustomer Class Implementation
CivilianCustomer::CivilianCustomer(int id, const std::string &name, int locationDistance, int maxOrders)
    : Customer(id, name, locationDistance, maxOrders) {}

CivilianCustomer *CivilianCustomer::clone() const {
    return new CivilianCustomer(*this);
}
