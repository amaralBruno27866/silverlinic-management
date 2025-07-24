#ifndef PDF_CONFIG_H
#define PDF_CONFIG_H

#include <string>
#include <map>

using namespace std;

namespace SilverClinic {
    
    /**
     * @brief Configuration class for PDF generation settings
     * 
     * This class contains all the styling, layout, and formatting
     * configurations for generating professional PDF reports.
     */
    class PDFConfig {
    public:
        // ========================================
        // Color Definitions (RGB values)
        // ========================================
        
        struct Color {
            float r, g, b;
            Color() : r(0.0f), g(0.0f), b(0.0f) {}
            Color(float red, float green, float blue) : r(red), g(green), b(blue) {}
        };
        
        static const Color HEADER_BLUE;          // #1e3a8a
        static const Color SECTION_GRAY;         // #f8fafc
        static const Color TEXT_BLACK;           // #000000
        static const Color TEXT_SECONDARY;       // #64748b
        static const Color STATUS_ACTIVE;        // #10b981
        static const Color STATUS_CLOSED;        // #dc2626
        static const Color STATUS_PENDING;       // #f59e0b
        static const Color BORDER_LIGHT;         // #e2e8f0
        
        // ========================================
        // Typography Settings
        // ========================================
        
        static const float TITLE_FONT_SIZE;      // 16pt
        static const float SUBTITLE_FONT_SIZE;   // 12pt
        static const float TEXT_FONT_SIZE;       // 10pt
        static const float NOTES_FONT_SIZE;      // 9pt
        static const float FOOTER_FONT_SIZE;     // 8pt
        
        // ========================================
        // Layout Dimensions (in points)
        // ========================================
        
        static const float PAGE_WIDTH;           // 595 (A4)
        static const float PAGE_HEIGHT;          // 842 (A4)
        static const float MARGIN_LEFT;          // 57 (2cm)
        static const float MARGIN_RIGHT;         // 57 (2cm)
        static const float MARGIN_TOP;           // 57 (2cm)
        static const float MARGIN_BOTTOM;        // 57 (2cm)
        
        static const float SECTION_PADDING;      // 10
        static const float LINE_SPACING;         // 14
        static const float SECTION_SPACING;      // 20
        
        // ========================================
        // Report Templates
        // ========================================
        
        enum class ReportType {
            SUMMARY,
            DETAILED,
            CLINICAL
        };
        
        struct ReportTemplate {
            string name;
            bool includeHeader;
            bool includeClientInfo;
            bool includeAssessorInfo;
            bool includeTimeline;
            bool includeFullNotes;
            bool includeFooter;
            bool includeWatermark;
        };
        
        static map<string, ReportTemplate> getReportTemplates();
        
        // ========================================
        // Text Content Templates
        // ========================================
        
        static const string CLINIC_NAME;
        static const string CLINIC_SUBTITLE;
        static const string CONFIDENTIALITY_NOTICE;
        static const string SYSTEM_VERSION;
        
        // ========================================
        // Status Icons and Labels
        // ========================================
        
        static map<string, string> getStatusIcons();
        static map<string, Color> getStatusColors();
        
        // ========================================
        // Utility Methods
        // ========================================
        
        /**
         * @brief Convert RGB values to PDF color format
         * @param color Color struct with RGB values
         * @return Formatted color string for PDF library
         */
        static string colorToPDFFormat(const Color& color);
        
        /**
         * @brief Get content area width (page width - margins)
         * @return Available width for content
         */
        static float getContentWidth();
        
        /**
         * @brief Get content area height (page height - margins)
         * @return Available height for content
         */
        static float getContentHeight();
        
        /**
         * @brief Calculate Y position for next section
         * @param currentY Current Y position
         * @param sectionHeight Height of the section to add
         * @return New Y position
         */
        static float calculateNextY(float currentY, float sectionHeight);
        
        /**
         * @brief Get template configuration by name
         * @param templateName Name of the template ("summary", "detailed", "clinical")
         * @return ReportTemplate configuration
         */
        static ReportTemplate getTemplate(const string& templateName);
        
        /**
         * @brief Format date for display in PDF
         * @param isoDate Date in ISO format (YYYY-MM-DD)
         * @param includeTime Whether to include time component
         * @return Formatted date string
         */
        static string formatDate(const string& isoDate, bool includeTime = false);
        
        /**
         * @brief Get localized text for PDF generation
         * @param key Text key identifier
         * @return Localized text string
         */
        static string getLocalizedText(const string& key);
        
    private:
        // Static initialization helpers
        static void initializeColors();
        static void initializeTemplates();
        static void initializeStatusMaps();
    };
    
} // namespace SilverClinic

#endif // PDF_CONFIG_H
