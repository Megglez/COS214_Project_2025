#ifndef GARDENER_H
#define GARDENER_H
#include <iostream>
#include "Staff.h"
using namespace std;

class Gardener : public Staff
{
public:
	Inventory *subject;
	Gardener(std::string &name, std::string &id, InfoDesk *infodesk, Inventory *inventory);
	virtual ~Gardener();
	void performDuty() override;
	bool canHandleEnquiry() override;
	void careForPlants();
	void careForPlants(Plant *plant);
	void setRole() override;
};

#endif
