/**
 * @file Busy.cpp
 * @brief Implementation of the Busy class
 * @author Chaos_Compilers
 * @date 2025
 */

#include "Busy.h"
#include "Staff.h"
#include "InfoDesk.h"
void Busy::handle()
{
	// TODO - implement Busy::handle

	if (staff)
	{
		// Capture the Staff* locally and perform all non-state-switch work first
		Staff *s = staff;
		cout << s->getName() << " is completing this task." << std::endl;

		// Clear current customer and mark availability before switching state
		s->setCurrentCustomer(nullptr);
		s->setAvailability(true);

		// Notify InfoDesk that staff is now available
		InfoDesk *infod = s->getInfodesk();
		if (infod)
		{
			infod->notifyStaffAvailable(s);
		}
		std::cout << "Staff has assisted customer. Staff is now Available." << std::endl;

		// IMPORTANT: Switch state LAST because changeState deletes the current state (this)
		// Do not access any members of this Busy instance after this call
		s->changeState();
		return;
	}
}

Busy::Busy()
{
	// Initialize Busy state
}

Busy::~Busy()
{
}

std::string Busy::getStateName() const
{
	return "Busy";
}

bool Busy::canAcceptCustomer()
{
	return false;
}
