#include "Order.h"
#include <iostream>
#include <sstream>

Order::Order(int id, int customerId, int distance)
    : id(id), customerId(customerId), distance(distance), status(OrderStatus::PENDING), collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER) {}

int Order::getId() const
{
    return id;
}

int Order::getCustomerId() const
{
    return customerId;
}

void Order::setStatus(OrderStatus status)
{
    this->status = status;
}

void Order::setCollectorId(int collectorId)
{
    this->collectorId = collectorId;
}

void Order::setDriverId(int driverId)
{
    this->driverId = driverId;
}

int Order::getCollectorId() const
{
    return collectorId;
}

int Order::getDriverId() const
{
    return driverId;
}

OrderStatus Order::getStatus() const
{
    return status;
}
int Order::getDistance() const
{
    return distance;
}
std::string Order::getStringstatus() const
{
     std::ostringstream oss;
    if(status == OrderStatus::PENDING)
    {
      oss << "PENDING";
    }
    else if(status == OrderStatus::COLLECTING)
    {
        oss << "COLLECTING";
    }
    else if(status == OrderStatus::DELIVERING)
    {
        oss << "DELIVERING";
    }
    else if(status == OrderStatus::COMPLETED)
    {
        oss << "COMPLETED";
    }
    return oss.str();  

}

const string Order::toString() const
{
    string s = getStringstatus();
    std::ostringstream oss;
    if (s == "PENDING")
    {
        oss << "OrderId: " << getId() << "\n"
            << "OrderStatus:" << " Pending" << "\n"
            << "CustomerId: " << getCustomerId() << "\n"
            << "Collector:" << " None" << "\n"
            << "Driver:" << " None";

       
    }
    else if (s == "COLLECTING")
    {
        oss << "OrderId: " << getId() << "\n"
            << "OrderStatus:" << " Collecting" << "\n"
            << "CustomerId: " << getCustomerId() << "\n"
            << "Collector: " << getCollectorId() << "\n"
            << "Driver:" << " None";
           
    }else if (s == "DELIVERING")
    {
        oss << "OrderId: " << getId() << "\n"
            << "OrderStatus:" << " Delivering" << "\n"
            << "CustomerId: " << getCustomerId() << "\n"
            << "Collector: " << getCollectorId() << "\n"
            << "Driver: " <<  getDriverId();
           
    }else{
        oss << "OrderId: " << getId() << "\n"
            << "OrderStatus:" << " Completed" << "\n"
            << "CustomerId: " << getCustomerId() << "\n"
            << "Collector: " << getCollectorId() << "\n"
            << "Driver: " << getDriverId() << "\n";
    }
        
    return oss.str();
}
Order :: Order( const Order& other):
     id(other.id) ,customerId(other.customerId),distance(other.distance),status(other.status),collectorId(other.collectorId),driverId(other.driverId)

{}

Order *Order::clone(){
    return new Order(*this);
}