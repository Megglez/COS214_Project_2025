/**
 * @file InfoDesk.cpp
 * @brief Implementation of the InfoDesk class for staff coordination and customer routing
 * @author Chaos_Compilers
 * @date 2025
 */

#include "InfoDesk.h"
#include "Staff.h"
#include "../Customer/Customer.h"
#include "../Customer/Enquire.h"
using namespace std;

void InfoDesk::notify(Staff *staff)
{
    cout << "Customer needs your assistance";
    staff->getID(); // remove later
}

InfoDesk::InfoDesk()
{
    // TODO - implement InfoDesk::InfoDesk
    chainHead = nullptr;
    while (!waitingCustomers.empty())
    {
        waitingCustomers.pop();
    }
    cout << "Info Desk created." << endl;
}

InfoDesk::~InfoDesk()
{
    cout << "Closing Info Desk." << endl;
}

InfoDesk *InfoDesk::getInfodesk()
{
    return this;
}

void InfoDesk::handleCustomer(Customer *customer) // called by enquiring customerState
{
    if (!customer)
    {
        cout << "Customer is null. Cannot assign to staff member." << endl;
        return;
    }

    if (customer->getAction()->getActionName() != "Enquiring")
    {
        cout << "Customer is not in Enquiring state.Cannot assign to staff member. " << endl;
        return;
    }

    if (customer->getAction()->getActionName() == "Enquiring")
    {
        cout << ".....Requesting assistance for customer..... " << customer->getId() << endl;
        // get a staff member
        Staff *assignedStaff;
        Enquire *enquiry = dynamic_cast<Enquire *>(customer->getAction());
        if (enquiry && enquiry->getQuestionType() == 0) // sales advice
        {
            assignedStaff = findQnAStaff(customer);
        }

        else if (enquiry && enquiry->getQuestionType() == 1)
        {
            assignedStaff = findInventoryStaff(customer);
        }

        else
        {
            cout << "Unknown enquiry type.General Staff Assignment." << endl;
            return;
        }

        if (assignedStaff)
        {
            cout << assignedStaff->getName() + " is on their way to assist customer. " << endl;
            assignedStaff->assistCustomer(customer);
            customer->setAssignedStaff(assignedStaff);
            cout << "Assigned staff " << assignedStaff->getName() << " to customer " << customer->getId() << endl;
        }

        else
        {
            waitingCustomers.push(customer);
            cout << "All staff unavailable. Queue customer " << waitingCustomers.size() << " for assistance." << endl;
        }

        // inventory advice
        /*Staff *assignedStaff;
        assignedStaff= findAvailableStaffThroughChain();
        if(assignedStaff)
        {
            std::cout<<assignedStaff->getName() +" is on their way to assist customer. "<<std::endl;
        assignedStaff->assistCustomer(customer);
        customer->setAssignedStaff(assignedStaff);
      }

      else
      {
        waitingCustomers.push(customer);
        std::cout<<"All staff unavailable. Queue customer "<< waitingCustomers.size()<<" for assistance."<<std::endl;

      }*/
    }
}

Staff *InfoDesk::findQnAStaff(Customer *customer)
{
    cout << "Finding QnA Staff for customer " << customer->getId() << endl;

    for (Staff *staff : AllStaff)
    {
        if (staff->getStaffType() == "SalesStaff" && staff->getAvailability())
        {
            cout << "Found Sales Staff: " << staff->getName() << endl;
            return staff;
        }
    }

    for (Staff *staff : AllStaff)
    {
        if (staff->getStaffType() == "Manager" && staff->getAvailability())
        {
            cout << "Found Manager: " << staff->getName() << endl;
            return staff;
        }
    }
    cout << "No QnA Staff available currently." << endl;
    return nullptr;
}

Staff *InfoDesk::findInventoryStaff(Customer *customer)
{
    cout << "Finding Inventory Staff for customer " << customer->getId() << endl;

    for (Staff *staff : AllStaff)
    {
        if (staff->getStaffType() == "Gardener" && staff->getAvailability())
        {
            cout << "Found Gardener: " << staff->getName() << endl;
            return staff;
        }
    }
    for (Staff *staff : AllStaff)
    {
        if (staff->getStaffType() == "Manager" && staff->getAvailability())
        {
            cout << "Found Manager: " << staff->getName() << endl;
            return staff;
        }
    }
    cout << "No Inventory Staff available currently." << endl;
    return nullptr;
}

bool InfoDesk::FindAvailableStaff(Customer *cc)
{
    cc->getId();
    Staff *assignedStaff;
    assignedStaff = findAvailableStaffThroughChain();
    if (assignedStaff)
    {

        std::cout << "Found staff member. Staff - " + assignedStaff->getName() << std::endl;
    }
    else
    {
        std::cout << "Currently no staff available. Please queue for assistance." << std::endl;
        return false;
    }
    // Return true if a staff member was found, false otherwise
    return (assignedStaff != nullptr);
}

void InfoDesk::processWaitingCustomers()
{
    std::cout << "....Processing Waiting Customers....." << std::endl;

    if (waitingCustomers.empty())
    {
        std::cout << "No customers waiting in queue." << std::endl;
        return;
    }

    int processed = 0;
    bool foundStaff;
    do
    {
        foundStaff = false;

        if (waitingCustomers.empty())
        {
            break;
        }

        Customer *customer = waitingCustomers.front();

        // Check if customer or action is null (customer left or invalid)
        if (!customer || !customer->getAction())
        {
            std::cout << "Customer in queue is null or has no action. Removing from queue." << std::endl;
            waitingCustomers.pop();
            continue;
        }

        if (customer->getAction()->getActionName() != "Enquiring")
        {
            std::cout << "Customer " << customer->getId() << " is no longer enquiring. Removing from queue." << std::endl;
            waitingCustomers.pop();
            continue;
        }

        if (customer->getAssignedStaff())
        {
            std::cout << "Customer " << customer->getId() << " already has assigned staff. Removing from queue." << std::endl;
            waitingCustomers.pop();
            continue;
        }
        Staff *assignedStaff = nullptr;
        Enquire *enquiry = dynamic_cast<Enquire *>(customer->getAction());

        if (enquiry && enquiry->getQuestionType() == 0) // sales advice
        {
            assignedStaff = findQnAStaff(customer);
        }
        else if (enquiry && enquiry->getQuestionType() == 1) // inventory advice
        {

            assignedStaff = findInventoryStaff(customer);
        }

        if (assignedStaff)
        {
            std::cout << "Assigning " << assignedStaff->getName() << " to customer " << customer->getId() << std::endl;
            assignedStaff->assistCustomer(customer);
            customer->setAssignedStaff(assignedStaff);
            waitingCustomers.pop();
            processed++;
            foundStaff = true;
        }
        else
        {
            std::cout << "No suitable staff available for customer " << customer->getId() << ". Remaining in queue. Stop iteration..." << std::endl;
            break;
        }
    } while (foundStaff);
    {
        cout << "Continuing to process waiting customers..." << endl;
        cout << "Customers left in queue: " << waitingCustomers.size() << endl;
        if (!waitingCustomers.empty())
        {
            cout << "Who's in customer in queue..." << endl;
            queue<Customer *> tempQueue = waitingCustomers; // Create a copy of the original queue
            while (!tempQueue.empty())
            {
                Customer *cust = tempQueue.front();
                std::cout << "Customer ID: " << cust->getId() << std::endl;
                tempQueue.pop();
                if (!tempQueue.empty())
                {
                    std::cout << "->..." << endl;
                }
            }
        }
        cout << endl;
    }
}

bool InfoDesk::removeStaff(Staff *ss)
{
    if (!ss)
    {
        std::cout << "staff is NULL. Cannot remove Staff member." << std::endl;
        return false;
    }

    auto it = std::find(AllStaff.begin(), AllStaff.end(), ss);

    if (it != AllStaff.end())
    {
        AllStaff.erase(it);
        std::cout << "Staff is now Busy." << std::endl;
        buildChain();
        return true;
    }
    else
    {
        std::cout << "Could not find staff member. " << std::endl;
        return false;
    }
}

bool InfoDesk::addStaff(Staff *ss)
{
    if (!ss)
    {
        std::cout << "staff is NULL. Cannot add Staff member." << std::endl;
        return false;
    }

    auto it = std::find(AllStaff.begin(), AllStaff.end(), ss);

    if (it != AllStaff.end())
    {
        std::cout << "Staff Already set as Available." << std::endl;
        return false;
    }

    AllStaff.push_back(ss); // added
    std::cout << ss->getName() + " is now available for assistance." << std::endl;
    buildChain();
    return true;
}

void InfoDesk::buildChain()
{
    clearChain();
    if (AllStaff.empty())
    {
        std::cout << "Currently no staff registered with system." << std::endl;
        return;
    }

    std::vector<std::string> order = {"Sales", "Gardener", "Manager", "Cashier"};
    std::vector<Staff *> sales = getStaffByType("SalesStaff");
    std::vector<Staff *> gardeners = getStaffByType("Gardener");
    std::vector<Staff *> managers = getStaffByType("Manager");
    std::vector<Staff *> chashiers = getStaffByType("Cashier");

    chainHead = nullptr;
    Staff *current = nullptr;

    for (Staff *staff : sales)
    {
        if (!chainHead)
        {
            chainHead = staff;
            current = chainHead;
        }
        else
        {
            current->setNextInChain(staff);
            current = staff;
        }
    }

    for (Staff *staff : gardeners)
    {
        if (!chainHead)
        {
            chainHead = staff;
            current = chainHead;
        }
        else
        {
            current->setNextInChain(staff);
            current = staff;
        }
    }

    for (Staff *staff : managers)
    {
        if (!chainHead)
        {
            chainHead = staff;
            current = chainHead;
        }
        else
        {
            current->setNextInChain(staff);
            current = staff;
        }
    }

    for (Staff *staff : chashiers)
    {
        if (!chainHead)
        {
            chainHead = staff;
            current = chainHead;
        }
        else
        {
            current->setNextInChain(staff);
            current = staff;
        }
    }

    if (current)
    {
        current->setNextInChain(nullptr);
    }

    std::cout << "In chain: " << std::endl;
    std::cout << sales.size() << " SalesStaff " << std::endl;
    std::cout << managers.size() << " Managers" << std::endl;
    std::cout << gardeners.size() << " Gardeners" << std::endl;
    std::cout << chashiers.size() << " Cashiers" << std::endl;
}

void InfoDesk::clearChain()
{
    if (chainHead)
    {
        Staff *current = chainHead;
        while (current)
        {
            Staff *next = current->getNextInChain();
            current->setNextInChain(nullptr);
            current = next;
        }
        chainHead = nullptr;
    }
}

Staff *InfoDesk::findAvailableStaffThroughChain()
{
    if (!chainHead)
    {
        return nullptr;
    }

    Staff *current = chainHead;
    while (current)
    {
        // Check if the current staff member is available
        if (current->getAvailability() == true)
        {
            return current;
        }
        current = current->getNextInChain();
    }
    return nullptr; // No available staff found
}

vector<Staff *> InfoDesk::getStaffByType(std::string type) const
{
    vector<Staff *> result;
    for (Staff *ss : AllStaff)
    {
        if (ss->getStaffType() == type)
        {
            result.push_back(ss);
        }
    }
    return result;
}

void InfoDesk::notifyStaffAvailable(Staff *freedStaff)
{
    if (!freedStaff)
    {
        std::cout << "Cannot recognise staff. Staff is null." << std::endl;
        return;
    }

    std::cout << freedStaff->getName() << "is available." << std::endl;
}

void InfoDesk::AssignStaffToCustomer(Customer *cc)
{
    if (cc == nullptr)
    {
        std::cout << "Customer is Null. Cannot assign staff member to customer." << std::endl;
        return;
    }

    Staff *assignedStaff;
    assignedStaff = findAvailableStaffThroughChain();
    if (assignedStaff)
    {
        std::cout << "Assigning immediately." << std::endl;
        std::cout << assignedStaff->getName() << " assigned to customer " << cc->getId() << std::endl;
        assignedStaff->assistCustomer(cc);
        cc->setAssignedStaff(assignedStaff);
    }

    else
    {
        waitingCustomers.push(cc);
        std::cout << "No Staff available. Customer added to queue." << std::endl;
        std::cout << "Current queue size: " << waitingCustomers.size() << std::endl;
    }
}

vector<Staff *> InfoDesk::getAllStaff()
{
    return AllStaff;
}

void InfoDesk::removeCustomerFromQueue(Customer *customer)
{
    if (!customer)
    {
        return;
    }

    // Since std::queue doesn't support removal, we need to rebuild it without the customer
    std::queue<Customer *> newQueue;
    int removedCount = 0;

    while (!waitingCustomers.empty())
    {
        Customer *current = waitingCustomers.front();
        waitingCustomers.pop();

        if (current != customer)
        {
            newQueue.push(current);
        }
        else
        {
            removedCount++;
            std::cout << "Removed customer " << customer->getId() << " from waiting queue" << std::endl;
        }
    }

    waitingCustomers = newQueue;

    if (removedCount > 0)
    {
        std::cout << "Removed " << removedCount << " instance(s) of customer " << customer->getId() << " from queue. Queue size now: " << waitingCustomers.size() << std::endl;
    }
}
