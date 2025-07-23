#!/bin/bash

# Test runner script for Silver Clinic Management System
echo "🚀 Silver Clinic Test Suite"
echo "============================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Build directory
BUILD_DIR="build_test"
PROJECT_ROOT="$(pwd)"

# Function to run a test
run_test() {
    local test_name="$1"
    local test_executable="$2"
    
    echo -e "\n${BLUE}🧪 Running $test_name${NC}"
    echo "$(printf '=%.0s' {1..50})"
    
    if [ -f "$test_executable" ]; then
        if "$test_executable"; then
            echo -e "${GREEN}✅ $test_name PASSED${NC}"
            return 0
        else
            echo -e "${RED}❌ $test_name FAILED${NC}"
            return 1
        fi
    else
        echo -e "${RED}❌ $test_name executable not found: $test_executable${NC}"
        return 1
    fi
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}❌ Error: Please run this script from the project root directory${NC}"
    exit 1
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}📁 Creating build directory...${NC}"
    mkdir -p "$BUILD_DIR"
fi

# Build the project
echo -e "${BLUE}🔨 Building project...${NC}"
cd "$BUILD_DIR"

if cmake -DCMAKE_BUILD_TYPE=Debug .. && make; then
    echo -e "${GREEN}✅ Build successful${NC}"
else
    echo -e "${RED}❌ Build failed${NC}"
    exit 1
fi

cd "$PROJECT_ROOT"

# Run tests
echo -e "\n${YELLOW}🧪 Starting test execution...${NC}"

total_tests=0
passed_tests=0

# Unit Tests
echo -e "\n${BLUE}📋 UNIT TESTS${NC}"
echo "==============="

if run_test "Core Classes Unit Tests" "$BUILD_DIR/test_core_classes"; then
    ((passed_tests++))
fi
((total_tests++))

if run_test "FamilyPhysician Unit Tests" "$BUILD_DIR/test_family_physician"; then
    ((passed_tests++))
fi
((total_tests++))

# Integration Tests
echo -e "\n${BLUE}🔗 INTEGRATION TESTS${NC}"
echo "======================"

if run_test "Database Integration Tests" "$BUILD_DIR/test_database"; then
    ((passed_tests++))
fi
((total_tests++))

# Individual Unit Tests
echo -e "\n${BLUE}🔬 INDIVIDUAL UNIT TESTS${NC}"
echo "========================="

# List of individual test executables (now built by CMake)
INDIVIDUAL_TESTS=(
    "test_datetime"
    "test_address" 
    "test_assessor"
    "test_utils"
    "test_emergency_contact"
)

# Run each individual unit test
for test in "${INDIVIDUAL_TESTS[@]}"; do
    if run_test "$test Individual Tests" "$BUILD_DIR/$test"; then
        ((passed_tests++))
    fi
    ((total_tests++))
done

# Summary
echo -e "\n${BLUE}📊 TEST SUMMARY${NC}"
echo "================"
echo "Total test suites: $total_tests"
echo -e "Passed: ${GREEN}$passed_tests ✅${NC}"
echo -e "Failed: ${RED}$((total_tests - passed_tests)) ❌${NC}"

success_rate=$(( passed_tests * 100 / total_tests ))
echo "Success rate: $success_rate%"

if [ $passed_tests -eq $total_tests ]; then
    echo -e "\n${GREEN}🎉 All test suites passed!${NC}"
    exit 0
else
    echo -e "\n${RED}⚠️  Some test suites failed!${NC}"
    exit 1
fi
