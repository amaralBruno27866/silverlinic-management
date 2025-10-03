#include "db/DatabaseSchema.h"

namespace SilverClinic {
namespace db {

std::string DatabaseSchema::getAssessorTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS assessor(
            id INTEGER PRIMARY KEY,
            firstname TEXT NOT NULL,
            lastname TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            normalized_email TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL
        )
    )";
}

std::string DatabaseSchema::getClientTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS client(
            id INTEGER PRIMARY KEY,
            firstname TEXT NOT NULL,
            lastname TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            date_of_birth TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL
        )
    )";
}

std::string DatabaseSchema::getCaseProfileTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS case_profile(
            id INTEGER PRIMARY KEY,
            client_id INTEGER NOT NULL,
            assessor_id INTEGER NOT NULL,
            status TEXT NOT NULL DEFAULT 'Pending',
            notes TEXT CHECK(length(notes) <= 1500),
            created_at TEXT NOT NULL,
            closed_at TEXT,
            modified_at TEXT NOT NULL
        )
    )";
}

std::string DatabaseSchema::getAddressTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS address(
            id INTEGER PRIMARY KEY,
            user_key INTEGER NOT NULL,
            street TEXT,
            city TEXT,
            province TEXT,
            postal_code TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL
        )
    )";
}

std::string DatabaseSchema::getFamilyPhysicianTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS family_physician(
            id INTEGER PRIMARY KEY,
            type TEXT NOT NULL DEFAULT 'FPH',
            client_id INTEGER NOT NULL,
            full_name TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            address_id INTEGER,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (client_id) REFERENCES client(id),
            FOREIGN KEY (address_id) REFERENCES address(id)
        )
    )";
}

std::string DatabaseSchema::getEmergencyContactTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS emergency_contact(
            id INTEGER PRIMARY KEY,
            type TEXT NOT NULL DEFAULT 'ECT',
            client_id INTEGER NOT NULL,
            full_name TEXT NOT NULL,
            relationship TEXT,
            phone TEXT,
            alt_phone TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (client_id) REFERENCES client(id)
        )
    )";
}

std::string DatabaseSchema::getLegalRepresentativeTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS legal_representative(
            id INTEGER PRIMARY KEY,
            type TEXT NOT NULL DEFAULT 'LRT',
            client_id INTEGER NOT NULL,
            company_name TEXT NOT NULL,
            full_name TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (client_id) REFERENCES client(id)
        )
    )";
}

std::string DatabaseSchema::getInsuranceCompanyTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS insurance_company(
            id INTEGER PRIMARY KEY,
            type TEXT NOT NULL DEFAULT 'ICO',
            client_id INTEGER NOT NULL,
            company_name TEXT NOT NULL,
            adjuster_name TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            claim_number TEXT,
            policy_number TEXT,
            date_accident TEXT NOT NULL,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (client_id) REFERENCES client(id)
        )
    )";
}

std::string DatabaseSchema::getAutomobileAnxietyInventoryTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS automobile_anxiety_inventory(
            id INTEGER PRIMARY KEY,
            form_guid TEXT UNIQUE NOT NULL,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'AAI',
            question_1 BOOLEAN DEFAULT 0,
            question_2 BOOLEAN DEFAULT 0,
            question_3 BOOLEAN DEFAULT 0,
            question_4 BOOLEAN DEFAULT 0,
            question_5 BOOLEAN DEFAULT 0,
            question_6 BOOLEAN DEFAULT 0,
            question_7 BOOLEAN DEFAULT 0,
            question_8 BOOLEAN DEFAULT 0,
            question_9 BOOLEAN DEFAULT 0,
            question_10 BOOLEAN DEFAULT 0,
            question_11 BOOLEAN DEFAULT 0,
            question_12 BOOLEAN DEFAULT 0,
            question_13 BOOLEAN DEFAULT 0,
            question_14_driver BOOLEAN DEFAULT 0,
            question_14_passenger BOOLEAN DEFAULT 0,
            question_14_no_difference BOOLEAN DEFAULT 0,
            question_15_a BOOLEAN DEFAULT 0,
            question_15_b TEXT,
            question_16 BOOLEAN DEFAULT 0,
            question_17 BOOLEAN DEFAULT 0,
            question_18 BOOLEAN DEFAULT 0,
            question_19 BOOLEAN DEFAULT 0,
            question_19_sidewalks BOOLEAN DEFAULT 0,
            question_19_crossing BOOLEAN DEFAULT 0,
            question_19_both BOOLEAN DEFAULT 0,
            question_20 BOOLEAN DEFAULT 0,
            question_21 BOOLEAN DEFAULT 0,
            question_22 BOOLEAN DEFAULT 0,
            question_23 BOOLEAN DEFAULT 0,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
}

std::string DatabaseSchema::getBeckDepressionInventoryTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS beck_depression_inventory(
            id INTEGER PRIMARY KEY,
            form_guid TEXT UNIQUE NOT NULL,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'BDI',
            question_1 INTEGER DEFAULT 0 CHECK(question_1 >= 0 AND question_1 <= 3),
            question_2 INTEGER DEFAULT 0 CHECK(question_2 >= 0 AND question_2 <= 3),
            question_3 INTEGER DEFAULT 0 CHECK(question_3 >= 0 AND question_3 <= 3),
            question_4 INTEGER DEFAULT 0 CHECK(question_4 >= 0 AND question_4 <= 3),
            question_5 INTEGER DEFAULT 0 CHECK(question_5 >= 0 AND question_5 <= 3),
            question_6 INTEGER DEFAULT 0 CHECK(question_6 >= 0 AND question_6 <= 3),
            question_7 INTEGER DEFAULT 0 CHECK(question_7 >= 0 AND question_7 <= 3),
            question_8 INTEGER DEFAULT 0 CHECK(question_8 >= 0 AND question_8 <= 3),
            question_9 INTEGER DEFAULT 0 CHECK(question_9 >= 0 AND question_9 <= 3),
            question_10 INTEGER DEFAULT 0 CHECK(question_10 >= 0 AND question_10 <= 3),
            question_11 INTEGER DEFAULT 0 CHECK(question_11 >= 0 AND question_11 <= 3),
            question_12 INTEGER DEFAULT 0 CHECK(question_12 >= 0 AND question_12 <= 3),
            question_13 INTEGER DEFAULT 0 CHECK(question_13 >= 0 AND question_13 <= 3),
            question_14 INTEGER DEFAULT 0 CHECK(question_14 >= 0 AND question_14 <= 3),
            question_15 INTEGER DEFAULT 0 CHECK(question_15 >= 0 AND question_15 <= 3),
            question_16 INTEGER DEFAULT 0 CHECK(question_16 >= 0 AND question_16 <= 3),
            question_17 INTEGER DEFAULT 0 CHECK(question_17 >= 0 AND question_17 <= 3),
            question_18 INTEGER DEFAULT 0 CHECK(question_18 >= 0 AND question_18 <= 3),
            question_19 INTEGER DEFAULT 0 CHECK(question_19 >= 0 AND question_19 <= 3),
            question_20 INTEGER DEFAULT 0 CHECK(question_20 >= 0 AND question_20 <= 3),
            question_21 INTEGER DEFAULT 0 CHECK(question_21 >= 0 AND question_21 <= 3),
            total_score INTEGER DEFAULT 0,
            severity_level TEXT DEFAULT 'Minimal',
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
}

std::string DatabaseSchema::getBeckAnxietyInventoryTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS beck_anxiety_inventory(
            id INTEGER PRIMARY KEY,
            form_guid TEXT UNIQUE NOT NULL,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'BAI',
            question_1 INTEGER DEFAULT 0 CHECK(question_1 >= 0 AND question_1 <= 3),
            question_2 INTEGER DEFAULT 0 CHECK(question_2 >= 0 AND question_2 <= 3),
            question_3 INTEGER DEFAULT 0 CHECK(question_3 >= 0 AND question_3 <= 3),
            question_4 INTEGER DEFAULT 0 CHECK(question_4 >= 0 AND question_4 <= 3),
            question_5 INTEGER DEFAULT 0 CHECK(question_5 >= 0 AND question_5 <= 3),
            question_6 INTEGER DEFAULT 0 CHECK(question_6 >= 0 AND question_6 <= 3),
            question_7 INTEGER DEFAULT 0 CHECK(question_7 >= 0 AND question_7 <= 3),
            question_8 INTEGER DEFAULT 0 CHECK(question_8 >= 0 AND question_8 <= 3),
            question_9 INTEGER DEFAULT 0 CHECK(question_9 >= 0 AND question_9 <= 3),
            question_10 INTEGER DEFAULT 0 CHECK(question_10 >= 0 AND question_10 <= 3),
            question_11 INTEGER DEFAULT 0 CHECK(question_11 >= 0 AND question_11 <= 3),
            question_12 INTEGER DEFAULT 0 CHECK(question_12 >= 0 AND question_12 <= 3),
            question_13 INTEGER DEFAULT 0 CHECK(question_13 >= 0 AND question_13 <= 3),
            question_14 INTEGER DEFAULT 0 CHECK(question_14 >= 0 AND question_14 <= 3),
            question_15 INTEGER DEFAULT 0 CHECK(question_15 >= 0 AND question_15 <= 3),
            question_16 INTEGER DEFAULT 0 CHECK(question_16 >= 0 AND question_16 <= 3),
            question_17 INTEGER DEFAULT 0 CHECK(question_17 >= 0 AND question_17 <= 3),
            question_18 INTEGER DEFAULT 0 CHECK(question_18 >= 0 AND question_18 <= 3),
            question_19 INTEGER DEFAULT 0 CHECK(question_19 >= 0 AND question_19 <= 3),
            question_20 INTEGER DEFAULT 0 CHECK(question_20 >= 0 AND question_20 <= 3),
            question_21 INTEGER DEFAULT 0 CHECK(question_21 >= 0 AND question_21 <= 3),
            total_score INTEGER DEFAULT 0,
            severity_level TEXT DEFAULT 'Minimal',
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
}

std::string DatabaseSchema::getPainBodyMapTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS pain_body_map(
            id INTEGER PRIMARY KEY,
            form_guid TEXT UNIQUE NOT NULL,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'PBM',
            pain_data_json TEXT NOT NULL DEFAULT '{}',
            additional_comments TEXT CHECK(length(additional_comments) <= 2000),
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
}

std::string DatabaseSchema::getActivitiesOfDailyLivingTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS activities_of_daily_living(
            id INTEGER PRIMARY KEY,
            form_guid TEXT UNIQUE NOT NULL,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'ADL',
            activities_data_json TEXT NOT NULL DEFAULT '{}',
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
}

std::string DatabaseSchema::getSCL90RTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS scl90r(
            id INTEGER PRIMARY KEY,
            form_guid TEXT UNIQUE NOT NULL,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'SCL90R',
            question_1 INTEGER DEFAULT 0 CHECK(question_1 >= 0 AND question_1 <= 3),
            question_2 INTEGER DEFAULT 0 CHECK(question_2 >= 0 AND question_2 <= 3),
            question_3 INTEGER DEFAULT 0 CHECK(question_3 >= 0 AND question_3 <= 3),
            question_4 INTEGER DEFAULT 0 CHECK(question_4 >= 0 AND question_4 <= 3),
            question_5 INTEGER DEFAULT 0 CHECK(question_5 >= 0 AND question_5 <= 3),
            question_6 INTEGER DEFAULT 0 CHECK(question_6 >= 0 AND question_6 <= 3),
            question_7 INTEGER DEFAULT 0 CHECK(question_7 >= 0 AND question_7 <= 3),
            question_8 INTEGER DEFAULT 0 CHECK(question_8 >= 0 AND question_8 <= 3),
            question_9 INTEGER DEFAULT 0 CHECK(question_9 >= 0 AND question_9 <= 3),
            question_10 INTEGER DEFAULT 0 CHECK(question_10 >= 0 AND question_10 <= 3),
            question_11 INTEGER DEFAULT 0 CHECK(question_11 >= 0 AND question_11 <= 3),
            question_12 INTEGER DEFAULT 0 CHECK(question_12 >= 0 AND question_12 <= 3),
            question_13 INTEGER DEFAULT 0 CHECK(question_13 >= 0 AND question_13 <= 3),
            question_14 INTEGER DEFAULT 0 CHECK(question_14 >= 0 AND question_14 <= 3),
            question_15 INTEGER DEFAULT 0 CHECK(question_15 >= 0 AND question_15 <= 3),
            question_16 INTEGER DEFAULT 0 CHECK(question_16 >= 0 AND question_16 <= 3),
            question_17 INTEGER DEFAULT 0 CHECK(question_17 >= 0 AND question_17 <= 3),
            question_18 INTEGER DEFAULT 0 CHECK(question_18 >= 0 AND question_18 <= 3),
            question_19 INTEGER DEFAULT 0 CHECK(question_19 >= 0 AND question_19 <= 3),
            question_20 INTEGER DEFAULT 0 CHECK(question_20 >= 0 AND question_20 <= 3),
            question_21 INTEGER DEFAULT 0 CHECK(question_21 >= 0 AND question_21 <= 3),
            question_22 INTEGER DEFAULT 0 CHECK(question_22 >= 0 AND question_22 <= 3),
            question_23 INTEGER DEFAULT 0 CHECK(question_23 >= 0 AND question_23 <= 3),
            question_24 INTEGER DEFAULT 0 CHECK(question_24 >= 0 AND question_24 <= 3),
            question_25 INTEGER DEFAULT 0 CHECK(question_25 >= 0 AND question_25 <= 3),
            question_26 INTEGER DEFAULT 0 CHECK(question_26 >= 0 AND question_26 <= 3),
            question_27 INTEGER DEFAULT 0 CHECK(question_27 >= 0 AND question_27 <= 3),
            question_28 INTEGER DEFAULT 0 CHECK(question_28 >= 0 AND question_28 <= 3),
            question_29 INTEGER DEFAULT 0 CHECK(question_29 >= 0 AND question_29 <= 3),
            question_30 INTEGER DEFAULT 0 CHECK(question_30 >= 0 AND question_30 <= 3),
            question_31 INTEGER DEFAULT 0 CHECK(question_31 >= 0 AND question_31 <= 3),
            question_32 INTEGER DEFAULT 0 CHECK(question_32 >= 0 AND question_32 <= 3),
            question_33 INTEGER DEFAULT 0 CHECK(question_33 >= 0 AND question_33 <= 3),
            question_34 INTEGER DEFAULT 0 CHECK(question_34 >= 0 AND question_34 <= 3),
            question_35 INTEGER DEFAULT 0 CHECK(question_35 >= 0 AND question_35 <= 3),
            question_36 INTEGER DEFAULT 0 CHECK(question_36 >= 0 AND question_36 <= 3),
            question_37 INTEGER DEFAULT 0 CHECK(question_37 >= 0 AND question_37 <= 3),
            question_38 INTEGER DEFAULT 0 CHECK(question_38 >= 0 AND question_38 <= 3),
            question_39 INTEGER DEFAULT 0 CHECK(question_39 >= 0 AND question_39 <= 3),
            question_40 INTEGER DEFAULT 0 CHECK(question_40 >= 0 AND question_40 <= 3),
            question_41 INTEGER DEFAULT 0 CHECK(question_41 >= 0 AND question_41 <= 3),
            question_42 INTEGER DEFAULT 0 CHECK(question_42 >= 0 AND question_42 <= 3),
            question_43 INTEGER DEFAULT 0 CHECK(question_43 >= 0 AND question_43 <= 3),
            question_44 INTEGER DEFAULT 0 CHECK(question_44 >= 0 AND question_44 <= 3),
            question_45 INTEGER DEFAULT 0 CHECK(question_45 >= 0 AND question_45 <= 3),
            question_46 INTEGER DEFAULT 0 CHECK(question_46 >= 0 AND question_46 <= 3),
            question_47 INTEGER DEFAULT 0 CHECK(question_47 >= 0 AND question_47 <= 3),
            question_48 INTEGER DEFAULT 0 CHECK(question_48 >= 0 AND question_48 <= 3),
            question_49 INTEGER DEFAULT 0 CHECK(question_49 >= 0 AND question_49 <= 3),
            question_50 INTEGER DEFAULT 0 CHECK(question_50 >= 0 AND question_50 <= 3),
            question_51 INTEGER DEFAULT 0 CHECK(question_51 >= 0 AND question_51 <= 3),
            question_52 INTEGER DEFAULT 0 CHECK(question_52 >= 0 AND question_52 <= 3),
            question_53 INTEGER DEFAULT 0 CHECK(question_53 >= 0 AND question_53 <= 3),
            question_54 INTEGER DEFAULT 0 CHECK(question_54 >= 0 AND question_54 <= 3),
            question_55 INTEGER DEFAULT 0 CHECK(question_55 >= 0 AND question_55 <= 3),
            question_56 INTEGER DEFAULT 0 CHECK(question_56 >= 0 AND question_56 <= 3),
            question_57 INTEGER DEFAULT 0 CHECK(question_57 >= 0 AND question_57 <= 3),
            question_58 INTEGER DEFAULT 0 CHECK(question_58 >= 0 AND question_58 <= 3),
            question_59 INTEGER DEFAULT 0 CHECK(question_59 >= 0 AND question_59 <= 3),
            question_60 INTEGER DEFAULT 0 CHECK(question_60 >= 0 AND question_60 <= 3),
            question_61 INTEGER DEFAULT 0 CHECK(question_61 >= 0 AND question_61 <= 3),
            question_62 INTEGER DEFAULT 0 CHECK(question_62 >= 0 AND question_62 <= 3),
            question_63 INTEGER DEFAULT 0 CHECK(question_63 >= 0 AND question_63 <= 3),
            question_64 INTEGER DEFAULT 0 CHECK(question_64 >= 0 AND question_64 <= 3),
            question_65 INTEGER DEFAULT 0 CHECK(question_65 >= 0 AND question_65 <= 3),
            question_66 INTEGER DEFAULT 0 CHECK(question_66 >= 0 AND question_66 <= 3),
            question_67 INTEGER DEFAULT 0 CHECK(question_67 >= 0 AND question_67 <= 3),
            question_68 INTEGER DEFAULT 0 CHECK(question_68 >= 0 AND question_68 <= 3),
            question_69 INTEGER DEFAULT 0 CHECK(question_69 >= 0 AND question_69 <= 3),
            question_70 INTEGER DEFAULT 0 CHECK(question_70 >= 0 AND question_70 <= 3),
            question_71 INTEGER DEFAULT 0 CHECK(question_71 >= 0 AND question_71 <= 3),
            question_72 INTEGER DEFAULT 0 CHECK(question_72 >= 0 AND question_72 <= 3),
            question_73 INTEGER DEFAULT 0 CHECK(question_73 >= 0 AND question_73 <= 3),
            question_74 INTEGER DEFAULT 0 CHECK(question_74 >= 0 AND question_74 <= 3),
            question_75 INTEGER DEFAULT 0 CHECK(question_75 >= 0 AND question_75 <= 3),
            question_76 INTEGER DEFAULT 0 CHECK(question_76 >= 0 AND question_76 <= 3),
            question_77 INTEGER DEFAULT 0 CHECK(question_77 >= 0 AND question_77 <= 3),
            question_78 INTEGER DEFAULT 0 CHECK(question_78 >= 0 AND question_78 <= 3),
            question_79 INTEGER DEFAULT 0 CHECK(question_79 >= 0 AND question_79 <= 3),
            question_80 INTEGER DEFAULT 0 CHECK(question_80 >= 0 AND question_80 <= 3),
            question_81 INTEGER DEFAULT 0 CHECK(question_81 >= 0 AND question_81 <= 3),
            question_82 INTEGER DEFAULT 0 CHECK(question_82 >= 0 AND question_82 <= 3),
            question_83 INTEGER DEFAULT 0 CHECK(question_83 >= 0 AND question_83 <= 3),
            question_84 INTEGER DEFAULT 0 CHECK(question_84 >= 0 AND question_84 <= 3),
            question_85 INTEGER DEFAULT 0 CHECK(question_85 >= 0 AND question_85 <= 3),
            question_86 INTEGER DEFAULT 0 CHECK(question_86 >= 0 AND question_86 <= 3),
            question_87 INTEGER DEFAULT 0 CHECK(question_87 >= 0 AND question_87 <= 3),
            question_88 INTEGER DEFAULT 0 CHECK(question_88 >= 0 AND question_88 <= 3),
            question_89 INTEGER DEFAULT 0 CHECK(question_89 >= 0 AND question_89 <= 3),
            question_90 INTEGER DEFAULT 0 CHECK(question_90 >= 0 AND question_90 <= 3),
            gsi INTEGER DEFAULT 0,
            pst INTEGER DEFAULT 0,
            psdi REAL DEFAULT 0.0,
            severity_level TEXT DEFAULT 'Minimal',
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
}

std::string DatabaseSchema::getFormGuidsTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS form_guids (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            guid TEXT UNIQUE NOT NULL,
            case_profile_id INTEGER NOT NULL,
            form_key TEXT NOT NULL,
            created_at TEXT NOT NULL DEFAULT (datetime('now'))
        )
    )";
}

std::string DatabaseSchema::getAssessorEmailIndexSQL() {
    return R"(
        CREATE UNIQUE INDEX IF NOT EXISTS idx_assessor_normalized_email_unique ON assessor(normalized_email) WHERE normalized_email IS NOT NULL AND normalized_email <> ''
    )";
}

std::string DatabaseSchema::getAssessorNamePhoneIndexSQL() {
    return R"(
        CREATE UNIQUE INDEX IF NOT EXISTS idx_assessor_name_phone_unique ON assessor(firstname, lastname, phone)
    )";
}

std::string DatabaseSchema::getPopulateNormalizedEmailSQL() {
    return R"(
        UPDATE assessor SET normalized_email = lower(trim(email)) WHERE email IS NOT NULL AND trim(email) <> '';
    )";
}

std::string DatabaseSchema::getSchemaVersionTableSQL() {
    return R"(
        CREATE TABLE IF NOT EXISTS schema_version(
            id INTEGER PRIMARY KEY,
            version INTEGER NOT NULL,
            applied_at TEXT NOT NULL DEFAULT (datetime('now'))
        )
    )";
}

std::vector<std::pair<std::string, std::string>> DatabaseSchema::getAllTableDefinitions() {
    return {
        {"Schema Version", getSchemaVersionTableSQL()},
        {"Assessor", getAssessorTableSQL()},
        {"Client", getClientTableSQL()},
        {"Case Profile", getCaseProfileTableSQL()},
        {"Address", getAddressTableSQL()},
        {"Family Physician", getFamilyPhysicianTableSQL()},
        {"Emergency Contact", getEmergencyContactTableSQL()},
        {"Legal Representative", getLegalRepresentativeTableSQL()},
        {"Insurance Company", getInsuranceCompanyTableSQL()},
        {"Automobile Anxiety Inventory", getAutomobileAnxietyInventoryTableSQL()},
        {"Beck Depression Inventory", getBeckDepressionInventoryTableSQL()},
        {"Beck Anxiety Inventory", getBeckAnxietyInventoryTableSQL()},
        {"Pain Body Map", getPainBodyMapTableSQL()},
        {"Activities of Daily Living", getActivitiesOfDailyLivingTableSQL()},
        {"SCL90R", getSCL90RTableSQL()},
        {"Form GUIDs", getFormGuidsTableSQL()}
    };
}

std::vector<std::pair<std::string, std::string>> DatabaseSchema::getAllIndexDefinitions() {
    return {
        {"Assessor Email Index", getAssessorEmailIndexSQL()},
        {"Populate Normalized Email", getPopulateNormalizedEmailSQL()},
        {"Assessor Name+Phone Index", getAssessorNamePhoneIndexSQL()}
    };
}

int DatabaseSchema::getCurrentSchemaVersion() {
    return 1; // Version 1: Initial centralized schema
}

} // namespace db
} // namespace SilverClinic