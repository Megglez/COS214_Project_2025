/**
 * @file Spring.cpp
 * @brief Implementation of Spring season behavior
 *
 * Contains the concrete implementation of spring-specific seasonal
 * logic including transitions, command execution, and inventory adjustments.
 */

#include "Spring.h"
#include "SpringCommand.h"
#include "Nursery/Nursery.h"

/**
 * @brief Constructs Spring season and executes spring commands
 * @param inv Pointer to Inventory system for seasonal operations
 *
 * Initializes the spring season state and immediately executes
 * spring-specific commands through the SpringCommand class. This
 * ensures that seasonal adjustments are applied when the spring
 * state is entered.
 *
 * @param inv Valid Inventory pointer for seasonal operations
 *
 * @post commands pointer is set to a new SpringCommand instance
 * @post SpringCommand::execute() is called during construction
 *
 * @note Cleans up any existing commands before assigning new one
 * @warning Assumes commands may have been initialized in base class
 *
 * @par Spring Inventory Adjustments:
 * Based on previous inventory logic, spring typically sets:
 * - Succulents: 25 (low stock - still risky)
 * - Herbs: 80 (good stock)
 * - Flowers: 90 (high stock)
 * - Trees: 100 (high stock - prime planting time)
 */
Spring::Spring(Inventory *inv) : Seasons(), inventory(inv)
{
    if (commands)
    {
        delete commands;
    }
    commands = new SpringCommand(inventory);
    commands->execute();
}

/**
 * @brief Destructor for Spring season
 *
 * Cleans up Spring-specific resources. The inventory pointer
 * is not owned by this class and is not deleted.
 *
 * @note The commands pointer is managed by the base class or
 *       derived class destruction chain.
 */
Spring::~Spring()
{
}

/**
 * @brief Returns the spring season identifier
 * @return String "spring"
 *
 * @override Implements Seasons::getSeason()
 *
 * @note This method needs to be implemented to return the actual season name
 */
std::string Spring::getSeason()
{
    return "Spring";
}

/**
 * @brief Handles transition from spring to summer
 *
 * Implements the state transition logic for moving from spring
 * to summer season. This method should update the context's
 * current state to Summer.
 *
 * @override Implements Seasons::handleChange()
 *
 * @todo Implement the actual state transition logic
 * @todo Add context parameter to access state setter method
 * @note Currently contains placeholder implementation
 *
 * @par Seasonal Transition Logic:
 * Spring → Summer transition typically involves:
 * - Increasing stock of summer-flowering plants
 * - Transitioning plants from seedling to prime stages
 * - Preparing for peak growing season
 * - Adjusting for warmer weather conditions
 */
void Spring::handleChange(Nursery *nursery)
{
    nursery->setState(new Summer(inventory));
}