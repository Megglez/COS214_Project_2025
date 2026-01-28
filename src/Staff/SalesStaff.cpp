/**
 * @file SalesStaff.cpp
 * @brief Implementation of the SalesStaff class for customer sales assistance
 * @author Chaos_Compilers
 * @date 2025
 */

#include "SalesStaff.h"
#include "../Customer/Customer.h"
#include "../Customer/Enquire.h"
#include "InfoDesk.h"
#include <set>

SalesStaff::SalesStaff(string &name, string &id, InfoDesk *infodesk, Inventory *inventory) : Staff(name, id, infodesk), subject(inventory)
{
	// TODO - implement SalesStaff::SalesStaff
	setRole();
}

SalesStaff::~SalesStaff()
{
}

bool SalesStaff::canHandleEnquiry()
{
	return true;
}

void SalesStaff::performDuty() // 1 job
{
	// Check if we have a customer assigned
	Customer *cust = getCurrentCustomer();
	if (!cust)
		return;

	// already checked question type=0;
	Enquire *enquiry = dynamic_cast<Enquire *>(cust->getAction());
	if (!enquiry)
		return;

	// Only respond once per assistance - check if we've already responded
	static std::set<Customer *> respondedCustomers;
	if (respondedCustomers.count(cust) > 0)
		return; // Already answered this customer
	respondedCustomers.insert(cust);

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

	// Note: Customer will be released by GUI after timer expires
	// Staff state will be managed by the GUI
}

void SalesStaff::setRole()
{
	role = "SalesStaff";
}
