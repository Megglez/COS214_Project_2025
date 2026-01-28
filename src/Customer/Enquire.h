/**
 * @file Enquire.h
 * @brief Enquire action state for customers asking questions
 *
 * This file defines the Enquire class which represents a customer asking
 * questions at the info desk. Supports both advice questions and information
 * questions about plant inventory.
 */

#ifndef ENQUIRE_H
#define ENQUIRE_H

#include "Action.h"
#include "../Greenhouse/Plant.h"
#include <string>
#include <vector>

class Customer;
class InfoDesk;

/**
 * @class Enquire
 * @brief Represents a customer asking questions at the info desk
 *
 * The Enquire class is a concrete state in the State pattern, representing
 * a customer who has questions about plants or the nursery. Supports two
 * types of questions:
 * - Advice questions: General questions about plant care, seasonal plants, etc.
 * - Information questions: Specific questions about inventory quantities
 *
 * Features:
 * - Dual question types (advice vs. information)
 * - Associated plants of interest
 * - Integration with InfoDesk for staff assistance
 * - State transitions to Browse or leaving
 */
class Enquire : public Action
{
private:
    /**
     * @brief Plants the customer is asking about
     */
    std::vector<Plant *> plantsOfInterest;

    /**
     * @brief The question being asked
     */
    std::string enquiryQuestion;

    /**
     * @brief Type of question: 0 = advice, 1 = information
     */
    int questionType;

    /**
     * @brief Flag indicating if this Enquire owns the plant pointers
     */
    bool ownsPlants;

public:
    /**
     * @brief Construct an Enquire action for a single plant
     * @param plant Pointer to the plant of interest
     * @param question The question being asked (default: empty)
     * @param qType Question type: 0 = advice, 1 = information (default: 0)
     * @param ownsPlantPointers If true, Enquire will delete plants in destructor (default: false)
     *
     * Creates an enquiry about a specific plant.
     */
    Enquire(Plant *plant, const std::string &question = "", int qType = 0, bool ownsPlantPointers = false) : Action("Enquiring"), enquiryQuestion(question), questionType(qType), ownsPlants(ownsPlantPointers)
    {
        if (plant)
            plantsOfInterest.push_back(plant);
    }

    /**
     * @brief Construct an Enquire action for multiple plants
     * @param plants Vector of plants the customer is asking about
     * @param question The question being asked (default: empty)
     * @param qType Question type: 0 = advice, 1 = information (default: 0)
     * @param ownsPlantPointers If true, Enquire will delete plants in destructor (default: false)
     *
     * Creates an enquiry about multiple plants.
     */
    Enquire(std::vector<Plant *> plants, const std::string &question = "", int qType = 0, bool ownsPlantPointers = false) : Action("Enquiring"), plantsOfInterest(plants), enquiryQuestion(question), questionType(qType), ownsPlants(ownsPlantPointers)
    {
        std::cout << "[ENQUIRE CONSTRUCTOR] Created Enquire action with question: " << question << std::endl;
    }

    /**
     * @brief Virtual destructor - cleans up owned plant pointers
     */
    virtual ~Enquire()
    {
        if (ownsPlants)
        {
            for (Plant *plant : plantsOfInterest)
            {
                delete plant;
            }
        }
    }

    /**
     * @brief Handle the enquiry action
     *
     * Displays the customer's question and asks for staff assistance.
     * Implements the Action interface.
     */
    void handle() override;

    /**
     * @brief Handle enquiry with customer context
     * @param customer The customer making the enquiry
     *
     * Requests staff assistance from the info desk.
     */
    void handle(Customer *customer) override;

    /**
     * @brief Get the next action for this customer
     * @return Action* Pointer to next action (Browse or nullptr to leave)
     *
     * After enquiring, customers may:
     * - Return to browsing (new Browse action)
     * - Leave the nursery (nullptr)
     */
    Action *getNextAction() override;

    /**
     * @brief Request staff assistance for answering questions
     * @param customer The customer requesting assistance
     * @param desk Reference to the InfoDesk for staff coordination
     *
     * Forwards the enquiry to the info desk to find available staff.
     */
    void requestStaffAssistance(Customer *customer, InfoDesk &desk) override;

    /**
     * @brief Get the enquiry question text
     * @return std::string The question being asked
     */
    std::string getEnquiryQuestion() const { return enquiryQuestion; }

    /**
     * @brief Get the plants the customer is asking about
     * @return std::vector<Plant*> Vector of plants of interest
     */
    std::vector<Plant *> getPlantsOfInterest() const { return plantsOfInterest; }

    /**
     * @brief Get the question type
     * @return int Question type: 0 = advice question, 1 = information question
     *
     * - Type 0 (advice): Questions like "What plants are good for summer?"
     * - Type 1 (information): Questions like "How many Roses do you have in stock?"
     */
    int getQuestionType() const { return questionType; }

    /**
     * @brief Get the advice question if this is an advice-type enquiry
     * @return std::string The advice question, or empty string if not an advice question
     *
     * Returns the question text only if questionType is 0 (advice question).
     * If this is an information question (type 1), returns an empty string.
     */
    std::string getAdviceQuestion() const
    {
        if (questionType == 0)
        {
            return enquiryQuestion;
        }
        return "";
    }
};
#endif