/**
 * @file Gardener.cpp
 * @brief Implementation of the Gardener class for plant care and inventory queries
 * @author Chaos_Compilers
 * @date 2025
 */

#include "Gardener.h"
#include "../Customer/Customer.h"
#include "../Customer/Enquire.h"
#include "InfoDesk.h"
#include <sstream>
#include <set>

void Gardener::careForPlants(Plant *plant)
{
	cout << "Gardener " << getName() << " is caring for " << plant->getName() << endl;
	plant->helpPlant();
}

Gardener::Gardener(std::string &name, std::string &id, InfoDesk *infodesk, Inventory *inventory) : Staff(name, id, infodesk), subject(inventory)
{
	setRole();
}

Gardener::~Gardener()
{
}

void Gardener::careForPlants()
{
	// Implementation for careForPlants without parameters
}

void Gardener::performDuty() // customer
{
	Customer *cust = getCurrentCustomer();
	if (!cust)
		return;

	// Only respond once per assistance
	static std::set<Customer *> respondedCustomers;
	if (respondedCustomers.count(cust) > 0)
		return; // Already answered this customer
	respondedCustomers.insert(cust);

	Enquire *enquiry = dynamic_cast<Enquire *>(cust->getAction());
	if (enquiry && !enquiry->getEnquiryQuestion().empty()) // garden advice: how many of plant X in stock
	{
		string ss = enquiry->getEnquiryQuestion();
		std::istringstream iss(ss);
		std::string word;
		int count = 0;
		while (iss >> word)
		{
			++count;
			if (count == 3)
			{
				// word is the 3rd word
				break;
			}
		}
		for (const auto &item : subject->getInventory())
		{
			const auto &plantName = item.first;
			const auto &quantity = item.second.second;
			if (plantName == word)
			{
				cout << "Plant: " << plantName << ", Quantity: " << quantity << std::endl;
				break;
			}
		}
	}

	// Note: Customer will be released by GUI after timer expires
	// Staff state will be managed by the GUI
}

bool Gardener::canHandleEnquiry()
{
	return true;
}

void Gardener::setRole()
{
	this->role = "Gardener";
}
