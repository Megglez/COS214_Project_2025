/**
 * @file Summer.cpp
 * @brief Implementation of Summer season behavior
 *
 * Contains the concrete implementation of summer-specific seasonal
 * logic including transitions, command execution, and inventory adjustments.
 */

#include "Summer.h"
#include "SummerCommand.h"
#include "Nursery/Nursery.h"

/**
 * @brief Constructs Summer season and executes summer commands
 * @param inv Pointer to Inventory system for seasonal operations
 *
 * Initializes the summer season state and immediately executes
 * summer-specific commands through the SummerCommand class. This
 * ensures that seasonal adjustments are applied when the summer
 * state is entered.
 *
 * @param inv Valid Inventory pointer for seasonal operations
 *
 * @post commands pointer is set to a new SummerCommand instance
 * @post SummerCommand::execute() is called during construction
 *
 * @note Cleans up any existing commands before assigning new one
 * @warning Assumes commands may have been initialized in base class
 *
 * @par Summer Inventory Adjustments:
 * Based on previous inventory logic, summer typically sets:
 * - Succulents: 100 (high stock - peak season)
 * - Herbs: 100 (high stock - all types available)
 * - Flowers: 100 (high stock - summer annuals)
 * - Trees: 70 (medium stock)
 */
Summer::Summer(Inventory *inv) : inventory(inv)
{
    if (commands)
    {
        delete commands;
    }
    commands = new SummerCommand(inventory);
    commands->execute();
}

std::string Summer::getSeason()
{
    return "Summer";
}

/**
 * @brief Destructor for Summer season
 *
 * Cleans up Summer-specific resources. The inventory pointer
 * is not owned by this class and is not deleted.
 *
 * @note The commands pointer is managed by the base class or
 *       derived class destruction chain.
 */
Summer::~Summer()
{
}

/**
 * @brief Handles transition from summer to autumn
 *
 * Implements the state transition logic for moving from summer
 * to autumn season. This method should update the context's
 * current state to Autumn.
 *
 * @override Implements Seasons::handleChange()
 *
 * @todo Implement the actual state transition logic
 * @todo Add context parameter to access state setter method
 * @note Currently contains placeholder implementation
 *
 *
 * @par Seasonal Transition Logic:
 * Summer → Autumn transition typically involves:
 * - Reducing stock of heat-loving plants
 * - Preparing for autumn planting season
 * - Updating plant stages for cooler weather
 */
void Summer::handleChange(Nursery *nursery)
{
    nursery->setState(new Autumn(inventory));
}