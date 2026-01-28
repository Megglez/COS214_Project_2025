/**
 * @file Autumn.cpp
 * @brief Implementation of Autumn season behavior
 *
 * Contains the concrete implementation of autumn-specific seasonal
 * logic including transitions, command execution, and inventory adjustments.
 */

#include "Autumn.h"
#include "AutumnCommand.h"
#include "Nursery/Nursery.h"

/**
 * @brief Constructs Autumn season and executes autumn commands
 * @param inv Pointer to Inventory system for seasonal operations
 *
 * Initializes the autumn season state and immediately executes
 * autumn-specific commands through the AutumnCommand class. This
 * ensures that seasonal adjustments are applied when the autumn
 * state is entered.
 *
 * @param inv Valid Inventory pointer for seasonal operations
 *
 * @post commands pointer is set to a new AutumnCommand instance
 * @post AutumnCommand::execute() is called during construction
 *
 * @note Cleans up any existing commands before assigning new one
 * @warning Assumes commands may have been initialized in base class
 */
Autumn::Autumn(Inventory *inv) : inventory(inv)
{
    if (commands)
    {
        delete commands;
    }
    commands = new AutumnCommand(inventory);
    commands->execute();
}

/**
 * @brief Destructor for Autumn season
 *
 * Cleans up Autumn-specific resources. The inventory pointer
 * is not owned by this class and is not deleted.
 *
 * @note The commands pointer is managed by the base class or
 *       derived class destruction chain.
 */
Autumn::~Autumn()
{
}

std::string Autumn::getSeason()
{
    return "Autumn";
}

/**
 * @brief Handles transition from autumn to winter
 *
 * Implements the state transition logic for moving from autumn
 * to winter season. This method should update the context's
 * current state to Winter.
 *
 * @override Implements Seasons::handleChange()
 *
 * @todo Implement the actual state transition logic
 * @todo Add context parameter to access state setter method
 * @note Currently contains placeholder implementation
 *
 * @par Expected Implementation:
 * @code
 * void Autumn::handleChange() {
 *     context->setState(new Winter(inventory));
 * }
 * @endcode
 */
void Autumn::handleChange(Nursery *nursery)
{
    nursery->setState(new Winter(inventory));
}