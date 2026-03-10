#pragma once

#include <cctype>
#include <string>
#include <utility>

namespace am::str {

// Trim both ends; utility is intentionally copy-based for call-site simplicity.
inline std::string trim_copy(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    return s;
}

// Uppercase copy for case-insensitive keys/symbols normalization.
inline std::string upper_copy(std::string s) {
    for (char& ch : s) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return s;
}

// Common helper used by config/CSV ingestion where both operations are needed.
inline std::string upper_trim_copy(std::string s) {
    return upper_copy(trim_copy(std::move(s)));
}

} // namespace am::str
