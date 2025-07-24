#include "utils/PDFConfig.h"

using namespace std;
using namespace SilverClinic;

// ========================================
// Color Definitions
// ========================================

const PDFConfig::Color PDFConfig::HEADER_BLUE(0.12f, 0.23f, 0.54f);      // #1e3a8a
const PDFConfig::Color PDFConfig::SECTION_GRAY(0.97f, 0.98f, 0.99f);     // #f8fafc
const PDFConfig::Color PDFConfig::TEXT_BLACK(0.0f, 0.0f, 0.0f);          // #000000
const PDFConfig::Color PDFConfig::TEXT_SECONDARY(0.39f, 0.45f, 0.55f);   // #64748b
const PDFConfig::Color PDFConfig::STATUS_ACTIVE(0.06f, 0.72f, 0.51f);    // #10b981
const PDFConfig::Color PDFConfig::STATUS_CLOSED(0.86f, 0.15f, 0.15f);    // #dc2626
const PDFConfig::Color PDFConfig::STATUS_PENDING(0.96f, 0.62f, 0.04f);   // #f59e0b
const PDFConfig::Color PDFConfig::BORDER_LIGHT(0.89f, 0.91f, 0.94f);     // #e2e8f0

// ========================================
// Typography Settings
// ========================================

const float PDFConfig::TITLE_FONT_SIZE = 16.0f;
const float PDFConfig::SUBTITLE_FONT_SIZE = 12.0f;
const float PDFConfig::TEXT_FONT_SIZE = 10.0f;
const float PDFConfig::NOTES_FONT_SIZE = 9.0f;
const float PDFConfig::FOOTER_FONT_SIZE = 8.0f;

// ========================================
// Layout Dimensions
// ========================================

const float PDFConfig::PAGE_WIDTH = 595.0f;      // A4 width in points
const float PDFConfig::PAGE_HEIGHT = 842.0f;     // A4 height in points
const float PDFConfig::MARGIN_LEFT = 57.0f;      // 2cm in points
const float PDFConfig::MARGIN_RIGHT = 57.0f;     // 2cm in points
const float PDFConfig::MARGIN_TOP = 57.0f;       // 2cm in points
const float PDFConfig::MARGIN_BOTTOM = 57.0f;    // 2cm in points

const float PDFConfig::SECTION_PADDING = 10.0f;
const float PDFConfig::LINE_SPACING = 14.0f;
const float PDFConfig::SECTION_SPACING = 20.0f;

// ========================================
// Text Content
// ========================================

const string PDFConfig::CLINIC_NAME = "SILVER CLINIC";
const string PDFConfig::CLINIC_SUBTITLE = "Professional Healthcare Management System";
const string PDFConfig::CONFIDENTIALITY_NOTICE = 
    "This document is confidential and protected by medical privacy laws. "
    "Unauthorized access, use, or disclosure is strictly prohibited.";
const string PDFConfig::SYSTEM_VERSION = "Silver Clinic Management System v1.0";

// ========================================
// Method Implementations
// ========================================

map<string, PDFConfig::ReportTemplate> PDFConfig::getReportTemplates() {
    map<string, ReportTemplate> templates;
    
    // Summary Report Template
    templates["summary"] = {
        "Summary Report",
        true,   // includeHeader
        true,   // includeClientInfo
        true,   // includeAssessorInfo
        false,  // includeTimeline
        false,  // includeFullNotes (only first 200 chars)
        true,   // includeFooter
        false   // includeWatermark
    };
    
    // Detailed Report Template
    templates["detailed"] = {
        "Detailed Case Report",
        true,   // includeHeader
        true,   // includeClientInfo
        true,   // includeAssessorInfo
        true,   // includeTimeline
        true,   // includeFullNotes
        true,   // includeFooter
        false   // includeWatermark
    };
    
    // Clinical Report Template
    templates["clinical"] = {
        "Clinical Assessment Report",
        true,   // includeHeader
        true,   // includeClientInfo
        true,   // includeAssessorInfo
        true,   // includeTimeline
        true,   // includeFullNotes
        true,   // includeFooter
        true    // includeWatermark
    };
    
    return templates;
}

map<string, string> PDFConfig::getStatusIcons() {
    map<string, string> icons;
    icons["Pending"] = "⏳";
    icons["Active"] = "🟢";
    icons["Closed"] = "🔴";
    icons["Cancelled"] = "❌";
    return icons;
}

map<string, PDFConfig::Color> PDFConfig::getStatusColors() {
    map<string, Color> colors;
    colors["Pending"] = STATUS_PENDING;
    colors["Active"] = STATUS_ACTIVE;
    colors["Closed"] = STATUS_CLOSED;
    colors["Cancelled"] = TEXT_SECONDARY;
    return colors;
}

string PDFConfig::colorToPDFFormat(const Color& color) {
    // Convert RGB float values to string format for PDF library
    return to_string(color.r) + " " + to_string(color.g) + " " + to_string(color.b);
}

float PDFConfig::getContentWidth() {
    return PAGE_WIDTH - MARGIN_LEFT - MARGIN_RIGHT;
}

float PDFConfig::getContentHeight() {
    return PAGE_HEIGHT - MARGIN_TOP - MARGIN_BOTTOM;
}

float PDFConfig::calculateNextY(float currentY, float sectionHeight) {
    return currentY - sectionHeight - SECTION_SPACING;
}

PDFConfig::ReportTemplate PDFConfig::getTemplate(const string& templateName) {
    auto templates = getReportTemplates();
    auto it = templates.find(templateName);
    
    if (it != templates.end()) {
        return it->second;
    }
    
    // Return detailed template as default
    return templates["detailed"];
}

string PDFConfig::formatDate(const string& isoDate, bool includeTime) {
    if (isoDate.empty()) {
        return "N/A";
    }
    
    // Basic implementation - can be enhanced with proper date parsing
    if (includeTime && isoDate.length() >= 19) {
        // Format: YYYY-MM-DD HH:MM:SS -> DD/MM/YYYY HH:MM
        string year = isoDate.substr(0, 4);
        string month = isoDate.substr(5, 2);
        string day = isoDate.substr(8, 2);
        string time = isoDate.substr(11, 5);
        
        return day + "/" + month + "/" + year + " " + time;
    } else if (isoDate.length() >= 10) {
        // Format: YYYY-MM-DD -> DD/MM/YYYY
        string year = isoDate.substr(0, 4);
        string month = isoDate.substr(5, 2);
        string day = isoDate.substr(8, 2);
        
        return day + "/" + month + "/" + year;
    }
    
    return isoDate; // Return as-is if format is unexpected
}

string PDFConfig::getLocalizedText(const string& key) {
    map<string, string> texts;
    
    // Portuguese localization
    texts["case_id"] = "ID do Caso";
    texts["client_name"] = "Nome do Cliente";
    texts["assessor_name"] = "Nome do Assessor";
    texts["status"] = "Status";
    texts["created_date"] = "Data de Criação";
    texts["closed_date"] = "Data de Fechamento";
    texts["duration"] = "Duração";
    texts["priority"] = "Prioridade";
    texts["case_information"] = "INFORMAÇÕES DO CASO";
    texts["client_information"] = "INFORMAÇÕES DO CLIENTE";
    texts["assessor_information"] = "INFORMAÇÕES DO ASSESSOR";
    texts["case_timeline"] = "CRONOLOGIA DO CASO";
    texts["case_notes"] = "ANOTAÇÕES DO CASO";
    texts["email"] = "E-mail";
    texts["phone"] = "Telefone";
    texts["date_of_birth"] = "Data de Nascimento";
    texts["generated"] = "Gerado em";
    texts["report_type"] = "Tipo de Relatório";
    texts["page"] = "Página";
    texts["of"] = "de";
    texts["days"] = "dias";
    
    auto it = texts.find(key);
    return (it != texts.end()) ? it->second : key;
}
