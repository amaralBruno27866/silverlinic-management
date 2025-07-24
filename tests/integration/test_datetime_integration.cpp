#include "../../include/forms/InsuranceCompany.h"
#include "../../include/core/DateTime.h"
#include <iostream>

using namespace SilverClinic;
using namespace SilverClinic::Forms;

int main() {
    // Create an InsuranceCompany with DateTime accident date
    InsuranceCompany ic;
    ic.setClientId(300001);
    ic.setIcCompanyName("Test Insurance Co");
    ic.setIcDateAccident(DateTime(2024, 3, 15, 10, 30, 0));
    
    std::cout << "Insurance Company: " << ic.getIcCompanyName() << std::endl;
    std::cout << "Accident Date: " << ic.getIcDateAccident().toDateString() << std::endl;
    std::cout << "DateTime integration test successful!" << std::endl;
    
    return 0;
}
