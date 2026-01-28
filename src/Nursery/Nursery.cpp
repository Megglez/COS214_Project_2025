/**
 * @file Nursery.cpp
 * @brief Implementation of the Nursery facade class
 * @author Chaos_Compilers
 * @date 2025
 */

#include "Nursery.h"
#include "../Staff/Cashiers.h"
#include "../Staff/SalesStaff.h"
#include "../Staff/Gardener.h"
#include "../Staff/Manager.h"
#include "../Greenhouse/AddStock.h"
#include "../Greenhouse/Autumn.h"
#include "../Greenhouse/Winter.h"
#include "../Greenhouse/Summer.h"
#include <QDebug>
#include <algorithm>
#include <memory>
#include "../Greenhouse/Spring.h"

Nursery::Nursery(QObject *parent) : QObject(parent)
{
    qDebug() << "Nursery simulation core initialized.";

    // Initialize Customer Management
    customerFactory = new CustomerCreator();
    customerCount = 0;
    customerLimit = 20; // Can be adjusted as needed

    // Initialize Plant Management
    inventory = new Inventory();
    stock = new Stock(inventory);
    flowerFactory = new FlowerPlanter();
    herbFactory = new HerbPlanter();
    treeFactory = new TreePlanter();
    succulentFactory = new SucculentPlanter();
    currentSeason = new Spring(inventory);

    // Add diverse initial plants to stock for customers to browse
    // Flowers
    stock->Add(std::unique_ptr<Plant>(flowerFactory->planterMethod("Rose")), 20);
    stock->Add(std::unique_ptr<Plant>(flowerFactory->planterMethod("Tulip")), 15);
    stock->Add(std::unique_ptr<Plant>(flowerFactory->planterMethod("Lily")), 10);
    stock->Add(std::unique_ptr<Plant>(flowerFactory->planterMethod("Sunflower")), 12);
    stock->Add(std::unique_ptr<Plant>(flowerFactory->planterMethod("Daisy")), 18);

    // Herbs
    stock->Add(std::unique_ptr<Plant>(herbFactory->planterMethod("Basil")), 25);
    stock->Add(std::unique_ptr<Plant>(herbFactory->planterMethod("Mint")), 30);
    stock->Add(std::unique_ptr<Plant>(herbFactory->planterMethod("Rosemary")), 16);
    stock->Add(std::unique_ptr<Plant>(herbFactory->planterMethod("Thyme")), 14);
    stock->Add(std::unique_ptr<Plant>(herbFactory->planterMethod("Oregano")), 22);

    // Trees
    stock->Add(std::unique_ptr<Plant>(treeFactory->planterMethod("Oak")), 8);
    stock->Add(std::unique_ptr<Plant>(treeFactory->planterMethod("Maple")), 6);
    stock->Add(std::unique_ptr<Plant>(treeFactory->planterMethod("Pine")), 10);
    stock->Add(std::unique_ptr<Plant>(treeFactory->planterMethod("Apple")), 7);

    // Succulents
    stock->Add(std::unique_ptr<Plant>(succulentFactory->planterMethod("Aloe")), 24);
    stock->Add(std::unique_ptr<Plant>(succulentFactory->planterMethod("Cactus")), 28);
    stock->Add(std::unique_ptr<Plant>(succulentFactory->planterMethod("Jade")), 19);
    stock->Add(std::unique_ptr<Plant>(succulentFactory->planterMethod("Echeveria")), 13);

    qDebug() << "Initial stock added:" << stock->getStockListSize() << "plant types";

    // Initialize Staff Management
    infoDesk = new InfoDesk();

    // Create staff members
    std::string cashierName = "Alice";
    std::string cashierId = "C001";
    cashier = new Cashiers(cashierName, cashierId, infoDesk);
    infoDesk->addStaff(cashier);
    staff.push_back(cashier);

    std::string salesName1 = "Bob";
    std::string salesId1 = "S001";
    Staff *salesStaff1 = new SalesStaff(salesName1, salesId1, infoDesk, inventory);
    infoDesk->addStaff(salesStaff1);
    staff.push_back(salesStaff1);

    std::string salesName2 = "Carol";
    std::string salesId2 = "S002";
    Staff *salesStaff2 = new SalesStaff(salesName2, salesId2, infoDesk, inventory);
    infoDesk->addStaff(salesStaff2);
    staff.push_back(salesStaff2);

    std::string gardenerName = "Dave";
    std::string gardenerId = "G001";
    Staff *gardenerStaff = new Gardener(gardenerName, gardenerId, infoDesk, inventory);
    infoDesk->addStaff(gardenerStaff);
    staff.push_back(gardenerStaff);

    std::string managerName = "Eve";
    std::string managerId = "M001";
    qDebug() << "About to create Manager...";
    Staff *managerStaff = new Manager(managerName, managerId, infoDesk, inventory);
    qDebug() << "Manager created, adding to InfoDesk...";
    infoDesk->addStaff(managerStaff);
    qDebug() << "Manager added to InfoDesk, adding to staff vector...";
    staff.push_back(managerStaff);

    qDebug() << "Staff members created:" << staff.size();
    qDebug() << "Creating AddStock command...";

    startPlants = new AddStock(inventory);

    qDebug() << "Nursery constructor complete!";
}

void Nursery::setStock(unique_ptr<Plant> plant, int amount)
{
    startPlants->execute(move(plant), amount);
}

void Nursery::setSeason(Seasons *newSeason)
{
    if (currentSeason)
    {
        delete currentSeason;
    }
    currentSeason = newSeason;
    qDebug() << "Season changed to:" << currentSeason->getSeason().c_str();
}

Nursery::~Nursery()
{
    // Clean up customers
    for (Customer *customer : activeCustomers)
    {
        delete customer;
    }
    activeCustomers.clear();
    delete customerFactory;

    // Clean up staff (delete staff first, then InfoDesk)
    // Note: cashier is already in the staff vector, so don't delete separately
    for (Staff *s : staff)
    {
        delete s;
    }
    staff.clear();
    cashier = nullptr; // Already deleted as part of staff vector
    delete infoDesk;

    // Clean up plant management
    delete stock;
    delete inventory;
    delete flowerFactory;
    delete herbFactory;
    delete treeFactory;
    delete succulentFactory;
    delete currentSeason;
}
void Nursery::handleCustomerArrivalSignal()
{
    if (customerCount >= customerLimit)
    {
        qDebug() << "Nursery is at capacity, cannot accept more customers.";
        return;
    }

    Customer *newCustomer = customerFactory->createNewCustomer(this, stock);
    if (newCustomer)
    {
        activeCustomers.push_back(newCustomer);
        customerCount++;
        qDebug() << "Nursery: Added new customer. Total active customers:" << activeCustomers.size();

        // Assign customer to InfoDesk for staff allocation
        infoDesk->handleCustomer(newCustomer);
    }
}

void Nursery::addCustomer(Customer *customer)
{
    if (customer && customerCount < customerLimit)
    {
        activeCustomers.push_back(customer);
        customerCount++;
        qDebug() << "Nursery::addCustomer - Customer" << customer->getId() << "added. Total:" << customerCount << "/" << customerLimit;
    }
    else if (customer)
    {
        qDebug() << "Nursery::addCustomer - Cannot add customer. Count:" << customerCount << "Limit:" << customerLimit;
    }
}

void Nursery::handleChange()
{
    currentSeason->handleChange(this);
}

void Nursery::removeCustomer(Customer *customer)
{
    auto it = std::find(activeCustomers.begin(), activeCustomers.end(), customer);
    if (it != activeCustomers.end())
    {
        activeCustomers.erase(it);
        customerCount--;
    }
}

void Nursery::handleCustomerDeparture(Customer *customer)
{
    if (!customer)
    {
        qDebug() << "Warning: Attempted to remove null customer.";
        return;
    }

    qDebug() << "Customer" << customer->getId() << "is leaving the nursery.";

    // Remove from active customers list and update count
    auto it = std::find(activeCustomers.begin(), activeCustomers.end(), customer);
    if (it != activeCustomers.end())
    {
        activeCustomers.erase(it);
        customerCount--;
        qDebug() << "Customer" << customer->getId() << "removed. Total active customers:" << activeCustomers.size();

        // Free any assigned staff
        if (customer->getAssignedStaff())
        {
            Staff *staff = customer->getAssignedStaff();
            qDebug() << "Freeing staff" << staff->getName().c_str() << "assigned to departing customer.";
            staff->completeTask(); // Mark staff as available again
            customer->setAssignedStaff(nullptr);
        }

        // Remove customer from any waiting queues in InfoDesk
        if (infoDesk)
        {
            infoDesk->removeCustomerFromQueue(customer);
        }

        // NOTE: Do NOT delete customer here! The customer object is still executing code
        // (called from within Customer::processNextAction()). Deleting it here causes
        // a segfault. The GUI will handle the deletion after the customer is marked
        // for removal and visually moved to the exit.

        qDebug() << "Customer" << customer->getId() << "marked for departure. Current customer count:" << customerCount;
    }
    else
    {
        qDebug() << "Warning: Customer" << customer->getId() << "not found in active customers list.";
    }
}

// Note: If seasonal update functionality is needed, it should be declared in the header first
// The setState method in the header can be used for setting seasons

Cashiers *Nursery::getCashier() const
{
    if (infoDesk)
    {
        std::vector<Staff *> cashiers = infoDesk->getStaffByType("Cashier");
        if (!cashiers.empty())
        {
            return dynamic_cast<Cashiers *>(cashiers[0]);
        }
    }
    return nullptr;
}