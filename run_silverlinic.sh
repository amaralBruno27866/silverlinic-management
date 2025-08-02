#!/bin/bash
# 🏥 Silver Clinic Management System - Safe Launcher
# This script ensures the application runs from the correct directory

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🏥 Silver Clinic Management System - Launcher${NC}"
echo "=================================================="

# Find the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"

echo -e "${YELLOW}📂 Script location: ${SCRIPT_DIR}${NC}"

# Check if we're in the project root
if [[ ! -f "$PROJECT_ROOT/CMakeLists.txt" ]] || [[ ! -d "$PROJECT_ROOT/data" ]] || [[ ! -d "$PROJECT_ROOT/src" ]]; then
    echo -e "${RED}❌ ERROR: Script must be placed in project root directory!${NC}"
    echo -e "${RED}   Expected: CMakeLists.txt, data/, src/ directories${NC}"
    exit 1
fi

# Check if executable exists
EXECUTABLE="$PROJECT_ROOT/build_test/SilverClinic"
if [[ ! -f "$EXECUTABLE" ]]; then
    echo -e "${RED}❌ ERROR: SilverClinic executable not found!${NC}"
    echo -e "${YELLOW}   Expected location: $EXECUTABLE${NC}"
    echo -e "${YELLOW}   Please compile the project first:${NC}"
    echo -e "${YELLOW}   cd build_test && make SilverClinic -j4${NC}"
    exit 1
fi

# Change to project root directory
cd "$PROJECT_ROOT"
echo -e "${GREEN}✅ Working directory set to: $(pwd)${NC}"

# Check executable permissions
if [[ ! -x "$EXECUTABLE" ]]; then
    echo -e "${YELLOW}⚠️  Making executable runnable...${NC}"
    chmod +x "$EXECUTABLE"
fi

# Run the application
echo -e "${GREEN}🚀 Launching Silver Clinic Management System...${NC}"
echo "=================================================="

# Execute with error handling
if "$EXECUTABLE"; then
    echo "=================================================="
    echo -e "${GREEN}✅ Application completed successfully${NC}"
else
    exit_code=$?
    echo "=================================================="
    echo -e "${RED}❌ Application exited with error code: $exit_code${NC}"
    exit $exit_code
fi
