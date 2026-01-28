/**
 * @file Staff.cpp
 * @brief Implementation of the Staff base class
 * @author Chaos_Compilers
 * @date 2025
 */

#include "Staff.h"
#include "../Customer/Customer.h"
#include "InfoDesk.h"

void Staff::changeState()
{
	// TODO - implement Staff ::changeState
	if (this->state)
	{
		std::string oldState = this->state->getStateName();
		if (this->state->getStateName() == "Busy")
		{
			delete this->state;
			this->state = new Available();
			this->state->setContext(this);
			cout << "Staff " << name << " state changed from " << oldState << " to " << this->state->getStateName() << endl;
		}
		else
		{
			delete this->state;
			this->state = new Busy();
			this->state->setContext(this);
			cout << "Staff " << name << " state changed from " << oldState << " to " << this->state->getStateName() << endl;
		}
	}
}

Staff::Staff(const std::string &name, const std::string &id, InfoDesk *infodesk)
{
	this->name = name;
	this->staffID = id;
	available = true;
	infoDesk = infodesk;
	nextInChain = nullptr;
	currentCustomer = nullptr;
	state = new Available();
	state->setContext(this);
}

void Staff::update(const std::string &update)
{
	std::cout << "New Notification: " << update << std::endl;
}

Staff::~Staff()
{
	delete state;
}

void Staff::setAvailability(bool isAvailable)
{
	available = isAvailable;
}

Staff *Staff::getNextInChain()
{
	return nextInChain;
}

void Staff::setNextInChain(Staff *ss)
{
	nextInChain = ss;
}

std::string Staff::getName()
{
	return name;
}

std::string Staff::getID()
{
	return staffID;
}

std::string Staff::getStaffType()
{

	return role;
}
bool Staff::getAvailability()
{
	return available;
}

InfoDesk *Staff::getInfodesk()
{
	return this->infoDesk;
}

void Staff::completeTask()
{
	state->handle();
}

void Staff::assistCustomer(Customer *cc)
{
	if (!cc)
	{
		cout << "Staff " << name << "cannot assist null Customer" << std::endl;
		return;
	}
	if (!getAvailability()) // but staff will be available
	{
		cout << "Staff " << name << "is not Available." << endl;
		return;
	}

	cout << "Staff " << name << " is assisting Customer " << cc->getId() << endl;
	currentCustomer = cc;
	this->changeState();
	available = false;
	cout << "Staff " << name << " status is now: " << getStateName() << endl;
	performDuty();
	cc->setAssignedStaff(this);
}

Staff *Staff::handleEnquiryRequest()
{
	if (canHandleEnquiry() && getAvailability())
	{
		std::cout << getStaffType() << " " << name << "can handle enquiry." << std::endl;
		return this;
	}

	if (nextInChain)
	{
		std::cout << getStaffType() << " " << name << "can't handle enquiry.Passing on to.." << std::endl;
		std::cout << nextInChain->getStaffType();
		return nextInChain->handleEnquiryRequest();
	}

	// otherwise
	std::cout << "No staff avaialable to handle enquiry." << std::endl;
	return nullptr;
}

void Staff::registerToAllStaff(InfoDesk *desk)
{
	if (!desk)
	{
		std::cout << "cannot register. Null Infodesk." << std::endl;
		return;
	}

	if (infoDesk)
	{
		std::cout << "Staff already registered in AllStaff." << std::endl;
		return;
	}

	infoDesk = desk;
	desk->addStaff(this);
	std::cout << name << " registered from AllStaff." << std::endl;
}

void Staff ::unregisterFromAllStaff()
{
	if (!infoDesk)
	{
		std::cout << "Staff not registered!" << std::endl;
		return;
	}

	this->infoDesk->removeStaff(this);
	infoDesk = nullptr;
	std::cout << name << " unregistered from AllStaff." << std::endl;
}

std::string Staff::getStateName()
{
	if (state)
	{
		return state->getStateName();
	}
	return "Unknown";
}

Customer *Staff::getCurrentCustomer()
{
	return currentCustomer;
}

void Staff::setCurrentCustomer(Customer *cc)
{
	currentCustomer = cc;
}
