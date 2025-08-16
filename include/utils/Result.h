// Simple Result struct to standardize operation outcomes for services / GUI layer
#ifndef SILVERCLINIC_RESULT_H
#define SILVERCLINIC_RESULT_H

#include <string>
#include <utility>

namespace SilverClinic {

struct Result {
    bool success {false};
    std::string code;      // machine-friendly code (e.g. "ok", "validation.invalid_client_id")
    std::string message;   // human-friendly (can be localized in UI layer)

    static Result Ok(std::string msg = "") { return {true, "ok", std::move(msg)}; }
    static Result Error(std::string c, std::string msg) { return {false, std::move(c), std::move(msg)}; }
};

}

#endif
