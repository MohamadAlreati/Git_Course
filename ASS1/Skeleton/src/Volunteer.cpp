#include "Volunteer.h"
#include <iostream>
#include <sstream>


// Volunteer Class Implementation
Volunteer::Volunteer(int id, const std::string &name)
    :completedOrderId(NO_ORDER), activeOrderId(NO_ORDER), id(id), name(name){}

int Volunteer::getId() const
{
    return id;
}

const std::string &Volunteer::getName() const
{
    return name;
}

int Volunteer::getActiveOrderId() const
{
    return activeOrderId;
}
// the last completed one!
int Volunteer::getCompletedOrderId() const
{
    return completedOrderId;
}
bool Volunteer::isBusy() const
{
     if(activeOrderId == NO_ORDER){
        return false;

     }
     return true;
     
}


// CollectorVolunteer Class Implementation
CollectorVolunteer::CollectorVolunteer(int id, const std::string &name, int coolDown)
    : Volunteer(id, name), coolDown(coolDown), timeLeft(coolDown) {}

CollectorVolunteer *CollectorVolunteer::clone() const
{
    return new CollectorVolunteer(*this);
}


void CollectorVolunteer::step()
{
    if (decreaseCoolDown())
    {       
       completedOrderId = activeOrderId;
       activeOrderId = NO_ORDER;
       
    }
}

int CollectorVolunteer::getCoolDown() const
{
    return coolDown;
}

int CollectorVolunteer::getTimeLeft() const
{
    return timeLeft;
}

bool CollectorVolunteer::hasOrdersLeft() const
{
    return true; // CollectorVolunteer always has orders left
}
bool CollectorVolunteer::decreaseCoolDown()
{
    timeLeft--;
    return timeLeft == 0;
}

bool CollectorVolunteer::canTakeOrder(const Order &order) const
{
    return !isBusy() && order.getStatus() == OrderStatus::PENDING;
}

void CollectorVolunteer::acceptOrder(const Order &order)
{
    if (canTakeOrder(order))
    {   RecentFinsh=false;
        activeOrderId = order.getId();
        timeLeft = coolDown;
        // the oreder status still pending(how to change it )
    }
    else
    {
        throw std::out_of_range("cant accept this order!");
    }
}

std::string CollectorVolunteer::toString() const
{  
    string StringOrderID=std::to_string(getActiveOrderId());
    if(StringOrderID=="-1"){
        StringOrderID="None";
    }
    string b;
    if(isBusy())
    b = "true";
    else
    b= "false";
    string time;
    if(getTimeLeft()==0)
    time = "None";
    else
    time = std::to_string(getTimeLeft());
    std::ostringstream oss;
        oss << "VolunteerID: " << getId() << "\n"
            << "isBusy: " + b << "\n"
            << "OrderId: " << StringOrderID << "\n"
            << "TimeLeft: " << time << "\n"
            << "OrdersLeft: " << "No Limit";
    return oss.str();     

}

// LimitedCollectorVolunteer Class Implementation
LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const std::string &name, int coolDown, int maxOrders)
    : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const
{
    return new LimitedCollectorVolunteer(*this);
}

bool LimitedCollectorVolunteer::hasOrdersLeft() const
{
    return ordersLeft > 0;
}

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const
{
    return CollectorVolunteer::canTakeOrder(order) && hasOrdersLeft();
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order)
{
    if (canTakeOrder(order))
    {   RecentFinsh=false;
        CollectorVolunteer::acceptOrder(order);
        // should change the preder status somewhere!
        --ordersLeft;
    }
}

int LimitedCollectorVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedCollectorVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

std::string LimitedCollectorVolunteer::toString() const
{
   
    string StringOrderID=std::to_string(getActiveOrderId());
    if(StringOrderID=="-1"){
        StringOrderID="None";
    }
    string b;
    if(isBusy())
    b = "true";
    else
    b= "false";
    string time;
    if(getTimeLeft()==0)
    time = "None";
    else
    time = std::to_string(getTimeLeft());
    std::ostringstream oss;
        oss << "VolunteerID: " << getId() << "\n"
            << "isBusy: " +b << "\n"
            << "OrderId: " << StringOrderID << "\n"
            << "TimeLeft: " << time << "\n"
            << "OrdersLeft: " << getNumOrdersLeft() ;
    return oss.str();     
        
}
// DriverVolunteer Class Implementation
DriverVolunteer::DriverVolunteer(int id, const std::string &name, int maxDistance, int distancePerStep)
    : Volunteer(id, name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(maxDistance) {}

DriverVolunteer *DriverVolunteer::clone() const
{
    return new DriverVolunteer(*this);
}

int DriverVolunteer::getDistanceLeft() const
{
    return distanceLeft;
}

int DriverVolunteer::getMaxDistance() const
{
    return maxDistance;
}

int DriverVolunteer::getDistancePerStep() const
{
    return distancePerStep;
}

bool DriverVolunteer::decreaseDistanceLeft()
{
   distanceLeft = distanceLeft - distancePerStep;
   if( 0 >= getDistanceLeft())
   {
    distanceLeft = 0;
    return true;
   }
   else
   {
    return false;
   }
}

bool DriverVolunteer::hasOrdersLeft() const
{
    return true; // DriverVolunteer always has orders left
}

bool DriverVolunteer::canTakeOrder(const Order &order) const
{
    return !isBusy() && order.getStatus() == OrderStatus::COLLECTING && order.getDistance() <= getMaxDistance();
}
//--------------------------------------------------------------
void DriverVolunteer::acceptOrder(const Order &order)
{
    if (canTakeOrder(order))
    {    RecentFinsh=false;
        activeOrderId = order.getId();
        distanceLeft = order.getDistance();
    }
    else
    {
        throw std::out_of_range("Driver:cant accept this order!");
    }
}

void DriverVolunteer::step()
{
    
    if (decreaseDistanceLeft())
    {   
        
        completedOrderId = activeOrderId;
        activeOrderId=NO_ORDER;

    }
}

std::string DriverVolunteer::toString() const
{
    
    string StringOrderID=std::to_string(getActiveOrderId());
    if(StringOrderID=="-1"){
        StringOrderID="None";
    }
    string b;
    if(isBusy())
    b = "true";
    else
    b= "false";
    string d;
    if(getDistanceLeft()==0)
    d = "None";
    else
    d = std::to_string(getDistanceLeft());
    std::ostringstream oss;
        oss << "VolunteerID: " << getId() << "\n"
            << "isBusy: " +b << "\n"
            << "OrderId: " << StringOrderID << "\n"
            << "TimeLeft: " << d << "\n"
            << "OrdersLeft: No Limit";
    return oss.str();     
}

// LimitedDriverVolunteer Class Implementation
LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const std::string &name, int maxDistance, int distancePerStep, int maxOrders)
    : DriverVolunteer(id, name, maxDistance, distancePerStep), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const
{
    return new LimitedDriverVolunteer(*this);
}

int LimitedDriverVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedDriverVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

bool LimitedDriverVolunteer::hasOrdersLeft() const
{
    return ordersLeft > 0;
}

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const
{
    return DriverVolunteer::canTakeOrder(order) && hasOrdersLeft();
}

void LimitedDriverVolunteer::acceptOrder(const Order &order)
{
    if (canTakeOrder(order))
    {   RecentFinsh=false;
        DriverVolunteer::acceptOrder(order);
        ordersLeft--;
    }
}

std::string LimitedDriverVolunteer::toString() const
{
  
    string StringOrderID=std::to_string(getActiveOrderId());
    if(StringOrderID=="-1"){
        StringOrderID="None";
    }
    string b;
    if(isBusy())
    b = "true";
    else
    b= "false";
    string d;
    if(getDistanceLeft()==0)
    d = "None";
    else
    d = std::to_string(getDistanceLeft());
    std::ostringstream oss;
        oss << "VolunteerID: " << getId() << "\n"
            << "isBusy: " +b << "\n"
            << "OrderId:" << StringOrderID << "\n"
            << "TimeLeft: " << d << "\n"
            << "OrdersLeft: "<<getNumOrdersLeft();
    return oss.str();     
        
}

string CollectorVolunteer::stringGetType()const
{
    return "CollectorVolunteer";
}
string LimitedCollectorVolunteer::stringGetType()const
{
    return "LimitedCollectorVolunteer";
}
string DriverVolunteer::stringGetType()const 
{
    return "DriverVolunteer";
}
string LimitedDriverVolunteer::stringGetType()const 
{
    return "LimitedDriverVolunteer";
}

// Destructor
Volunteer::~Volunteer()
{
    // No dynamic memory allocated, no need to delete
}

