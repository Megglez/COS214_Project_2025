/**
 * @file Browse.h
 * @brief Browse action state for customers in the nursery
 *
 * This file defines the Browse class which represents a customer browsing
 * plants in the nursery. It uses Qt's timer system for asynchronous browsing
 * behavior and implements the State pattern.
 */

#ifndef BROWSE_H
#define BROWSE_H
#include "Action.h"
#include "../Greenhouse/Plant.h"
#include <vector>
#include <QObject>
#include <QTimer>
#include <QDebug>
class Customer;
class InfoDesk;

/**
 * @class Browse
 * @brief Represents a customer browsing plants in the nursery
 *
 * The Browse class is a concrete state in the State pattern, representing
 * a customer looking at plants. It uses Qt's QTimer for simulating asynchronous
 * browsing time before transitioning to the next state (Enquire, Purchasing, or leaving).
 *
 * Features:
 * - Asynchronous browsing with timer-based state transitions
 * - Manages list of plants customer is interested in
 * - Supports random decoration selection for plants
 * - Transitions to Enquire, Purchasing, or Browse states
 *
 * @note Inherits from QObject for Qt signal/slot mechanism
 */
class Browse : public QObject, public Action
{
    Q_OBJECT
private:
    /**
     * @brief List of plants the customer is considering
     */
    std::vector<Plant *> plantsToBuy;

    /**
     * @brief Quantities for each plant in plantsToBuy
     */
    std::vector<int> quantities;

    /**
     * @brief Timer for asynchronous browsing duration
     */
    QTimer *browseTimer;

    /**
     * @brief Pointer to the customer performing this action
     */
    Customer *currentCustomer;

    /**
     * @brief Flag indicating if this Browse owns the plant pointers (cloned)
     */
    bool ownsPlants;

public:
    /**
     * @brief Construct a Browse action for a single plant
     * @param plant Pointer to the plant to browse
     * @param quantity Desired quantity of the plant
     * @param ownsPlantPointers If true, Browse will delete plants in destructor
     *
     * Creates a browsing action with one plant and initializes the timer.
     */
    Browse(Plant *plant, int quantity, bool ownsPlantPointers = true) : Action("Browsing"), QObject(nullptr), currentCustomer(nullptr), ownsPlants(ownsPlantPointers)
    {
        if (plant)
        {
            plantsToBuy.push_back(plant);
            quantities.push_back(quantity);
        }
        browseTimer = new QTimer(this);
        connect(browseTimer, &QTimer::timeout, this, &Browse::onBrowseTimeout);
    }

    /**
     * @brief Construct a Browse action for multiple plants
     * @param plants Vector of plant pointers to browse
     * @param quants Vector of desired quantities for each plant
     * @param ownsPlantPointers If true, Browse will delete plants in destructor
     *
     * Creates a browsing action with multiple plants and initializes the timer.
     * The plants vector can contain decorated plants (GiftWrap, Pot, etc.).
     */
    Browse(std::vector<Plant *> plants, std::vector<int> quants, bool ownsPlantPointers = true) : Action("Browsing"), QObject(nullptr), plantsToBuy(plants), quantities(quants), currentCustomer(nullptr), ownsPlants(ownsPlantPointers)
    {
        browseTimer = new QTimer(this);
        connect(browseTimer, &QTimer::timeout, this, &Browse::onBrowseTimeout);
    }

    /**
     * @brief Virtual destructor - cleans up owned plant pointers
     */
    virtual ~Browse()
    {
        if (ownsPlants)
        {
            for (Plant *plant : plantsToBuy)
            {
                delete plant;
            }
        }
    }

    /**
     * @brief Handle the browsing action
     *
     * Displays the plants the customer is considering buying.
     * Implements the Action interface.
     */
    void handle() override;

    /**
     * @brief Handle browsing with customer context
     * @param customer The customer performing the browse action
     *
     * Starts the asynchronous browsing process with timer.
     */
    void handle(Customer *customer) override;

    /**
     * @brief Start asynchronous browsing with timer
     * @param customer The customer who is browsing
     *
     * Initiates a random duration browse period (20-40 seconds) after which
     * the customer will transition to the next action.
     */
    void startBrowsing(Customer *customer);

    /**
     * @brief Get the next action for this customer
     * @return Action* Pointer to next action (Browse, Enquire, or Purchasing)
     *
     * Randomly selects the next state from three possibilities:
     * - Continue browsing (new Browse)
     * - Ask questions (Enquire)
     * - Proceed to purchase (Purchasing)
     */
    Action *getNextAction() override;

    /**
     * @brief Request staff assistance while browsing
     * @param customer The customer requesting assistance
     * @param desk Reference to the InfoDesk for staff coordination
     *
     * Transitions the customer from Browse to Enquire state.
     */
    void requestStaffAssistance(Customer *customer, InfoDesk &desk) override;

    /**
     * @brief Get the list of plants to buy
     * @return std::vector<Plant*> Vector of plant pointers
     */
    std::vector<Plant *> getPlantsToBuy() const { return plantsToBuy; }

    /**
     * @brief Get the quantities for each plant
     * @return std::vector<int> Vector of quantities
     */
    std::vector<int> getQuantities() const { return quantities; }

private slots:
    /**
     * @brief Slot called when browse timer expires
     *
     * Determines the next action (Enquire or Purchasing) and transitions
     * the customer to that state. May also add plants to basket if purchasing.
     */
    void onBrowseTimeout();
};

#endif // BROWSE_H