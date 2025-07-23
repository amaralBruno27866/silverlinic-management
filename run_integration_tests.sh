#!/bin/bash

echo "🧪 Running Integration Tests"
echo "==========================="

cd "$(dirname "$0")"

# Array of integration tests
integration_tests=(
    "test_datetime_integration:tests/integration/test_datetime_integration.cpp:src/forms/InsuranceCompany.cpp src/core/DateTime.cpp src/core/Utils.cpp"
    # Add more integration tests here as needed
)

total_tests=0
passed_tests=0

for test_config in "${integration_tests[@]}"; do
    IFS=':' read -r test_name test_file dependencies <<< "$test_config"
    
    echo ""
    echo "🧪 Running $test_name..."
    echo "========================"
    
    # Compile the test
    if g++ -I./include $test_file $dependencies -o temp_$test_name -std=c++17; then
        # Run the test
        if ./temp_$test_name; then
            echo "✅ $test_name PASSED"
            ((passed_tests++))
        else
            echo "❌ $test_name FAILED"
        fi
        # Clean up
        rm -f temp_$test_name
    else
        echo "❌ $test_name COMPILATION FAILED"
    fi
    
    ((total_tests++))
done

echo ""
echo "📊 Integration Test Summary"
echo "=========================="
echo "Total tests: $total_tests"
echo "Passed: $passed_tests ✅"
echo "Failed: $((total_tests - passed_tests)) ❌"

if [ $passed_tests -eq $total_tests ]; then
    echo "🎉 All integration tests passed!"
    exit 0
else
    echo "⚠️ Some integration tests failed!"
    exit 1
fi
