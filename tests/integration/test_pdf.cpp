#include <iostream>
#include <sqlite3.h>
#include "managers/CaseProfileManager.h"
#include "core/DatabaseConfig.h"

using namespace std;
using namespace SilverClinic;

int main() {
    cout << "🏥 Testing PDF Generation" << endl;
    cout << "========================" << endl;
    
    // Initialize database using the correct path
    sqlite3* db = nullptr;
    string dbPath = DatabaseConfig::MAIN_DATABASE_PATH; // Uses data/clinic.db
    
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        cerr << "Error: Cannot open database: " << sqlite3_errmsg(db) << endl;
        cerr << "Database path: " << dbPath << endl;
        return 1;
    }
    
    cout << "✅ Database opened successfully: " << dbPath << endl;
    
    // Create CaseProfileManager
    CaseProfileManager caseManager(db);
    
    // Ensure PDFs directory exists
    system("mkdir -p data/pdfs");
    
    // Test PDF generation for case ID 400001 (sample data)
    string outputPath = "data/pdfs/test_case_report.pdf";
    string reportType = "summary";
    
    cout << "📄 Generating PDF report..." << endl;
    cout << "   Case ID: 400001" << endl;
    cout << "   Output: " << outputPath << endl;
    cout << "   Type: " << reportType << endl;
    
    bool result = caseManager.generatePDFReport(400001, outputPath, reportType);
    
    if (result) {
        cout << "✅ PDF generated successfully!" << endl;
        cout << "   File: " << outputPath << endl;
    } else {
        cout << "❌ Failed to generate PDF" << endl;
    }
    
    // Test detailed report
    string detailedPath = "data/pdfs/test_case_detailed.pdf";
    cout << "\n📄 Generating detailed PDF report..." << endl;
    bool detailedResult = caseManager.generatePDFReport(400001, detailedPath, "detailed");
    
    if (detailedResult) {
        cout << "✅ Detailed PDF generated successfully!" << endl;
        cout << "   File: " << detailedPath << endl;
    } else {
        cout << "❌ Failed to generate detailed PDF" << endl;
    }
    
    // Clean up
    sqlite3_close(db);
    cout << "\n🎯 PDF Testing completed!" << endl;
    
    return (result && detailedResult) ? 0 : 1;
}
