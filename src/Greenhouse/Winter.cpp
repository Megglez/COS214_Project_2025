/**
 * @file Winter.cpp
 * @brief Implementation of Winter season behavior
 *
 * Contains the concrete implementation of winter-specific seasonal
 * logic including transitions, command execution, and inventory adjustments.
 * Completes the seasonal cycle by transitioning to spring.
 */

#include "Winter.h"
#include "WinterCommand.h"
#include "Nursery/Nursery.h"

/**
 * @brief Constructs Winter season and executes winter commands
 * @param inv Pointer to Inventory system for seasonal operations
 *
 * Initializes the winter season state and immediately executes
 * winter-specific commands through the WinterCommand class. This
 * ensures that seasonal adjustments are applied when the winter
 * state is entered.
 *
 * @param inv Valid Inventory pointer for seasonal operations
 *
 * @post commands pointer is set to a new WinterCommand instance
 * @post WinterCommand::execute() is called during construction
 *
 * @note Cleans up any existing commands before assigning new one
 * @warning Assumes commands may have been initialized in base class
 *
 * @par Winter Inventory Adjustments:
 * Based on previous inventory logic, winter typically sets:
 * - Succulents: 0 (out of stock - frost kills them)
 * - Herbs: 0 (out of stock)
 * - Flowers: 5 (very low stock)
 * - Trees: 30 (low stock - dormant season)
 */
Winter::Winter(Inventory *inv) : inventory(inv)
{
    if (commands)
    {
        delete commands;
    }
    commands = new WinterCommand(inventory);
    commands->execute();
}

/**
 * @brief Destructor for Winter season
 *
 * Cleans up Winter-specific resources. The inventory pointer
 * ownership is questioned in the implementation comments.
 *
 * @note The commands pointer is managed by the base class or
 *       derived class destruction chain.
 * @warning The commented code suggests uncertainty about inventory
 *          ownership - clarify whether Winter owns the inventory
 */
Winter::~Winter()
{
    // delete inventory? - Ownership question needs resolution
}

/**
 * @brief Returns the winter season identifier
 * @return String "winter"
 *
 * @override Implements Seasons::getSeason()
 *
 * @note This method needs to be implemented to return the actual season name
 */
std::string Winter::getSeason()
{
    return "Winter";
}

/**
 * @brief Handles transition from winter to spring
 *
 * Implements the state transition logic for moving from winter
 * to spring season. This method should update the context's
 * current state to Spring, completing the annual seasonal cycle.
 *
 * @override Implements Seasons::handleChange()
 *
 * @todo Implement the actual state transition logic
 * @todo Add context parameter to access state setter method
 * @note Currently contains placeholder implementation
 *
 * @par Seasonal Transition Logic:
 * Winter → Spring transition typically involves:
 * - Restocking plants after winter dormancy
 * - Transitioning plants from dead/dormant to seed/sapling stages
 * - Preparing for spring planting season
 * - Marking the beginning of new growth cycle
 */
void Winter::handleChange(Nursery *nursery)
{
    nursery->setState(new Spring(inventory));
}