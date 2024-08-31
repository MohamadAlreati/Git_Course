#pragma once
#include <string>
#include <vector>
#include "WareHouse.h"
#include "Order.h"
#include "Volunteer.h"

class WareHouse;

using std::string;
using std::vector;

enum class ActionStatus
{
    COMPLETED,
    ERROR
};

enum class CustomerType
{
    Soldier,
    Civilian
};

class BaseAction
{
public:
    BaseAction();
    virtual ~BaseAction(){};
    BaseAction(const BaseAction &other);
    virtual void act(WareHouse &wareHouse) = 0;
    virtual string toString() const = 0;
    virtual BaseAction *clone() const = 0;
    ActionStatus getStatus() const; //
protected:
    void complete();             //
    void error(string errorMsg); //
    string getErrorMsg() const;  //
private:
    string errorMsg;
    ActionStatus status;
    bool SearchForCollector(Order *order);
    bool SearchForDriver(Order *order);
};

class SimulateStep : public BaseAction
{

public:
    SimulateStep(const SimulateStep &other);
    void Stage1(WareHouse &WareHouse);
    void Stage2(WareHouse &WareHouse);
    void Stage3(WareHouse &warehouse);
    void Stage4(WareHouse &warehouse);
    SimulateStep(int numOfSteps);            //
    void act(WareHouse &wareHouse) override; //
    std::string toString() const override;   //
    SimulateStep *clone() const override;
    ~SimulateStep() = default;
    void popIndex(WareHouse &WareHouse, int i);
    void popIndex1(WareHouse &WareHouse, Order &toRemove);
    void popVolunteer1(WareHouse &WareHouse, Volunteer *toRemove);

private:
    const int numOfSteps;
};

class AddOrder : public BaseAction
{
public:
    AddOrder(int id);
    void act(WareHouse &wareHouse) override;
    string toString() const override; //
    AddOrder *clone() const override;
    ~AddOrder() = default;
    AddOrder(const AddOrder &other);
private:
    const int customerId;
};

class AddCustomer : public BaseAction
{
public:
    AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders);
    void act(WareHouse &wareHouse) override;
    AddCustomer *clone() const override;
    string toString() const override;
    ~AddCustomer() = default;
    AddCustomer(const AddCustomer &other);
private:
    const string customerName;
    const CustomerType customerType;
    const int distance;
    const int maxOrders;
    CustomerType StrType(const string &customerType); // conver from string to CustomerType!
};

class PrintOrderStatus : public BaseAction
{
public:
    PrintOrderStatus(int id);
    void act(WareHouse &wareHouse) override;
    PrintOrderStatus *clone() const override;
    string toString() const override;
    ~PrintOrderStatus() = default;
    PrintOrderStatus(const PrintOrderStatus &other);
private:
    const int orderId;
};

class PrintCustomerStatus : public BaseAction
{
public:
    PrintCustomerStatus(int customerId);
    void act(WareHouse &wareHouse) override;
    PrintCustomerStatus *clone() const override;
    string toString() const override;
    ~PrintCustomerStatus() = default;
     PrintCustomerStatus(const PrintCustomerStatus &other);
private:
    const int customerId;
};

class PrintVolunteerStatus : public BaseAction
{
public:
    PrintVolunteerStatus(int id);
    void act(WareHouse &wareHouse) override;
    PrintVolunteerStatus *clone() const override;
    string toString() const override;
    ~PrintVolunteerStatus() = default;
     PrintVolunteerStatus(const PrintVolunteerStatus &other);
private:
    const int volunteerId;
};

class PrintActionsLog : public BaseAction
{
public:
    PrintActionsLog();
    void act(WareHouse &wareHouse) override;
    PrintActionsLog *clone() const override;
    string toString() const override;
    ~PrintActionsLog() = default;
    PrintActionsLog(const PrintActionsLog &other);
private:
};

class Close : public BaseAction
{
public:
    Close();
    void act(WareHouse &wareHouse) override;
    Close *clone() const override;
    string toString() const override; //
    ~Close() = default;
    Close(const Close &other);
private:
};

class BackupWareHouse : public BaseAction
{
public:
    BackupWareHouse();
    void act(WareHouse &wareHouse) override;
    BackupWareHouse *clone() const override;
    string toString() const override; //
    ~BackupWareHouse() = default;
    BackupWareHouse(const BackupWareHouse &other);
private:
};

class RestoreWareHouse : public BaseAction
{
public:
    RestoreWareHouse();
    void act(WareHouse &wareHouse) override;
    RestoreWareHouse *clone() const override;
    string toString() const override; //
    ~RestoreWareHouse() = default;
     RestoreWareHouse(const RestoreWareHouse &other);
private:
};