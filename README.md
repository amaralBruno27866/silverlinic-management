# SilverClinic Management System

A comprehensive healthcare management system developed in C++ for mental health clinics, featuring client management, assessor tracking, case profiles, and form processing with Canadian healthcare compliance.

## 🏥 Features

### Core System
- **Client Management**: Complete patient information with Canadian data validation
- **Assessor Management**: Mental health professionals and support staff tracking
- **Case Profiles**: Connect clients with assessors, status management (Pending → Active → Closed)
- **Address Management**: Canadian postal code validation and formatting
- **DateTime Utilities**: Advanced date/time handling with age calculations

### Form Processing
- **Family Physician**: Medical professional information and contact details
- **Emergency Contacts**: Emergency contact information with validation
- **Legal Representatives**: Legal representative data management
- **Insurance Information**: Health insurance details and coverage

### Data Validation
- **Canadian Standards**: Postal codes, phone numbers, SIN validation
- **Email Validation**: RFC-compliant email format checking
- **Character Limits**: Configurable limits with automatic truncation and logging
- **ID Management**: Systematic ID generation with prefixes for different entity types

## 🛠️ Technology Stack

- **Language**: C++17
- **Database**: SQLite3
- **Build System**: CMake
- **Platform**: Cross-platform (Linux, Windows)
- **Standards**: Canadian healthcare data compliance

## 📁 Project Structure

```
slt_cpp_programn/
├── include/                  # Header files
│   ├── [core classes]       # Core system entities
│   └── forms/               # Form processing classes
├── src/                     # Implementation files
│   ├── [core classes]      # Core system implementations
│   ├── forms/              # Form implementations
│   └── main.cpp            # Application entry point
├── forms_html/             # HTML templates for reference
├── docs/                   # Documentation
└── build/                  # Build artifacts (generated)
```

## 🚀 Quick Start

### Prerequisites
- GCC 13.3+ or compatible C++17 compiler
- CMake 3.16+
- SQLite3 development libraries

### Installation

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake libsqlite3-dev
```

#### Build and Run
```bash
# Clone the repository
git clone git@github.com:amaralBruno27866/silverlinic-management.git
cd silverlinic-management

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the application
./SilverClinic
```

## 📊 Database Schema

### Core Tables
- **assessor**: Mental health professionals
- **client**: Patient information
- **address**: Location data with Canadian validation
- **case_profile**: Client-assessor relationships

### ID System
- Assessor: 100000+
- Address: 200000+
- Client: 300000+
- CaseProfile: 400000+
- FamilyPhysician: 500000+

## 🧪 Testing

The system includes comprehensive tests for:
- DateTime calculations and age computation
- Canadian data validation (postal codes, phone numbers)
- Case profile status management
- Form data validation and character limits
- Database integrity and relationships

## 🧪 Clinical Instruments (Excerpt)

### Beck Anxiety Inventory (BAI)
Updated interpretation thresholds (August 2025 refactor):
- Minimal: 0–7
- Mild: 8–15
- Moderate: 16–25
- Severe: 26–63

High-risk operational flag (system logic): total score >= 16 (moderate or higher).

CSV export (frontend) columns now include: Total_Score, Severity_Level.

Notes:
- Item labels in C++ class and HTML are synchronized.
- Legacy depression-oriented comments were removed.
- Sub-score grouping (cognitive/affective/somatic) in code remains legacy for backward compatibility; may be clinically refined later.

## 📝 Usage Examples

### Creating a Client
```cpp
Client client("João Santos", "joao@email.com", "(416) 555-0123");
client.setDateOfBirth(DateTime(1985, 3, 15));
```

### Managing Case Profiles
```cpp
CaseProfile caseProfile(client.getClientId(), assessor.getAssessorId());
caseProfile.activate();  // Pending → Active
caseProfile.setNotes("Treatment progress notes...");
```

### Form Processing
```cpp
SilverClinic::Forms::FamilyPhysician familyDoc(
    clientId, "Dr. Maria Silva", "Toronto Medical Center"
);
familyDoc.setPostalCode("M5G1X5");  // Auto-formatted to "M5G 1X5"
```

## 🔧 Configuration

### Character Limits
- Case Profile Notes: 1,500 characters
- Family Physician Notes: 500 characters
- Doctor Name: 100 characters
- Clinic Name: 150 characters

### Canadian Validation
- Postal Code: K1A 0A6 format
- Phone: (416) 555-0123 format
- SIN: 123-456-789 format

## 📈 Development Status

### ✅ Completed
- Core entity management system
- SQLite3 database integration
- Canadian data validation utilities
- Case profile workflow management
- Form processing framework
- Character limit validation

### 🔄 In Progress
- Additional form entities (Emergency Contact, Legal Representative)
- Web interface development
- Reporting and analytics
- Appointment scheduling

### 📋 Planned
- Multi-language support
- Advanced reporting dashboard
- Integration with external systems
- Mobile application

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Author

**Bruno Amaral**
- GitHub: [@amaralBruno27866](https://github.com/amaralBruno27866)
- Email: bamaral2.git2023@gmail.com

## 🙏 Acknowledgments

- Canadian healthcare standards compliance
- Modern C++ best practices
- SQLite3 for robust data management
- CMake for cross-platform building

---

*SilverClinic Management System - Empowering healthcare professionals with efficient client management tools.*
