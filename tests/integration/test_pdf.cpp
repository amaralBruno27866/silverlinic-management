#include <iostream>
#include <sqlite3.h>
#include "managers/CaseProfileManager.h"
#include "core/DatabaseConfig.h"
#include "forms/AutomobileAnxietyInventory.h"
#include "managers/AutomobileAnxietyInventoryManager.h"

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

    // Insert a sample AAI form via manager API so PDF embedding section has data
    {
        SilverClinic::Forms::AutomobileAnxietyInventory::resetIdCounter(); // ensure deterministic id start (700001)
        SilverClinic::Forms::AutomobileAnxietyInventory aai(400001);
        // Set a few representative answers
        aai.setQuestion1(true); aai.setQuestion2(false); aai.setQuestion3(true);
        aai.setQuestion14Driver(true); aai.setQuestion19(true); aai.setQuestion19Crossing(true);
        aai.setQuestion15A(true); aai.setQuestion15B("Family member present");
        aai.setQuestion20(true); aai.setQuestion22(true);
    AutomobileAnxietyInventoryManager aaiMgr(db);
        aaiMgr.create(aai);
    }
    
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
    
    // Quick DB check: ensure at least one AAI record exists for the case (sanity for embedding)
    {
        const char* countSql = "SELECT COUNT(*) FROM automobile_anxiety_inventory WHERE case_profile_id=400001";
        sqlite3_stmt* st=nullptr; if(sqlite3_prepare_v2(db,countSql,-1,&st,nullptr)==SQLITE_OK){
            int rc = sqlite3_step(st); if(rc==SQLITE_ROW){ int c = sqlite3_column_int(st,0); cout << "AAI records for case 400001: " << c << endl; }
            sqlite3_finalize(st);
        }
    }

    // Clean up
    sqlite3_close(db);
    cout << "\n🎯 PDF Testing completed!" << endl;
    
    return (result && detailedResult) ? 0 : 1;
}
