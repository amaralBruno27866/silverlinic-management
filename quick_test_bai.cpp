#include "forms/BeckAnxietyInventory.h"
#include <iostream>

using namespace SilverClinic::Forms;
using namespace std;

int main() {
    cout << "=== Beck Anxiety Inventory Quick Test ===" << endl;
    
    // Test basic functionality
    BeckAnxietyInventory::resetIdCounter();
    BeckAnxietyInventory bai(12345);
    
    cout << "BAI ID: " << bai.getBAIId() << endl;
    cout << "Type: " << bai.getType() << endl;
    cout << "Case Profile ID: " << bai.getCaseProfileId() << endl;
    
    // Test setting some questions
    bai.setQuestion1(2);
    bai.setQuestion2(1);
    bai.setQuestion3(3);
    
    cout << "Question 1: " << bai.getQuestion1() << endl;
    cout << "Question 2: " << bai.getQuestion2() << endl;
    cout << "Question 3: " << bai.getQuestion3() << endl;
    cout << "Total Score: " << bai.getTotalScore() << endl;
    cout << "Severity Level: " << bai.getSeverityLevel() << endl;
    
    cout << "=== BAI Test Completed Successfully! ===" << endl;
    return 0;
}
