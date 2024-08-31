#include "WareHouse.h"
#include <vector>
#include <string>

// WareHouse responible for Volunteers,Coustomers Action,and Orders.

WareHouse::WareHouse(const string &configFilePath) : isOpen(false), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), ordersCounter(0), customerCounter(0), volunteerCounter(0)
{
  std::ifstream inFile;
  inFile.open(configFilePath);
  if (!inFile)
  {
    std::cerr << "Unable to open file datafile.txt";
    exit(1);
  }
  std::string line;
  while (getline(inFile, line))
  {
    if (!(line.empty()) && line.at(0) != '#')
    {
      std::queue<std::string> lineInputs;
      splitString(line, lineInputs, ' ');
      if (lineInputs.front() == "customer")
      {
        lineInputs.pop();
        string name = lineInputs.front();
        lineInputs.pop();
        string type = lineInputs.front();
        lineInputs.pop();
        int distance = stoi(lineInputs.front());
        lineInputs.pop();
        int max_orders = stoi(lineInputs.front());
        lineInputs.empty();
        if (type == "soldier")
        {
          Customer *Mycustomer = new SoldierCustomer(customerCounter, name, distance, max_orders);
          customers.push_back(Mycustomer);
          customerCounter++;        }
        else if (type == "civilian")
        {
          Customer *Mycustomer = new CivilianCustomer(customerCounter, name, distance, max_orders);
          customers.push_back(Mycustomer);
          customerCounter++;
        }
      }
      else if (lineInputs.front() == "volunteer")
      {
        lineInputs.pop();
        string name = lineInputs.front();
        lineInputs.pop();
        string type = lineInputs.front();
        if (type == "collector")
        {
          lineInputs.pop();
          int coolDown = stoi(lineInputs.front());
          lineInputs.empty();
          Volunteer *MyCollectorVolunteer = new CollectorVolunteer(volunteerCounter, name, coolDown);
          volunteers.emplace_back(MyCollectorVolunteer);
          volunteerCounter++;
        }
        else if (type == "limited_collector")
        {
          lineInputs.pop();
          int coolDown = stoi(lineInputs.front());
          lineInputs.pop();
          int MaxDistance = stoi(lineInputs.front());
          lineInputs.empty();
          Volunteer *limited_collector = new LimitedCollectorVolunteer(volunteerCounter, name, coolDown, MaxDistance);
          volunteers.emplace_back(limited_collector);
          volunteerCounter++;
        }
        else if (type == "driver")
        {
          lineInputs.pop();
          int MaxDistance = stoi(lineInputs.front());
          lineInputs.pop();
          int distance_per_step = stoi(lineInputs.front());
          lineInputs.pop();
          Volunteer *Mydriver = new DriverVolunteer(volunteerCounter, name, MaxDistance, distance_per_step);
          volunteers.emplace_back(Mydriver);
          volunteerCounter++;
        }
        else if (type == "limited_driver")
        {
          lineInputs.pop();
          int MaxDistance = stoi(lineInputs.front());
          lineInputs.pop();
          int distance_per_step = stoi(lineInputs.front());
          lineInputs.pop();
          int maxOrders = stoi(lineInputs.front());
          lineInputs.pop();
          Volunteer *MyLimtedDriver = new LimitedDriverVolunteer(volunteerCounter, name, MaxDistance, distance_per_step, maxOrders);
          volunteers.emplace_back(MyLimtedDriver);
          volunteerCounter++;
        }
      }
      while (!lineInputs.empty())
      {
        lineInputs.pop();
      }
    }
  }
  inFile.close();
}
void WareHouse::Delete()
{
  for (BaseAction* action : actionsLog)
    {
      delete action;
      action = nullptr;
    }
    actionsLog.clear();
    // delete old volunteers
    for (Volunteer* volunteer : volunteers)
    {
      delete volunteer;
    }
    volunteers.clear();
    // delete old pendingOrders
    for ( Order* order :pendingOrders)
    {
      delete order;
    }
    pendingOrders.clear();
    // delete old inProcessOrders
    for ( Order* order :inProcessOrders)
    {
      delete order;
    }
    inProcessOrders.clear();
    // delete old completedOrders
    for (Order* order :completedOrders)
    {
      delete order;
    }
    completedOrders.clear();
    // delete old customers
    for (Customer* customer:customers)
    {
      delete customer;
    }
    customers.clear();
  
}
void WareHouse::start()
{
  std::cout << "Warehouse is open!" << std::endl;
  std::queue<std::string> *input = new std::queue<std::string>();
  std::string line;
  this->isOpen = true;
  while (isOpen)
  {
    getline(std::cin, line);
    splitString(line, *input, ' ');
    //cout << input->front()<<endl;
    if (input->front() == "step")
    {
      input->pop();
      int NumOfSteps = std::stoi(input->front());
      SimulationStep_Action(NumOfSteps);
    }
    else if (input->front() == "order")
    {
      input->pop();
      int CustomerId = std::stoi(input->front());
      AddOrder_Action(CustomerId);
    }
    else if (input->front() == "customer")
    {
      input->pop();
      string CustomrerName = input->front();
      input->pop();
      string CustomrerType = input->front();
      input->pop();
      int CustomerDistance = std::stoi(input->front());
      input->pop();
      int MaxOrder = std::stoi(input->front());
      AddCustomer_Action(CustomrerName, CustomrerType, CustomerDistance, MaxOrder);
      customerCounter ++ ;
    }
    else if (input->front() == "orderStatus")
    {
      input->pop();
      int OrderID = std::stoi(input->front());
      PrintOrderStatus_Action(OrderID);
    }
    else if (input->front() == "customerStatus")
    {
      input->pop();
      int CoustomerID = std::stoi(input->front());
      PrintCostomerStatus_Action(CoustomerID);
    }
    else if (input->front() == "volunteerStatus")
    {
      input->pop();
      int volunteerID = std::stoi(input->front());

      PrintVolunteerStatus_Action(volunteerID);
    }
    else if (input->front() == "log")
    {
      log_Action();
    }
    else if (input->front() == "close")
    {
      close_Action();
    }
    else if (input->front() == "backup")
    {
      
      backupWareHouse_Action();
    }
    else if (input->front() == "restore")
    {
      cout << "aloi"<< endl;
      restoreWareHouse_Action();
    }
    while (!input->empty())
    {
      input->pop();
    }
  }

  delete (input);
}
void WareHouse ::addOrder(Order *order)
{
  if (order->getStringstatus() == "PENDING")
  {
    
    pendingOrders.push_back(order);
  }
  else if (order->getStringstatus() == "COLLECTING")
  {
    inProcessOrders.push_back(order);
  }
  else if (order->getStringstatus() == "DELIVERING")
  {
    inProcessOrders.push_back(order);
  }
  else if (order->getStringstatus() == "COMPLETED")
  {
    completedOrders.push_back(order);
  } else {
    cout<< "Cannot place this order " << endl;
  }
  
}
void WareHouse ::addAction(BaseAction *action)
{
  actionsLog.push_back(action);
}
// WareHouse getters Funsctions!
const std::vector<BaseAction *> &WareHouse::getActions() const
{
  return actionsLog;
}
Customer &WareHouse::getCustomer(int customerId) const
{
  for (const auto &customer : customers)
  {
    if (customer->getId() == customerId)
    {
      return *customer;
    }
  }
  // If the customer with the specified ID is not found, i throwed an exception,
  throw std::out_of_range("Customer not found with ID: " + std::to_string(customerId));
}
Volunteer &WareHouse::getVolunteer(int volunteerId) const
{
  for (const auto &volunteer : volunteers)
  {
    if (volunteer->getId() == volunteerId)
    {
      return *volunteer;
    }
  }

  // If the volunteer with the specified ID is not found, you might throw an exception,
  // return a default volunteer, or handle it based on your specific requirements.
  // For now, let's throw an exception:
  throw std::out_of_range("Volunteer not found with ID: " + std::to_string(volunteerId));
}
Order &WareHouse::getOrder(int orderId) const
{
  Order* tmp = nullptr;
  
  for (Order* order : pendingOrders)
  {
    if (order->getId() == orderId)
    {
      tmp =order;
    }
  }

  for (Order* order : inProcessOrders)
  {
    if (order->getId() == orderId)
    {
      tmp = order;
    }
  }

  for (Order* order : completedOrders)
  {
    if (order->getId() == orderId)
    {
      tmp =order;
    }
  }
return *tmp;
  // If the order with the specified ID is not found, you might throw an exception,
  // return a default order, or handle it based on your specific requirements.
  // For now, let's throw an exception:
}
//---------------------------------------
// My Functions implementations
int WareHouse::getOrdersCounter() const
{
  return ordersCounter;
}
int WareHouse::getCustomerCounter() const
{
  return customerCounter;
}
int WareHouse::getVolunteerCounter() const
{
  return volunteerCounter;
}
void WareHouse::setOrdersCounter(int i)
{
  ordersCounter = i;
}
void WareHouse::setCustomerCounter(int i)
{
  customerCounter = i;
}
void WareHouse::setVolunteerCounter(int i)
{
  volunteerCounter = i;
}
void WareHouse::splitString(std::string s, std::queue<std::string> &queue, char c)
{
  std::string temp;
  for (char i : s)
  {
    if (i == c)
    {
      queue.push(temp);
      temp = "";
    }
    else if (i == '#')
    {
      break;
    }
    else
    {
      temp.push_back(i);
    }
  }
  queue.push(temp);
}
bool WareHouse ::customerExists(int customerId) const
{
  for (unsigned int i = 0; i < customers.size(); i++)
  {
    if (customers.at(i)->getId() == customerId)
    {
      return true;
    }
  }
  return false;
}
bool WareHouse ::volunteerExists(int volunteerId) const
{
  for (unsigned int i = 0; i < volunteers.size(); i++)
  {
    if (volunteers.at(i)->getId() == volunteerId)
    {
      return true;
    }
  }
  return false;
}
bool WareHouse ::orderExists(int orderId) const
{
  for (unsigned int i = 0; i < pendingOrders.size(); i++)
  {
    if (pendingOrders.at(i)->getId() == orderId)
    {
      return true;
    }
  }
  for (unsigned int i = 0; i < inProcessOrders.size(); i++)
  {
    if (inProcessOrders.at(i)->getId() == orderId)
    {
      return true;
    }
  }
  for (unsigned int i = 0; i < completedOrders.size(); i++)
  {
    if (completedOrders.at(i)->getId() == orderId)
    {
      return true;
    }
  }

  return false;
}
//--------------------------------------
void WareHouse ::close()
{
  isOpen = false;
}
void WareHouse ::open()
{
  isOpen = true;
}
//--------------------------------
// rule of 5
// Destructor
WareHouse ::~WareHouse()
{
  for (size_t i = 0; i < actionsLog.size(); i++)
  {
    delete actionsLog.at(i);
  }
  actionsLog.clear();
  for (unsigned int i = 0; i < volunteers.size(); i++)
  {
    delete volunteers.at(i);
  }
  volunteers.clear();
  for (unsigned int i = 0; i < pendingOrders.size(); i++)
  {
    delete pendingOrders.at(i);
  }
  pendingOrders.clear();
  for (unsigned int i = 0; i < inProcessOrders.size(); i++)
  {
    delete inProcessOrders.at(i);
  }
  inProcessOrders.clear();
  for (unsigned int i = 0; i < completedOrders.size(); i++)
  {
    delete completedOrders.at(i);
  }
  completedOrders.clear();
  for (unsigned int i = 0; i < customers.size(); i++)
  {
    delete customers.at(i);
  }
  customers.clear();
}
// copy constructor
WareHouse::WareHouse(const WareHouse &other) : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), ordersCounter(other.ordersCounter), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter)
{
  this->isOpen = other.isOpen;
  this->customerCounter = other.customerCounter;
  this->volunteerCounter = other.volunteerCounter;
  for (BaseAction* action : other.actionsLog)
  {
    // BaseAction *action = new BaseAction(actionsLog[i]);
    actionsLog.push_back(action->clone());
  }

  for (Order* o: other.inProcessOrders)
  {
    // Order *processedOrder = new Order(inProcessOrders[i]);
    inProcessOrders.push_back(o->clone());
  }
  for (Order* o: other.completedOrders)
  {
    // Order *isCompleted = new Order();
    completedOrders.push_back(o->clone());
  }

  for (Volunteer* v:other.volunteers)
  {
    // Volunteer *Voln = new Volunteer();
    volunteers.push_back(v->clone());
  }
  for (Order* o: other.pendingOrders)
  {
    // Order *ispending = new Order();
    pendingOrders.push_back(o->clone());
  }
  for (Customer* c: other.customers)
  {
    // Customer *cust = new Customer *(customers[i]);
    customers.push_back(c->clone());
  }
}
WareHouse &WareHouse::operator=(const WareHouse &other)
{
  cout<<"lll";
  if (&other != this)
  {
    isOpen = other.isOpen;
    customerCounter = other.customerCounter;
    volunteerCounter = other.volunteerCounter;
    // delete old actionlogs
    Delete();
    
    for (BaseAction* action : other.actionsLog)
    {
      actionsLog.push_back(action->clone());
    }
    // delete old volunteers

    for (Volunteer* volunteer : other.volunteers)
    {
      volunteers.push_back(volunteer->clone());
    }
    // delete old pendingOrders
    
    for (Order* o : other.pendingOrders)
    {
      Order* order = new Order(*o);
      pendingOrders.push_back(order);
    }
    // delete old inProcessOrders
    
    for (Order* o :other.inProcessOrders)
    {
        Order* order = new Order(*o);
        pendingOrders.push_back(order);
    }
    // delete old completedOrders
    
    for (Order* o :other.completedOrders)
    {
        Order* order = new Order(*o);
        pendingOrders.push_back(order);
    }
    // delete old customers
    
    for (Customer* customer:customers)
    {
      customers.push_back(customer);
    }
  }
  return *this;
}
// move assignment operator
WareHouse &WareHouse::operator=(WareHouse &&other)
{
  if (this != &other)
  {
    // this->clear();  do the implementation
    this->customerCounter = other.customerCounter;
    this->volunteerCounter = other.volunteerCounter;

    for (unsigned int i = 0; i < other.customers.size(); i++)
    {
      this->customers.push_back(other.customers.at(i));
      other.customers[i] = nullptr;
    }
    for (unsigned int i = 0; i < other.completedOrders.size(); i++)
    {
      this->completedOrders.push_back(other.completedOrders.at(i));
      other.completedOrders[i] = nullptr;
    }
    for (unsigned int i = 0; i < other.inProcessOrders.size(); i++)
    {
      this->inProcessOrders.push_back(other.inProcessOrders.at(i));
      other.inProcessOrders[i] = nullptr;
    }
    for (unsigned int i = 0; i < other.pendingOrders.size(); i++)
    {
      this->pendingOrders.push_back(other.pendingOrders.at(i));
      other.pendingOrders[i] = nullptr;
    }
    for (unsigned int i = 0; i < other.volunteers.size(); i++)
    {
      this->volunteers.push_back(other.volunteers.at(i));
      other.volunteers[i] = nullptr;
    }
    for (unsigned int i = 0; i < other.actionsLog.size(); i++)
    {
      this->actionsLog.push_back(other.actionsLog.at(i));
      other.actionsLog[i] = nullptr;
    }
    other.actionsLog.clear();
    other.volunteers.clear();
    other.pendingOrders.clear();
    other.inProcessOrders.clear();
    other.completedOrders.clear();
    other.customers.clear();
    other.isOpen = false;
  }
  return *this;
}
// move construtor
WareHouse::WareHouse(WareHouse &&other) : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), ordersCounter(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter)
{
  for (unsigned int i = 0; i < other.customers.size(); i++)
  {
    this->customers.push_back(other.customers.at(i));
    other.customers[i] = nullptr;
  }
  for (unsigned int i = 0; i < other.completedOrders.size(); i++)
  {
    this->completedOrders.push_back(other.completedOrders.at(i));
    other.completedOrders[i] = nullptr;
  }
  for (unsigned int i = 0; i < other.inProcessOrders.size(); i++)
  {
    this->inProcessOrders.push_back(other.inProcessOrders.at(i));
    other.inProcessOrders[i] = nullptr;
  }
  for (unsigned int i = 0; i < other.pendingOrders.size(); i++)
  {
    this->pendingOrders.push_back(other.pendingOrders.at(i));
    other.pendingOrders[i] = nullptr;
  }
  for (unsigned int i = 0; i < other.volunteers.size(); i++)
  {
    this->volunteers.push_back(other.volunteers.at(i));
    other.volunteers[i] = nullptr;
  }
  for (unsigned int i = 0; i < other.actionsLog.size(); i++)
  {
    this->actionsLog.push_back(other.actionsLog.at(i));
    other.actionsLog[i] = nullptr;
  }
  other.actionsLog.clear();
  other.volunteers.clear();
  other.pendingOrders.clear();
  other.inProcessOrders.clear();
  other.completedOrders.clear();
  other.customers.clear();
  other.isOpen = false;
}
//-----------------------------------
vector<Customer *>& WareHouse::getCustomers()
{
  return customers;
}
vector<Order *> &WareHouse ::getcompletedOrders()
{
  return completedOrders;
}
vector<Order *> &WareHouse ::getinProcessOrders()
{
  return inProcessOrders;
}
vector<Order *>& WareHouse ::getpendingOrders()
{
  return pendingOrders;
}
vector<Volunteer *>& WareHouse::getvolunteers()
{
  return volunteers;
}
// Action preformer(calling the functions in the Action cpp file!)
void WareHouse::SimulationStep_Action(int NumOfSteps)
{
  SimulateStep *action = new SimulateStep(NumOfSteps);
  action->act(*this);
  actionsLog.push_back(action);
}
void WareHouse::AddOrder_Action(int CustomerId)
{
  AddOrder *action = new AddOrder(CustomerId);
  action->act(*this);
  actionsLog.push_back(action);
}
void WareHouse::AddCustomer_Action(const string &name, const string &StringCoustomerType, int CustomrtDistance, int Maxorder)
{
  AddCustomer *action = new AddCustomer(name, StringCoustomerType, CustomrtDistance, Maxorder);
  action->act(*this);
  actionsLog.push_back(action);
  
}
void WareHouse::PrintOrderStatus_Action(int OrderID)
{
  PrintOrderStatus *action = new PrintOrderStatus(OrderID);
  action->act(*this);
  actionsLog.push_back(action);
}
void WareHouse::PrintCostomerStatus_Action(int CoustomerID)
{
  PrintCustomerStatus *action = new PrintCustomerStatus(CoustomerID);
  action->act(*this);
  actionsLog.push_back(action);
}
void WareHouse::PrintVolunteerStatus_Action(int volunteerID)
{
  PrintVolunteerStatus *action = new PrintVolunteerStatus(volunteerID);
  action->act(*this);
  actionsLog.push_back(action);
}
void WareHouse::log_Action()
{
  PrintActionsLog *action = new PrintActionsLog();
  action->act(*this);

}
void WareHouse::close_Action()
{
  Close *action = new Close();
  action->act(*this);
  actionsLog.push_back(action);
}
void WareHouse::backupWareHouse_Action()
{
  BackupWareHouse* action = new BackupWareHouse();
  action->act(*this);
  actionsLog.push_back(action);
}
void WareHouse::restoreWareHouse_Action()
{
  RestoreWareHouse* action = new RestoreWareHouse();
  action->act(*this);
  actionsLog.push_back(action);
}

void WareHouse::addCustomer(Customer* customer){
  customers.push_back(customer);
}
bool WareHouse:: ShouldBeInProcess(Order& order)
{
  for(Order* o : inProcessOrders)
  {
    if(o == &order)
    {
      return true;
    }
  }
  return false;
}