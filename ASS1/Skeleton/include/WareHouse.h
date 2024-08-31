#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <queue>

#include "Action.h"
#include "Customer.h"
#include "Volunteer.h"
#include "Order.h"


// Warehouse responsible for Volunteers, Customers Actions, and Orders.
class BaseAction;
class Volunteer;
class WareHouse {

    public: 
        WareHouse();//
        WareHouse(const string &configFilePath);//
        void start();//
        void addOrder(Order* order);//
        void addAction(BaseAction* action);//
        //-------------------------------------------
        Customer &getCustomer(int customerId) const;//
        Volunteer &getVolunteer(int volunteerId) const;//
        Order &getOrder(int orderId) const;//
        const vector<BaseAction*> &getActions() const;//
        //----------------------------------------
        vector<Customer*>& getCustomers();//me
        vector<Order*>& getpendingOrders();//me
        vector<Order*>& getinProcessOrders();//me
        vector<Order*>& getcompletedOrders();//me
        vector<Volunteer*>& getvolunteers();//me
        void splitString(std::string s, std::queue <std::string> &queue, char c) ;//me(// this function splits a given string (s) on the basis of a given char ( c ) , and puts the sub-strings in a queu
        bool customerExists(int customerId) const;//me
        bool volunteerExists(int volunteerId) const;//me
        bool orderExists(int orderId) const;//me
        void close();//
        void open();//
        void Delete();
        //------------------------------------------
        //rule of 5
        WareHouse(const WareHouse &other);           
        WareHouse& operator=(const WareHouse &other);  
        WareHouse(WareHouse &&other) ;       
        WareHouse& operator=(WareHouse &&other) ;  
        ~WareHouse(); 
        
        int  getOrdersCounter() const;
        int  getCustomerCounter() const;
        int  getVolunteerCounter() const;
        void setOrdersCounter(int i);
        void setCustomerCounter(int i);
        void setVolunteerCounter(int i);
        void addCustomer(Customer* customer);
        bool ShouldBeInProcess(Order & order);
        

    private:
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int ordersCounter;//added
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        //--------------------------------------------
        void SimulationStep_Action(int NumOfSteps);
        void AddOrder_Action(int CustomerId);
        void AddCustomer_Action( const string &name,const string &StringCoustomerType,int CustomrtDistance,int Maxorder);
        void PrintOrderStatus_Action(int OrderID);
        void PrintCostomerStatus_Action(int CoustomerID);
        void PrintVolunteerStatus_Action(int volunteerID);
        void log_Action();
        void close_Action();
        void backupWareHouse_Action();
        void restoreWareHouse_Action();

};
