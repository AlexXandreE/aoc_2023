
#include <sys/syslimits.h>

#include <algorithm>
#include <cctype>
#include <charconv>
#include <fstream>
#include <future>
#include <iostream>
#include <limits>
#include <map>
#include <mutex>
#include <numeric>
#include <ranges>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

const std::vector<std::string_view> kTestCase1{"Time:      7  15   30", "Distance:  9  40  200"};

std::vector<std::int64_t> extract_numbers(std::string_view input) noexcept
{
    std::vector<std::int64_t> numbers;

    for (std::size_t i = 0; i < input.size(); ++i) {
        if (std::isdigit(input[i]) == 0) continue;

        auto number = input.substr(i);
        std::int64_t value{0};
        auto [ptr, ec] = std::from_chars(number.data(), number.data() + number.size(), value);

        i += (ptr - number.data()) - 1;

        numbers.push_back(value);
    }

    return numbers;
}

auto solve(const auto& times, const auto& distances) -> std::int64_t
{
    std::int64_t final{1};
    for (std::int64_t i = 0; i < std::ssize(times); ++i) {
        auto max_time = times[i];
        auto record = distances[i];

        std::int64_t times_beat{0};
        for (std::int64_t j = 1; j < max_time; ++j) {
            auto start_time = j;
            auto timehold = max_time - start_time;
            auto speed = start_time * timehold;

            if (speed > record) times_beat++;
        }
        final *= times_beat;
    }

    return final;
}

int main(int argc, char** argv)
{
    auto times = extract_numbers(kTestCase1.front().substr(5));
    auto distances = extract_numbers(kTestCase1.back().substr(9));

    std::cout << "Testcase 1: " << solve(times, distances) << "\n";

    if (argc == 1) return 0;

    auto file = std::ifstream(std::string{argv[1]}, std::ios::in);
    if (!file.is_open()) {
        std::cout << "Error openning file\n";
        return 1;
    }

    std::string time_str;
    std::getline(file, time_str);
    std::string distance_str;
    std::getline(file, distance_str);

    {  // part 1
        times = extract_numbers(time_str.substr(5));
        distances = extract_numbers(distance_str.substr(9));

        std::cout << "part 1: " << solve(times, distances) << "\n";
    }

    times.clear();
    distances.clear();

    time_str.erase(remove(time_str.begin(), time_str.end(), ' '), time_str.end());
    distance_str.erase(remove(distance_str.begin(), distance_str.end(), ' '), distance_str.end());

    times = extract_numbers(time_str);
    distances = extract_numbers(distance_str);

    std::cout << "part 2: " << solve(times, distances) << "\n";

    return 0;
}

