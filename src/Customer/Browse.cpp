/**
 * @file Browse.cpp
 * @brief Implementation of the Browse action class
 *
 * Implements browsing behavior with asynchronous timer-based state transitions,
 * plant selection, and inventory management integration.
 */

#include "Browse.h"
#include "Purchasing.h" // Needed for getNextAction()
#include "Customer.h"   // Need full Customer definition
#include "Enquire.h"    // Need Enquire definition for new Enquire()
#include "../Staff/InfoDesk.h"

#include <QRandomGenerator>
#include "../Nursery/Nursery.h"

void Browse::handle()
{
    std::cout << "Customer is browsing. They are considering buying:" << std::endl;
    for (size_t i = 0; i < plantsToBuy.size(); i++)
    {
        std::cout << "  - " << quantities[i] << " of "
                  << (plantsToBuy[i] ? plantsToBuy[i]->getName() : "a generic plant") << std::endl;
    }
}

void Browse::handle(Customer *customer)
{
    // Use async version
    startBrowsing(customer);
}

void Browse::startBrowsing(Customer *customer)
{
    if (!customer)
    {
        std::cerr << "Error: Browse::startBrowsing() received null customer pointer." << std::endl;
        return;
    }

    currentCustomer = customer;

    std::cout << "Customer is browsing. They are considering buying:" << std::endl;
    for (size_t i = 0; i < plantsToBuy.size(); i++)
    {
        std::cout << "  - " << quantities[i] << " of "
                  << (plantsToBuy[i] ? plantsToBuy[i]->getName() : "a generic plant") << std::endl;
    }

    // Generate random time between 20 and 40 seconds (in milliseconds)
    int sleepTime = QRandomGenerator::global()->bounded(21) + 20; // 20 to 40 seconds
    std::cout << "Customer will browse for " << sleepTime << " seconds..." << std::endl;

    // Start timer (convert seconds to milliseconds)
    browseTimer->start(sleepTime * 1000);
}

void Browse::onBrowseTimeout()
{
    std::cout << "Browse time completed. Customer deciding next action..." << std::endl;

    // Decide next state: 0 = Enquire, 1 = Purchasing
    int nextState = QRandomGenerator::global()->bounded(2);

    if (!currentCustomer)
    {
        std::cerr << "Error: Browse timeout but no customer reference stored." << std::endl;
        return;
    }

    if (nextState == 0)
    {
        // Go to infodesk (enquire)
        std::cout << "Customer decided to enquire. Going to infodesk." << std::endl;

        // Clone plants for Enquire since Browse will delete its own plants
        std::vector<Plant *> enquirePlants;
        for (Plant *plant : plantsToBuy)
        {
            if (plant)
            {
                enquirePlants.push_back(plant->clone());
            }
        }

        // setAction will delete this Browse object, so don't access members after this
        if (currentCustomer)
        {
            // Pass ownsPlants=true since we cloned them for Enquire
            currentCustomer->setAction(new Enquire(enquirePlants, "", 0, true));
        }
        // You may want to call requestStaffAssistance here if InfoDesk is available
    }
    else
    {
        // Try to purchase all plants in the list
        std::cout << "Customer decided to purchase." << std::endl;
        Nursery *nursery = currentCustomer->getNursery();
        if (!nursery)
        {
            std::cerr << "Error: No nursery assigned to customer." << std::endl;
            return;
        }
        Inventory *inventory = nursery->getInventory();
        if (!inventory)
        {
            std::cerr << "Error: No inventory available." << std::endl;
            return;
        }
        auto &invMap = inventory->getInventory();

        // Process each plant type the customer wants
        std::vector<Plant *> plantsToPurchase;
        std::vector<int> quantitiesToPurchase;

        for (size_t i = 0; i < plantsToBuy.size(); i++)
        {
            Plant *plant = plantsToBuy[i];
            int quantity = quantities[i];

            if (!plant)
                continue;

            std::string plantName = plant->getName();
            auto it = invMap.find(plantName);
            int available = 0;
            if (it != invMap.end())
            {
                available = it->second.second;
            }
            int toTake = std::min(quantity, available);

            if (toTake > 0)
            {
                // Add to basket
                for (int j = 0; j < toTake; ++j)
                {
                    currentCustomer->addToBasket(plant, 1);
                }
                // Update inventory
                it->second.second -= toTake;
                std::cout << "Customer added " << toTake << " of " << plantName
                          << " to basket. Inventory now: " << it->second.second << std::endl;

                plantsToPurchase.push_back(plant);
                quantitiesToPurchase.push_back(toTake);
            }
            else
            {
                std::cout << "No stock available for " << plantName << ". Customer cannot add to basket." << std::endl;
            }
            // If customer wanted more than available, take max and set inventory to zero
            if (quantity > available && it != invMap.end())
            {
                it->second.second = 0;
                std::cout << "Customer wanted more " << plantName << " than available. Inventory set to zero." << std::endl;
            }
        }

        // Transfer plant ownership to basket - Browse should not delete them
        // Set ownsPlants to false before transitioning
        ownsPlants = false;

        // Change state to purchasing
        // NOTE: setAction will delete this Browse object, so don't access any members after this!
        if (currentCustomer)
        {
            currentCustomer->setAction(new Purchasing(std::vector<Plant *>(), std::vector<int>(), false));
        }
    }
}

Action *Browse::getNextAction()
{
    // Randomly return Browse, Enquire, or Purchasing
    int nextState = QRandomGenerator::global()->bounded(3); // 0, 1, or 2

    if (nextState == 0)
    {
        return new Browse(plantsToBuy, quantities);
    }
    else if (nextState == 1)
    {
        return new Enquire(plantsToBuy);
    }
    else
    {
        return new Purchasing(plantsToBuy, quantities);
    }
}

void Browse::requestStaffAssistance(Customer *customer, InfoDesk &desk)
{
    // Browsing customer decides to Enquire. Transition to the Enquire state.
    desk.getInfodesk();
    std::cout << "Customer wants to Enquire while browsing. Transitioning to Enquire state." << std::endl;
    customer->setAction(new Enquire(plantsToBuy));
}