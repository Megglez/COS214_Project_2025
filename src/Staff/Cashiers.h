#ifndef CASHIERS_H
#define CASHIERS_H
#include <iostream>
#include <queue>
#include "Staff.h"
#include "../Greenhouse/Inventory.h"
using namespace std;

class Cashiers : public Staff
{
private:
	std::queue<Customer *> customerQueue;
	Customer *currentCustomer;
	bool isProcessing;

public:
	Inventory *subject;

	// Queue management
	void enqueueCustomer(Customer *customer);
	Customer *dequeueCustomer();
	bool hasCustomersInQueue() const;
	int getQueueSize() const;
	const std::queue<Customer *> &getQueue() const { return customerQueue; }

	// Transaction processing
	void makeTransaction();
	void emptyBasket(Customer *customer);
	void processNextCustomer();

	// Staff interface
	std::string getStaffType();
	bool canHandleEnquiry() override;
	void performDuty() override;
	void setRole() override;

	Cashiers(std::string &name, std::string &id, InfoDesk *infodesk);
	virtual ~Cashiers();
};

#endif
