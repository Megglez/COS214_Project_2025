/**
 * @file Manager.cpp
 * @brief Implementation of the Manager class for staff and inventory management
 * @author Chaos_Compilers
 * @date 2025
 */

#include "Manager.h"
#include "../Customer/Customer.h"
#include "../Customer/Enquire.h"
#include "InfoDesk.h"
#include <sstream>
#include <set>

string Manager::trackInventory()
{
	// TODO - implement Manager::trackInventory
	stringstream ss;

	ss << "Here is the current Inventory status: " << endl;

	for (const auto &item : subject->getInventory())
	{
		const auto &plantPtr = item.second.first;
		const auto &quantity = item.second.second;
		if (plantPtr)
		{
			ss << "Plant: " << plantPtr->getName() << ", Quantity: " << quantity << std::endl;
		}
	}

	return ss.str();
}

Manager::Manager(string name, string id, InfoDesk *info, Inventory *inventory) : Staff(name, id, info), subject(inventory)
{
	// TODO - implement Manager::Manager
	setRole();
}

Manager::~Manager()
{
}

bool Manager::canHandleEnquiry()
{
	return true;
}

void Manager::performDuty()
{
	Customer *cust = getCurrentCustomer();
	if (!cust) // no one to help
		return;

	// Only respond once per assistance
	static std::set<Customer *> respondedCustomers;
	if (respondedCustomers.count(cust) > 0)
		return; // Already answered this customer
	respondedCustomers.insert(cust);

	Enquire *enquiry = dynamic_cast<Enquire *>(cust->getAction());
	if (enquiry && enquiry->getQuestionType() == 0) // sales advice
	{
		string question = enquiry->getEnquiryQuestion();
		if (question == "What summer flowers are available")
		{
			cout << "Listing summer plants from inventory:" << endl;
			vector<Plant *> summerPlants = subject->FlowerBySeason("Summer");
			for (const auto &item : summerPlants)
			{
				if (item)
				{
					std::cout << item->getName() << std::endl;
				}
			}
		}
		else if (question == "What winter plants are available?")
		{
			cout << "Listing winter plants from inventory:" << endl;
			vector<Plant *> winterPlants = subject->FlowerBySeason("Winter");
			for (const auto &item : winterPlants)
			{
				if (item)
				{
					std::cout << item->getName() << std::endl;
				}
			}
		}
		else if (question == "What is the best time of day to water my plants?")
		{
			cout << "The best time to water plants is early in the morning or late in the afternoon." << endl;
		}
		else if (question == "How many categories of plants do you sell?")
		{
			cout << "We sell 4 categories: Succulents,Flowers,Trees and Herbs." << endl;
		}
		else
		{
			cout << "General Sales Enquiry response." << endl;
		}
	}
	else if (enquiry && !enquiry->getEnquiryQuestion().empty()) // garden advice: how many of plant X in stock
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

void Manager::setRole()
{
	this->role = "Manager";
}
