#ifndef INVENTORY_H
#define INVENTORY_H

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <map>
#include <string>
#include "Plant.h"
#include "StageOfDevelopment.h"

// Forward declarations for stage classes
class Seed;
class Sapling;
class Prime;
class Wilting;
class Dead;
class Staff;

/**
 * @class Inventory
 * @brief Manages the collection of plants and staff notifications
 *
 * The Inventory class maintains a collection of plants with their quantities
 * and manages staff observers for notifications. It handles seasonal changes
 * that affect both stock levels and plant development stages.
 */
class Inventory
{
private:
    std::map<std::string, std::pair<std::unique_ptr<Plant>, int>> inventoryList; ///< Map of plant names to plant objects and quantities
    std::vector<Staff *> staffList;                                              ///< List of staff members to notify

public:
    /**
     * @brief Protected default constructor
     */
    Inventory();

public:
    /**
     * @brief Main action method for inventory operations
     */
    void action();

    /**
     * @brief Adds a plant to the inventory
     * @param plant Unique pointer to the plant to add
     * @param quantity Number of plants to add (default: 1)
     */
    void addPlant(std::unique_ptr<Plant> plant, int quantity = 1);

    /**
     * @brief Removes a specified quantity of plants from inventory
     * @param plant Unique pointer to the plant to remove
     * @param quantity Number of plants to remove
     */
    void removePlant(std::unique_ptr<Plant> plant, int quantity);

    /**
     * @brief Removes all instances of a specific plant from inventory
     * @param plant Unique pointer to the plant to remove completely
     */
    void removeAll(std::unique_ptr<Plant> plant);

    /**
     * @brief Handles seasonal transition and updates inventory accordingly
     * @param fromSeason The current season being transitioned from
     * @param toSeason The new season being transitioned to
     */
    void seasonalChange(std::string &fromSeason, std::string &toSeason);

    /**
     * @brief Adjusts stock levels based on seasonal requirements
     * @param season The season to adjust stock for
     */
    void adjustStockForSeason(const std::string &season);

    /**
     * @brief Adds default plants for the current season if they don't exist
     * @param season The season to add default plants for
     */
    void addDefaultPlantsForSeason(const std::string &season);

    /**
     * @brief Updates stock level for a specific plant type
     * @param targetType The type of plant to update
     * @param newStockLevel The new stock level to set
     */
    void updateStockByPlantType(const std::string &targetType, int newStockLevel);

    /**
     * @brief Updates development stages of all plants for a new season
     * @param season The new season to adjust stages for
     */
    void updatePlantStagesForSeason(const std::string &season);

    /**
     * @brief Determines the appropriate development stage for a plant in a given season
     * @param plant Pointer to the plant to evaluate
     * @param season The season to determine stage for
     * @return Pointer to the appropriate StageOfDevelopment object
     */
    StageOfDevelopment *determineStageForSeason(Plant *plant, const std::string &season);

    /**
     * @brief Displays the current inventory catalogue
     */
    void getCatalogue();

    /**
     * @brief Attaches a staff member to receive notifications
     * @param staff Pointer to the staff member to attach
     */
    void attach(Staff *staff);

    /**
     * @brief Detaches a staff member from notifications
     * @param staff Pointer to the staff member to detach
     */
    void detach(Staff *staff);

    /**
     * @brief Notifies all attached staff members with a message
     * @param message The message to send to staff members
     */
    void notify(std::string &message);

    /**
     * @brief Virtual destructor
     */
    virtual ~Inventory();

    /**
     * @brief returns the inventoryList
     * @returns std::map<std::string, std::pair<std::unique_ptr<Plant>
     */
    std::map<std::string, std::pair<std::unique_ptr<Plant>, int>> &getInventory();

    /**
     * @brief returns the number of stock of tha plant
     * @returns int
     */
    int getPlantNumber(Plant *plant);

    vector<Plant *> FlowerBySeason(std::string season);
};

#endif