
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
#include <string_view>
#include <unordered_map>
#include <vector>

const std::vector<std::string_view> kTestCase1{
    "seeds: 79 14 55 13",
    "",
    "seed-to-soil map:",
    "50 98 2",
    "52 50 48",
    "",
    "soil-to-fertilizer map:",
    "0 15 37",
    "37 52 2",
    "39 0 15",
    "",
    "fertilizer-to-water map:",
    "49 53 8",
    "0 11 42",
    "42 0 7",
    "57 7 4",
    "",
    "water-to-light map:",
    "88 18 7",
    "18 25 70",
    "",
    "light-to-temperature map:",
    "45 77 23",
    "81 45 19",
    "68 64 13",
    "",
    "temperature-to-humidity map:",
    "0 69 1",
    "1 0 69",
    "",
    "humidity-to-location map:",
    "60 56 37",
    "56 93 4",
};

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

auto lowest_location(std::int64_t seed, const std::array<std::map<std::int64_t, std::int64_t>*, 8>& states)
{
    std::int64_t map{1};
    auto next_value = seed;

    while (map < std::ssize(states)) {
        const auto* current_state = states[map];
        auto lower_bound = current_state->lower_bound(next_value);

        auto next_mapping = std::ranges::find_if(*current_state, [&](const auto& entry) {
            return entry.first <= next_value && lower_bound->second == entry.second;
        });

        if (next_mapping == current_state->end()) {
            map++;
            continue;
        }

        auto offset = next_value - next_mapping->first;
        next_value = next_mapping->second + offset;
        map++;
    }

    return next_value;
}

int main(int argc, char** argv)
{
    auto seeds = extract_numbers(kTestCase1.front().substr(7));

    std::map<std::int64_t, std::int64_t> seed_to_soil{
        {std::numeric_limits<std::int64_t>::max(), std::numeric_limits<std::int64_t>::max()}};
    std::map<std::int64_t, std::int64_t> soil_to_fertilizer{
        {std::numeric_limits<std::int64_t>::max(), std::numeric_limits<std::int64_t>::max()}};
    std::map<std::int64_t, std::int64_t> fertilizer_to_water{
        {std::numeric_limits<std::int64_t>::max(), std::numeric_limits<std::int64_t>::max()}};
    std::map<std::int64_t, std::int64_t> water_to_light{
        {std::numeric_limits<std::int64_t>::max(), std::numeric_limits<std::int64_t>::max()}};
    std::map<std::int64_t, std::int64_t> light_to_temperature{
        {std::numeric_limits<std::int64_t>::max(), std::numeric_limits<std::int64_t>::max()}};
    std::map<std::int64_t, std::int64_t> temperature_to_humidity{
        {std::numeric_limits<std::int64_t>::max(), std::numeric_limits<std::int64_t>::max()}};
    std::map<std::int64_t, std::int64_t> humidity_to_location{
        {std::numeric_limits<std::int64_t>::max(), std::numeric_limits<std::int64_t>::max()}};

    std::array<std::map<std::int64_t, std::int64_t>*, 8> states{
        nullptr,
        &seed_to_soil,
        &soil_to_fertilizer,
        &fertilizer_to_water,
        &water_to_light,
        &light_to_temperature,
        &temperature_to_humidity,
        &humidity_to_location};

    {
        std::int64_t state{0};
        for (std::size_t i = 1; i < kTestCase1.size(); ++i) {
            auto line = kTestCase1[i];
            if (line.empty()) continue;

            if (std::isdigit(line[0]) == 0) {
                state++;
                continue;
            }

            auto values = extract_numbers(line);

            auto& map = *states[state];

            std::int64_t destination = values[0];
            std::int64_t source = values[1];
            std::int64_t count = values[2];

            map[source] = destination;
            map[source + count - 1] = destination;
        }
    }

    {  // testcase p1
        std::vector<std::int64_t> locations;
        for (auto seed : seeds) {
            locations.push_back(lowest_location(seed, states));
        }

        std::cout << "minimum location(testcase p1): " << *std::ranges::min_element(locations) << "\n";
    }

    {  // testcase p2
        std::int64_t min_location{std::numeric_limits<std::int64_t>::max()};

        for (std::size_t i = 0; i < seeds.size(); i += 2) {
            auto start = seeds[i];
            auto end = start + seeds[i + 1];
            for (std::int64_t seed = start; seed < end; ++seed) {
                min_location = std::min(min_location, lowest_location(seed, states));
            }
        }

        std::cout << "minimum location(testcase p2): " << min_location << "\n";
    }

    if (argc == 1) return 0;

    for (auto* state : states | std::views::drop(1)) {
        state->clear();
    }

    auto file = std::ifstream(std::string{argv[1]}, std::ios::in);
    if (!file.is_open()) {
        std::cout << "Error openning file\n";
        return 1;
    }

    std::string line;
    std::int64_t index = 0;
    std::int64_t state = 0;

    std::getline(file, line);
    seeds = extract_numbers(line.substr(7));

    while (true) {
        std::getline(file, line);
        if (file.eof()) break;
        if (line.empty()) continue;

        if (std::isdigit(line[0]) == 0) {
            state++;
            continue;
        }

        auto values = extract_numbers(line);

        auto& map = *states[state];

        std::int64_t destination = values[0];
        std::int64_t source = values[1];
        std::int64_t count = values[2];

        map[source] = destination;
        map[source + count - 1] = destination;

        index++;
    }

    {  // part 1
        std::int64_t min_location{std::numeric_limits<std::int64_t>::max()};
        for (auto seed : seeds) {
            min_location = std::min(min_location, lowest_location(seed, states));
        }

        std::cout << "minimum location(p1): " << min_location << "\n";
    }

    {  // part 2
        constexpr std::size_t worker_count{5};
        std::vector<std::vector<std::pair<std::int64_t, std::int64_t>>> seed_pairs(worker_count);
        std::vector<std::pair<std::int64_t, std::int64_t>> pending_seeds;
        for (std::size_t i = 0; i < seeds.size(); i += 2) {
            pending_seeds.emplace_back(seeds[i], seeds[i + 1]);
        }

        // init seeds

        std::mutex mutex;
        std::vector<std::int64_t> outputs;
        std::vector<std::future<void>> workers;

        auto job = [&states, &outputs, &mutex, &pending_seeds]() {
            std::pair<std::int64_t, std::int64_t> data;

            while (true) {
                {
                    std::scoped_lock lock{mutex};
                    if (pending_seeds.empty()) return;

                    data = pending_seeds.back();
                    pending_seeds.pop_back();
                }

                std::int64_t min_location{std::numeric_limits<std::int64_t>::max()};
                auto start = data.first;
                auto end = start + data.second;
                for (std::int64_t seed = start; seed < end; ++seed) {
                    min_location = std::min(min_location, lowest_location(seed, states));
                }

                std::scoped_lock lock{mutex};
                outputs.push_back(min_location);
            }
        };

        for (std::size_t i = 0; i < worker_count; ++i) {
            workers.emplace_back(std::async(std::launch::async, job));
        }

        for (auto& worker : workers) {
            if (worker.valid()) {
                worker.wait();
            }
        }

        /* serial
        std::int64_t min_location{std::numeric_limits<std::int64_t>::max()};

        for (std::size_t i = 0; i < seeds.size(); i += 2) {
            auto start = seeds[i];
            auto end = start + seeds[i + 1];
            for (std::int64_t seed = start; seed < end; ++seed) {
                min_location = std::min(min_location, lowest_location(seed, states));
            }
        }
        std::cout << "minimum location(p2): " << min_location << "\n";
        */

        std::cout << "minimum location(p2): " << *std::ranges::min_element(outputs) << "\n";
    }

    return 0;
}

