#ifndef INFODESK_H
#define INFODESK_H
#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

// Forward declarations
class Staff;
class Customer;
class InfoDesk
{
private:
	//
	Staff *chainHead;						 // staff member with most priority
	std::queue<Customer *> waitingCustomers; // queued customers
	std::vector<Staff *> AllStaff;			 // all staff registered to infodesk
	Staff *findAvailableStaffThroughChain();

public:
	virtual void notify(Staff *staff); // notify

	InfoDesk();
	~InfoDesk();
	InfoDesk *getInfodesk();

	void buildChain();
	void clearChain();
	// assogns staff to customer
	void handleCustomer(Customer *customer); // called by requestAssistance

	Staff *findQnAStaff(Customer *customer);
	Staff *findInventoryStaff(Customer *customer);

	bool FindAvailableStaff(Customer *cc);
	bool removeStaff(Staff *ss);
	bool addStaff(Staff *ss);
	void processWaitingCustomers();
	void notifyStaffAvailable(Staff *freedStaff);
	vector<Staff *> getStaffByType(std::string ss) const; // vector of staff of type ss
	void AssignStaffToCustomer(Customer *cc);
	vector<Staff *> getAllStaff();
	void removeCustomerFromQueue(Customer *customer);
};

#endif
