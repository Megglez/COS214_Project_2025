/**
 * @file Customer.cpp
 * @brief Implementation of the Customer class
 *
 * Implements customer behavior including state management, basket operations,
 * staff interactions, and state transitions through the State pattern.
 */

#include "Customer.h"
#include <QDebug>
#include "../Staff/Staff.h"
#include "../Nursery/Nursery.h"

Customer::Customer(Action *action, Nursery *nursery, QObject *parent) : QObject(parent)
{
    static int nextId = 1;
    this->id = nextId++;
    this->action = action;
    this->nursery = nursery;
}

Customer::~Customer()
{
    delete action;
    // Clean up basket - these are cloned plants that customer owns
    for (Plant *plant : basket)
    {
        delete plant;
    }
    basket.clear();
}

// request from staff
void Customer::request()
{
    // TODO: Update for
    cout << "Customer " << id << " is requesting assistance for action: " << action->getActionName() << endl;
}

void Customer::setAction(Action *newAction)
{
    qDebug() << "Customer" << id << "setAction called. Old action:" << (this->action ? this->action->getActionName().c_str() : "nullptr")
             << "New action:" << (newAction ? newAction->getActionName().c_str() : "nullptr");

    if (this->action)
    {
        qDebug() << "Deleting old action...";
        delete this->action;
        qDebug() << "Old action deleted.";
    }
    this->action = newAction;
    qDebug() << "Customer" << id << "action set complete.";
}

bool Customer::addToBasket(Plant *plants, int quantity)
{
    // Clone each plant instance for the basket
    // This ensures each basket entry is independent
    for (int i = 0; i < quantity; i++)
    {
        basket.push_back(plants->clone());
    }

    std::cout << "Customer " << id << " added " << quantity << " of "
              << plants->getName() << " to basket." << std::endl;
    return true;
}

void Customer::clearBasket()
{
    std::cout << "Customer " << id << " basket cleared. Had " << basket.size() << " items." << std::endl;
    basket.clear();
}

int Customer::getId() const
{
    return id;
}

Action *Customer::getAction() const
{
    return action;
}

// ... (other methods: addToBasket, removeFromBasket, getId, getAction) ...

void Customer::setAssignedStaff(Staff *staff)
{
    this->assignedStaff = staff;
    if (staff)
    {
        cout << "Customer " << id << " is now being assisted by staff member " << staff->getName() << endl;
    }
    else
    {
        cout << "Customer " << id << " staff assignment cleared (no longer being assisted)" << endl;
    }
}

void Customer::processNextAction()
{
    if (!action)
    {
        return;
    }

    // Get the next action from current state
    Action *nextAction = action->getNextAction();

    if (nextAction == nullptr)
    {
        // Customer is leaving - set action to nullptr to signal leaving
        // but DON'T delete the customer here (avoid use-after-free)
        qDebug() << "Customer" << id << "is leaving the nursery";

        setAction(nullptr); // Clear action to signal leaving state

        // Only notify nursery if one is assigned (not in testing mode)
        if (nursery)
        {
            nursery->handleCustomerDeparture(this);
        }
        else
        {
            qDebug() << "Customer" << id << "departure handled in test mode (no nursery assigned)";
        }
    }
    else
    {
        // Transition to next state
        setAction(nextAction);
        qDebug() << "Customer" << id << "transitioned to" << nextAction->getActionName().c_str();
    }
}