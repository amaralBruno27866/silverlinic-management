#include "forms/PainBodyMap.h"
#include <iostream>

using namespace std;
using namespace SilverClinic::Forms;

int main() {
    cout << "🗺️ Pain Body Map Demo" << endl;
    cout << "=====================" << endl;
    
    try {
        // Reset ID counter for demo
        PainBodyMap::resetIdCounter();
        
        // Create a new Pain Body Map for a case
        int caseProfileId = 400001;
        PainBodyMap pbm(caseProfileId);
        
        cout << "\n📋 Created new Pain Body Map:" << endl;
        cout << "PBM ID: " << pbm.getPBMId() << endl;
        cout << "Case Profile ID: " << pbm.getCaseProfileId() << endl;
        
        // Add pain data for different body parts
        cout << "\n🔧 Adding pain data..." << endl;
        
        // Severe headache on left side
        pbm.setPainForBodyPart("head", true, false, 8, "Severe migraine episodes");
        cout << "✅ Added headache data" << endl;
        
        // Bilateral shoulder pain
        pbm.setPainForBodyPart("shoulders", true, true, 6, "Muscle tension from poor posture");
        cout << "✅ Added shoulder pain data" << endl;
        
        // Lower back pain on right side
        pbm.setPainForBodyPart("low_back", false, true, 9, "Sharp shooting pain when bending");
        cout << "✅ Added lower back pain data" << endl;
        
        // Knee pain with no specific side
        pbm.setPainForBodyPart("knees", false, false, 4, "Mild stiffness in the morning");
        cout << "✅ Added knee pain data" << endl;
        
        // Add general comments
        pbm.setAdditionalComments("Patient reports that pain is worse in the morning and improves with movement. Pain began after a car accident 3 months ago.");
        
        // Display detailed information
        pbm.displayInfo();
        pbm.displaySummary();
        pbm.displayPainMap();
        pbm.displayAnalysis();
        
        // Show JSON representation
        cout << "\n📄 JSON Representation:" << endl;
        cout << pbm.getPainDataJson() << endl;
        
        // Demonstrate individual body part access
        cout << "\n🔍 Individual Body Part Analysis:" << endl;
        if (pbm.hasEntryForBodyPart("head")) {
            BodyPartPain headPain = pbm.getPainForBodyPart("head");
            cout << "Head pain level: " << headPain.pain_level << "/10" << endl;
            cout << "Left side affected: " << (headPain.side_left ? "Yes" : "No") << endl;
            cout << "Comments: " << headPain.comments << endl;
        }
        
        // Show analysis results
        cout << "\n📊 Pain Analysis Results:" << endl;
        cout << "Total affected body parts: " << pbm.getTotalAffectedBodyParts() << endl;
        cout << "Highest pain level: " << pbm.getHighestPainLevel() << "/10" << endl;
        cout << "Average pain level: " << fixed << setprecision(1) << pbm.getAveragePainLevel() << "/10" << endl;
        
        auto severePain = pbm.getMostPainfulBodyParts(7);
        cout << "Body parts with severe pain (7+): ";
        for (const auto& part : severePain) {
            cout << part << " ";
        }
        cout << endl;
        
        auto leftSide = pbm.getLeftSideAffected();
        cout << "Left side affected areas: ";
        for (const auto& part : leftSide) {
            cout << part << " ";
        }
        cout << endl;
        
        auto bilateral = pbm.getBilateralAffected();
        cout << "Bilateral pain areas: ";
        for (const auto& part : bilateral) {
            cout << part << " ";
        }
        cout << endl;
        
        // Demonstrate JSON synchronization
        cout << "\n🔄 JSON Synchronization Demo:" << endl;
        cout << "Original JSON contains " << pbm.getAllPainData().size() << " entries" << endl;
        
        // Set new JSON data
        string newJson = R"({"jaw":{"body_part":"jaw","side_left":true,"side_right":true,"pain_level":5,"comments":"TMJ disorder"}})";
        pbm.setPainDataJson(newJson);
        
        cout << "After setting new JSON, map contains " << pbm.getAllPainData().size() << " entries" << endl;
        cout << "Jaw pain level: " << pbm.getPainForBodyPart("jaw").pain_level << "/10" << endl;
        
        // Show string representation
        cout << "\n📝 String Representation:" << endl;
        cout << pbm.toString() << endl;
        
        // Demonstrate validation
        cout << "\n✅ Validation Demo:" << endl;
        cout << "Is 'head' a valid body part? " << (PainBodyMap::isValidBodyPartName("head") ? "Yes" : "No") << endl;
        cout << "Is 'invalid_part' a valid body part? " << (PainBodyMap::isValidBodyPartName("invalid_part") ? "Yes" : "No") << endl;
        cout << "Is pain level 5 valid? " << (pbm.isValidPainLevel(5) ? "Yes" : "No") << endl;
        cout << "Is pain level 15 valid? " << (pbm.isValidPainLevel(15) ? "Yes" : "No") << endl;
        
        // Show available body parts
        cout << "\n📋 Available Body Parts:" << endl;
        auto bodyParts = PainBodyMap::getStandardBodyParts();
        for (size_t i = 0; i < bodyParts.size(); ++i) {
            cout << bodyParts[i];
            if (i < bodyParts.size() - 1) cout << ", ";
            if ((i + 1) % 6 == 0) cout << endl; // Line break every 6 items
        }
        cout << endl;
        
        cout << "\n🎉 Pain Body Map demo completed successfully!" << endl;
        
    } catch (const exception& e) {
        cerr << "❌ Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
