/**
 * @file Enquire.cpp
 * @brief Implementation of the Enquire action class
 *
 * Implements enquiry behavior including question handling, staff assistance
 * requests, and state transitions to Browse or leaving the nursery.
 */

#include "Enquire.h"
#include "Browse.h"
#include "Customer.h"
#include "../Staff/InfoDesk.h"
#include "../Nursery/Nursery.h"
#include <iostream>
#include <random>
using namespace std;

void Enquire::handle()
{
    cout << "Customer is handling the Enquire action" << endl;
    if (!enquiryQuestion.empty())
    {
        cout << "Customer's question: " << enquiryQuestion << endl;
    }

    // Request staff assistance immediately
    handle(nullptr);
}

void Enquire::handle(Customer *customer)
{
    if (!customer)
    {
        return;
    }

    cout << "Customer " << customer->getId() << " is enquiring at info desk." << endl;
    if (!enquiryQuestion.empty())
    {
        cout << "Question: " << enquiryQuestion << endl;
    }

    // Get info desk from nursery and request assistance
    Nursery *nursery = customer->getNursery();
    if (nursery && nursery->getInfoDesk())
    {
        InfoDesk *desk = nursery->getInfoDesk();
        desk->handleCustomer(customer);
    }
    else
    {
        cout << "Warning: No info desk available for customer enquiry." << endl;
    }
}

Action *Enquire::getNextAction()
{
    // Generate random integer between 0 and 1 to determine next action
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> actionDist(0, 1);

    int nextAction = actionDist(gen);

    if (nextAction == 0)
    {
        // Customer leaves the nursery (no next action)
        cout << "Customer finished enquiring and is leaving the nursery." << endl;
        return nullptr;
    }
    else
    {
        // Customer transitions to Browse state
        cout << "Customer finished enquiring and is now browsing for plants." << endl;
        // Create Browse action with the same plants they were asking about
        // Default quantity of 1 for each plant if they decide to browse
        std::vector<int> defaultQuantities(plantsOfInterest.size(), 1);
        return new Browse(plantsOfInterest, defaultQuantities);
    }
}

void Enquire::requestStaffAssistance(Customer *customer, InfoDesk &desk)
{
    cout << "Customer is requesting assistance at infodesk." << endl;
    desk.handleCustomer(customer);
}