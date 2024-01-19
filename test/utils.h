#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <cstddef>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace test_utils {

bool compareVectors(const std::vector<uint>& lhs,
                    const std::vector<uint>& rhs) {
    if (lhs.size() != rhs.size()) {
        spdlog::info("Vectors are not equal: [{}] and [{}]",
                     fmt::join(lhs, ", "), fmt::join(rhs, ", "));
        return false;
    }
    auto lhs_it = lhs.cbegin();
    auto rhs_it = rhs.cbegin();
    while (lhs_it != lhs.cend()) {
        if (*lhs_it++ != *rhs_it++) {
            spdlog::info("Vectors are not equal: [{}] and [{}]",
                         fmt::join(lhs, ", "), fmt::join(rhs, ", "));
            return false;
        }
    }
    return true;
}

} // namespace test_utils

#endif // TEST_UTILS_H