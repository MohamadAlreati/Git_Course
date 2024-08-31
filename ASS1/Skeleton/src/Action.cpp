#include <string>
#include <vector>
#include <algorithm>
#include "Action.h"

extern WareHouse *backup;

// BaseAction
BaseAction::BaseAction() : errorMsg(), status()
{
}
string BaseAction::getErrorMsg() const
{
    return this->errorMsg;
}
bool BaseAction::SearchForCollector(Order *order)
{
    return false;
}
bool BaseAction::SearchForDriver(Order *order)
{
    return false;
}
void BaseAction::complete()
{
    status = ActionStatus::COMPLETED;
}
void BaseAction::error(std::string errorMsg)
{
    this->errorMsg = "Error: " + errorMsg;
    status = ActionStatus::ERROR;
}
ActionStatus BaseAction::getStatus() const
{
    return this->status;
}
//----------------------------------------------------------------------------------------------
 // for (size_t j = 0; j < warehouse.getvolunteers().size(); j++)
    // {
    //     Volunteer *volunteer = warehouse.getvolunteers().at(j);

    //     if (!(volunteer->hasOrdersLeft()) && !(volunteer->isBusy()))
    //     {
    //         Volunteer *volunteerToDelete = volunteer;
    //         warehouse.getvolunteers().erase(std::remove(warehouse.getvolunteers().begin(), warehouse.getvolunteers().end(), volunteerToDelete), warehouse.getvolunteers().end());
    //         // popVolunteer1(warehouse, volunteerToDelete);
    //         delete volunteerToDelete;
    //     }
    // }
    // }
// SimulateStep
SimulateStep ::SimulateStep(int numOfSteps) : BaseAction(), numOfSteps(numOfSteps)
{
}
void SimulateStep::popIndex(WareHouse &warehouse, int i)
{
    warehouse.getpendingOrders().erase(warehouse.getpendingOrders().begin() + i);
}
void SimulateStep::popIndex1(WareHouse &warehouse, Order &toRemove)
{
    for (unsigned int i = 0; i < warehouse.getinProcessOrders().size(); i++)
    {
        if (warehouse.getinProcessOrders().at(i)->getId() == toRemove.getId())
        {
            warehouse.getinProcessOrders().erase(warehouse.getinProcessOrders().begin() + i);
            break;
        }
    }
}
void SimulateStep::Stage1(WareHouse &warehouse)
{
    //-----------stage 1----------
    for (size_t i = 0; i < warehouse.getpendingOrders().size(); i++)
    {
        Order *order = warehouse.getpendingOrders().at(i);
        if (order->getStringstatus() == "PENDING")
        {
            for (size_t j = 0; j < warehouse.getvolunteers().size(); j++)
            {
                Volunteer *volunteer = warehouse.getvolunteers().at(j);
                if (!(volunteer->isBusy()) && volunteer->canTakeOrder(*order))
                {
                    volunteer->acceptOrder(*order);
                    order->setStatus(OrderStatus::COLLECTING);
                    order->setCollectorId(volunteer->getId());
                    warehouse.getinProcessOrders().push_back(order);
                    popIndex(warehouse, i);
                    i--;
                    break;
                }
            }
        }
        else if (order->getStringstatus() == "COLLECTING")
        {

            for (size_t j = 0; j < warehouse.getvolunteers().size(); j++)
            {
                Volunteer *volunteer = warehouse.getvolunteers().at(j);
                if (!(volunteer->isBusy()) && volunteer->canTakeOrder(*order))
                {
                    volunteer->acceptOrder(*order);
                    order->setStatus((OrderStatus::DELIVERING));
                    order->setDriverId(volunteer->getId());
                    warehouse.getinProcessOrders().push_back(order);
                    popIndex(warehouse, i);
                    i--;
                    break;
                }
            }
        }
    }
}
void SimulateStep::Stage2(WareHouse &warehouse)
{
    for (size_t j = 0; j < warehouse.getvolunteers().size(); j++)
    {
        Volunteer *volunteer = warehouse.getvolunteers().at(j);

        if (volunteer->isBusy())
        {
            volunteer->step();
            if (!volunteer->isBusy())
            {
                for (size_t i = 0; i < warehouse.getpendingOrders().size(); i++)
                {
                    if (warehouse.getpendingOrders().at(i)->getCollectorId() != NO_VOLUNTEER)
                    {
                        Order *order = warehouse.getpendingOrders().at(i);
                        popIndex(warehouse, i);
                        warehouse.getinProcessOrders().push_back(order);
                    }
                }
            }
        }
    }

    /*
            volunteer->step();
                if(!volunteer->isBusy()){
                }
                for (size_t i = 0; i < warehouse.getpendingOrders().size(); i++) {
                   if(warehouse.getpendingOrders().at(i)->getCollectorId()!=NO_VOLUNTEER){
                     Order *order = war // for (size_t j = 0; j < warehouse.getvolunteers().size(); j++)
    // {
    //     Volunteer *volunteer = warehouse.getvolunteers().at(j);

    //     if (!(volunteer->hasOrdersLeft()) && !(volunteer->isBusy()))
    //     {
    //         Volunteer *volunteerToDelete = volunteer;
    //         warehouse.getvolunteers().erase(std::remove(warehouse.getvolunteers().begin(), warehouse.getvolunteers().end(), volunteerToDelete), warehouse.getvolunteers().end());
    //         // popVolunteer1(warehouse, volunteerToDelete);
    //         delete volunteerToDelete;
    //     }
    // }
    // }ehouse.getpendingOrders().at(i);
                     warehouse.getinProcessOrders().push_back(order);
            } */
}

void SimulateStep::Stage3(WareHouse &warehouse)
{
    for (size_t j = 0; j < warehouse.getvolunteers().size(); j++)
    {
        Volunteer *volunteer = warehouse.getvolunteers().at(j);

        if (!(volunteer->isBusy()) && (volunteer->getCompletedOrderId() != -1))
        {

            if ((warehouse.getOrder(volunteer->getCompletedOrderId())).getStringstatus() == "COLLECTING" && (warehouse.ShouldBeInProcess(warehouse.getOrder(volunteer->getCompletedOrderId()))))
            {
                warehouse.getpendingOrders().push_back(&warehouse.getOrder(volunteer->getCompletedOrderId()));
                popIndex1(warehouse, warehouse.getOrder(volunteer->getCompletedOrderId()));
            }

            else if ((warehouse.getOrder(volunteer->getCompletedOrderId())).getStringstatus() == "DELIVERING" && (warehouse.ShouldBeInProcess(warehouse.getOrder(volunteer->getCompletedOrderId()))))
            {
                warehouse.getcompletedOrders().push_back(&warehouse.getOrder(volunteer->getCompletedOrderId()));
                (warehouse.getOrder(volunteer->getCompletedOrderId())).setStatus(OrderStatus::COMPLETED);
                popIndex1(warehouse, warehouse.getOrder(volunteer->getCompletedOrderId()));
            }
        }
    }
}
void SimulateStep::popVolunteer1(WareHouse &warehouse, Volunteer *toRemove)
{
    vector<Volunteer*>& tmp = warehouse.getvolunteers(); // Use reference to vector
    tmp.erase(std::remove(tmp.begin(), tmp.end(), toRemove), tmp.end());
}
void SimulateStep::Stage4(WareHouse &warehouse)
{
    for (size_t j = 0; j < warehouse.getvolunteers().size(); j++)
    {
        Volunteer *volunteer = warehouse.getvolunteers().at(j);

        if (!(volunteer->hasOrdersLeft()) && !(volunteer->isBusy()))
        {
            Volunteer *volunteerToDelete = volunteer;
            warehouse.getvolunteers().erase(std::remove(warehouse.getvolunteers().begin(), warehouse.getvolunteers().end(), volunteerToDelete), warehouse.getvolunteers().end());
            // popVolunteer1(warehouse, volunteerToDelete);
            delete volunteerToDelete;
        }
    }
}

void SimulateStep ::act(WareHouse &warehouse)
{
    for (int i = 0; i < numOfSteps; i++)
    {
        SimulateStep::Stage1(warehouse);
        SimulateStep::Stage2(warehouse);
        SimulateStep::Stage3(warehouse);
        SimulateStep::Stage4(warehouse);
    // for (size_t j = 0; j < warehouse.getvolunteers().size(); j++)
    // {
    //     Volunteer *volunteer = warehouse.getvolunteers().at(j);

    //     if (!(volunteer->hasOrdersLeft()) && !(volunteer->isBusy()))
    //     {
    //         Volunteer *volunteerToDelete = volunteer;
    //         warehouse.getvolunteers().erase(std::remove(warehouse.getvolunteers().begin(), warehouse.getvolunteers().end(), volunteerToDelete), warehouse.getvolunteers().end());
    //         // popVolunteer1(warehouse, volunteerToDelete);
    //         delete volunteerToDelete;
    //     }
    // }
    }
}
//----------------------------------------------------------------------------------------------
// AddOrder
AddOrder::AddOrder(int id) : BaseAction(), customerId(id)
{
}
void AddOrder::act(WareHouse &warehouse)
{
    Customer *customer = nullptr;
    bool found = false;
    for (size_t i = 0; i < warehouse.getCustomers().size() && !found; i++)
    {
        if (warehouse.getCustomers().at(i)->getId() == this->customerId)
        {
            found = true;
            customer = warehouse.getCustomers().at(i);
            if (!(customer->canMakeOrder()))
            {
                error("cannot place this order");
                std ::cout << getErrorMsg() << std ::endl;
            }
            else
            {

                int orderId = warehouse.getOrdersCounter();
                warehouse.setOrdersCounter(orderId + 1);
                int Customer_Id = customer->getId();
                int Customer_Distance = customer->getCustomerDistance();
                Order *NewOrder = new Order(orderId, Customer_Id, Customer_Distance);
                warehouse.addOrder(NewOrder);
                customer->addOrder(orderId);
                customer->setOrdersCount(customer->getOrdersCount() + 1);
                complete();
            }
        }
    }
    if (!found)
    {
        error("cannot place this order");
        std ::cout << getErrorMsg() << std ::endl;
    }
}
//----------------------------------------------------------------------------------------------
// AddCustomer
AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders) : BaseAction(), customerName(customerName), customerType(StrType(customerType)), distance(distance), maxOrders(maxOrders)
{
}
void AddCustomer::act(WareHouse &warehouse)
{

    int CustomerNewId = warehouse.getCustomerCounter();
    if (customerType == CustomerType::Soldier)
    {
        Customer *NewCustomer = new SoldierCustomer(CustomerNewId, customerName, distance, maxOrders);
        warehouse.addCustomer(NewCustomer);
    }
    else if (customerType == CustomerType::Civilian)
    {
        Customer *NewCustomer = new CivilianCustomer(CustomerNewId, customerName, distance, maxOrders);
        warehouse.addCustomer(NewCustomer);
    }

    complete();
}
AddCustomer *AddCustomer::clone() const
{
    return new AddCustomer(*this);
}
CustomerType AddCustomer::StrType(const string &customerType)
{
    if (customerType == "soldier")
    {
        return CustomerType::Soldier;
    }
    else
    {
        return CustomerType::Civilian;
    }
}
PrintOrderStatus ::PrintOrderStatus(int id) : BaseAction(), orderId(id)
{
}
void PrintOrderStatus ::act(WareHouse &warehouse)
{
    Order *order;
    bool found = false;
    for (size_t i = 0; i < warehouse.getpendingOrders().size() && !found; i++)
    {
        if (warehouse.getpendingOrders().at(i)->getId() == orderId)
        {
            order = warehouse.getpendingOrders().at(i);
            found = true;
        }
    }
    for (size_t i = 0; i < warehouse.getinProcessOrders().size() && !found; i++)
    {
        if (warehouse.getinProcessOrders().at(i)->getId() == orderId)
        {
            order = warehouse.getinProcessOrders().at(i);
            found = true;
        }
    }
    for (size_t i = 0; i < warehouse.getcompletedOrders().size() && !found; i++)
    {
        if (warehouse.getcompletedOrders().at(i)->getId() == orderId)
        {
            order = warehouse.getcompletedOrders().at(i);
            found = true;
        }
    }
    if (found)
    {
        complete();
        std::cout << order->toString() << std::endl;
    }
    else
    {
        error("Order doesn't exist");
        std::cout << getErrorMsg() << std::endl;
    }
}
PrintOrderStatus *PrintOrderStatus::clone() const
{
    return new PrintOrderStatus(*this);
}
//----------------------------------------------------------------------------------------------
// PrintCustomerStatus
PrintCustomerStatus ::PrintCustomerStatus(int customerId) : BaseAction(), customerId(customerId)
{
}
void PrintCustomerStatus ::act(WareHouse &warehouse)
{
    Customer *MyCustomer;
    bool found = false;
    for (auto customer : warehouse.getCustomers())
    {
        if (customer->getId() == customerId)
        {
            MyCustomer = customer;
            found = true;
            break;
        }
    }

    if (found)
    {
        std::cout << "CustomerID:" + std::to_string(customerId) << std::endl;

        vector<int> orderIds = MyCustomer->getOrdersIds();

        vector<Order *> completedOrdersVector = warehouse.getcompletedOrders();
        for (size_t j = 0; j < completedOrdersVector.size(); j++)
        {
            if (completedOrdersVector.at(j)->getCustomerId() == customerId)
            {

                std::cout << "OrderID: " + std::to_string(completedOrdersVector.at(j)->getId()) << std::endl;
                std::cout << "OrderStatus: Completed" << std::endl;
            }
        }

        vector<Order *> ProcessOrdersVector = warehouse.getinProcessOrders();
        for (size_t k = 0; k < ProcessOrdersVector.size(); k++)
        {
            if (ProcessOrdersVector.at(k)->getCustomerId() == customerId)
            {
                if (ProcessOrdersVector.at(k)->Order::getStringstatus() == "COLLECTING")
                {
                    std::cout << "OrderID:" + std::to_string(ProcessOrdersVector.at(k)->getId()) << std::endl;
                    std::cout << "OrderStatus: Collecting" << std::endl;
                }
                else
                {
                    std::cout << "OrderID:" + std::to_string(ProcessOrdersVector.at(k)->getId()) << std::endl;
                    std::cout << "OrderStatus: Delivering" << std::endl;
                }
            }
        }
        vector<Order *> pendingOrdersVector = warehouse.getpendingOrders();
        for (size_t l = 0; l < pendingOrdersVector.size(); l++)
        {
            if (pendingOrdersVector.at(l)->getCustomerId() == customerId)
            {
                if (pendingOrdersVector.at(l)->Order::getStringstatus() == "COLLECTING")
                {
                    std::cout << "OrderID:" + std::to_string(pendingOrdersVector.at(l)->getId()) << std::endl;
                    std::cout << "OrderStatus: Collecting" << std::endl;
                }
                else
                {
                    std::cout << "OrderID:" + std::to_string(pendingOrdersVector.at(l)->getId()) << std::endl;
                    std::cout << "OrderStatus: Pending" << std::endl;
                }
            }
        }

        int numOredersLeft = MyCustomer->getMaxOrders() - MyCustomer->getNumOrders();
        std::cout << "numOrdersLeft: " + std::to_string(numOredersLeft) << std::endl;
        complete();
    }
    else
    {
        error("Customer doesn't exist");
        cout << getErrorMsg() << endl;
    }
}
PrintCustomerStatus *PrintCustomerStatus::clone() const
{
    return new PrintCustomerStatus(*this);
}
//----------------------------------------------------------------------------------------------
// PrintVolunteerStatus
PrintVolunteerStatus ::PrintVolunteerStatus(int id) : BaseAction(), volunteerId(id)
{
}
void PrintVolunteerStatus ::act(WareHouse &wareHouse)
{
    Volunteer *MyVolunteer;
    bool found = false;
    vector<Volunteer *> volunteerVector = wareHouse.getvolunteers();
    //--------------------------
    for (size_t i = 0; i < wareHouse.getvolunteers().size(); i++)
    {
        if (volunteerVector.at(i)->getId() == volunteerId)
        {
            MyVolunteer = volunteerVector.at(i);
            found = true;
        }
    }
    if (found)
    {
        std::cout << MyVolunteer->toString() << std::endl;
    }
    else
    {
        error("Error: Volunteer doesn't exist");
        std::cout<<"Error: Volunteer doesn't exist"<<std::endl;

    }
    wareHouse.addAction(this->clone());
}
PrintVolunteerStatus *PrintVolunteerStatus::clone() const
{
    return new PrintVolunteerStatus(*this);
}
PrintActionsLog ::PrintActionsLog() : BaseAction()
{
}
void PrintActionsLog::act(WareHouse &warehouse)
{
    std::vector<BaseAction *> temp = warehouse.getActions();

    for (size_t i = 0; i < warehouse.getActions().size(); i++)
    {
        std::cout << temp.at(i)->toString() << std::endl;
    }
    size_t size = temp.size();
    for (size_t i = 0; i < size; i++)
    {
        temp[i] = nullptr;
    }
    complete();
}
PrintActionsLog *PrintActionsLog::clone() const
{
    return new PrintActionsLog(*this);
}
//----------------------------------------------------------------------------------------------
// close
Close ::Close() : BaseAction()
{
}
void Close::act(WareHouse &wareHouse)
{
    for (size_t i = 0; i < wareHouse.getpendingOrders().size(); i++)
    {
        Order *order = wareHouse.getpendingOrders().at(i);
        if (order->getStringstatus() == "PENDING")
        {
            std::cout << "OrderId:" << order->getId() << " CustomerID:" << order->getCustomerId() << " OrderStatus: Pending" << std::endl;
        }
        else
        {
            std::cout << "OrderId:" << order->getId() << " CustomerID:" << order->getCustomerId() << " OrderStatus: Collecting" << std::endl;
        }
    }
    for (size_t i = 0; i < wareHouse.getinProcessOrders().size(); i++)
    {
        Order *order = wareHouse.getinProcessOrders().at(i);
        if (order->getStringstatus() == "COLLECTING")
        {
            std::cout << "OrderId:" << order->getId() << " CustomerID:" << order->getCustomerId() << " OrderStatus: Collecting" << std::endl;
        }
        else
        {
            std::cout << "OrderId:" << order->getId() << " CustomerID:" << order->getCustomerId() << " OrderStatus: Delivring" << std::endl;
        }
    }

    for (size_t i = 0; i < wareHouse.getcompletedOrders().size(); i++)
    {
        Order *order = wareHouse.getcompletedOrders().at(i);
        std::cout << "OrderId:" << order->getId() << " CustomerID:" << order->getCustomerId() << " OrderStatus: Completed" << std::endl;
    }
    wareHouse.Delete();
    complete();
    // should change the wareHouse status by calling:close();
    wareHouse.close();
}
Close *Close::clone() const
{
    return new Close(*this);
}
//----------------------------------------------------------------------------------------------
// BackupWareHouse
BackupWareHouse ::BackupWareHouse() : BaseAction()
{
}
void BackupWareHouse ::act(WareHouse &wareHouse)
{
    if (backup != nullptr)
    {
        delete backup;
    }
    backup = new WareHouse(wareHouse);
    complete();
}
//----------------------------------------------------------------------------------------------
BackupWareHouse *BackupWareHouse::clone() const
{
    return new BackupWareHouse(*this);
}
// RestoreWareHouse
RestoreWareHouse ::RestoreWareHouse() : BaseAction()
{
}
void RestoreWareHouse::act(WareHouse &wareHouse)
{
    if (backup == nullptr)
    {
        error("No backup available");
        std::cout << getErrorMsg() << std::endl;
    }
    else
    {

        wareHouse = *backup;

        complete();
    }
}
string SimulateStep::toString() const
{
    return "simulateStep " + std::to_string(numOfSteps) + " COMPLETED";
}
SimulateStep *SimulateStep::clone() const
{
    return new SimulateStep(*this);
}
string AddOrder::toString() const
{
    if (AddOrder::getStatus() == ActionStatus::COMPLETED)
    {
        return "order " + std::to_string(AddOrder::customerId) + " COMPLETED";
    }
    else
    {
        return "order " + std::to_string(AddOrder::customerId) + " ERROR";
    }
}
AddOrder *AddOrder::clone() const
{
    return new AddOrder(*this);
}
string RestoreWareHouse::toString() const
{
    if (RestoreWareHouse::getStatus() == ActionStatus::COMPLETED)
    {
        return "RestoreWareHouse COMPLETED";
    }
    else
    {
        return "restore ERROR";
    }
}
string BackupWareHouse::toString() const
{

    return "BackupWareHouse COMPLETED";
}
string Close::toString() const
{

    return "Close Completed";
}
string PrintActionsLog::toString() const
{
    return "log COMPLETED";
}

string PrintVolunteerStatus ::toString() const
{
    if (PrintVolunteerStatus::getStatus() == ActionStatus::COMPLETED)
    {
        return "volunteerStatus " + std::to_string(PrintVolunteerStatus::volunteerId) + " COMPLETED";
    }
    else
    {
        return "Error: Volunteer doesn't exist";
    }
}
string PrintCustomerStatus ::toString() const
{
    if (PrintCustomerStatus::getStatus() == ActionStatus::COMPLETED)
    {
        return "customerStatus " + std::to_string(PrintCustomerStatus::customerId) + " COMPLETED";
    }
    else
    {
        return "customerStatus " + std::to_string(PrintCustomerStatus::customerId) + " ERROR";
    }
}
string PrintOrderStatus ::toString() const
{
    if (PrintOrderStatus::getStatus() == ActionStatus::COMPLETED)
    {
        return "orderStatus " + std::to_string(PrintOrderStatus::orderId) + " COMPLETED";
    }
    else
    {
        return "orderStatus " + std::to_string(PrintOrderStatus::orderId) + " ERORR";
    }
}
string AddCustomer ::toString() const
{

    return "AddCustomer Completed";
}
RestoreWareHouse *RestoreWareHouse::clone() const
{
    return new RestoreWareHouse(*this);
}

// copy constructors

BaseAction::BaseAction(const BaseAction &other) : errorMsg(other.errorMsg), status(other.status) {}

// Copy constructor for SimulateStep
SimulateStep::SimulateStep(const SimulateStep &other) : numOfSteps(other.numOfSteps) {}

// Copy constructor for AddOrder
AddOrder::AddOrder(const AddOrder &other) : customerId(other.customerId) {}

// Copy constructor for AddCustomer
AddCustomer::AddCustomer(const AddCustomer &other)
    : customerName(other.customerName), customerType(other.customerType),
      distance(other.distance), maxOrders(other.maxOrders) {}

// Copy constructor for PrintOrderStatus
PrintOrderStatus::PrintOrderStatus(const PrintOrderStatus &other) : orderId(other.orderId) {}

// Copy constructor for PrintCustomerStatus
PrintCustomerStatus::PrintCustomerStatus(const PrintCustomerStatus &other) : customerId(other.customerId) {}

// Copy constructor for PrintVolunteerStatus
PrintVolunteerStatus::PrintVolunteerStatus(const PrintVolunteerStatus &other) : volunteerId(other.volunteerId) {}

// Copy constructor for PrintActionsLog
PrintActionsLog::PrintActionsLog(const PrintActionsLog &other) {}

// Copy constructor for Close
Close::Close(const Close &other) {}

// Copy constructor for BackupWareHouse
BackupWareHouse::BackupWareHouse(const BackupWareHouse &other) {}

// Copy constructor for RestoreWareHouse
RestoreWareHouse::RestoreWareHouse(const RestoreWareHouse &other) {}