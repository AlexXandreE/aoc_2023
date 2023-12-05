
#include <sys/syslimits.h>

#include <algorithm>
#include <cctype>
#include <charconv>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <set>
#include <string_view>
#include <unordered_map>
#include <vector>

constexpr std::array<std::string_view, 6> kTestCase1{
    "Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53",
    "Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19",
    "Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1",
    "Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83",
    "Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36",
    "Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11",
};

struct Scratchcard
{
    std::vector<int> winning_numbers;
    std::vector<int> lottery_numbers;

    std::size_t match_count{0};  // cache for part2
    std::size_t sum{1};
};

std::vector<int> extract_numbers(std::string_view input) noexcept
{
    std::vector<int> numbers;

    for (std::size_t i = 0; i < input.size(); ++i) {
        if (std::isdigit(input[i]) == 0) continue;

        auto number = input.substr(i);
        int value{0};
        auto [ptr, ec] = std::from_chars(number.data(), number.data() + number.size(), value);

        i += (ptr - number.data()) - 1;

        numbers.push_back(value);
    }

    return numbers;
}

Scratchcard parse(std::string_view input) noexcept
{
    auto id_separator = input.find(':');
    auto section_separator = input.substr(id_separator).find('|');

    Scratchcard card{
        .winning_numbers = extract_numbers(input.substr(id_separator + 1, section_separator - 1)),
        .lottery_numbers = extract_numbers(input.substr(id_separator + section_separator + 1))};

    return card;
}

void process_part1(auto& cards)
{
    auto winnings = std::accumulate(cards.begin(), cards.end(), 0, [](int sum, auto& card) {
        int found = std::ranges::count_if(card.lottery_numbers, [&card](auto number) {
            return std::ranges::find(card.winning_numbers, number) != card.winning_numbers.end();
        });

        card.match_count = found;  // part 2

        return sum + (found > 2 ? std::pow(2, found - 1) : found);
    });

    std::cout << "Part 1: " << winnings << "\n";
}

void recursive_add(std::size_t start, const std::vector<Scratchcard>& cards, std::vector<Scratchcard>& flat) noexcept
{
    const auto& card = cards[start];

    flat.push_back(card);

    for (std::size_t j = 1; j <= card.match_count; ++j) {
        recursive_add(start + j, cards, flat);
    }
}

// brute force
void process_part2_brute_force(std::vector<Scratchcard>& cards)
{
    std::vector<Scratchcard> flat;
    flat.reserve(cards.size());

    for (std::size_t i = 0; i < cards.size(); ++i) {
        recursive_add(i, cards, flat);
    }

    std::cout << "Part 2 (brute force): " << flat.size() << "\n";
}

void process_part2(std::vector<Scratchcard>& cards)
{
    for (size_t i = 0; i < cards.size(); ++i) {
        for (size_t j = i + 1; j <= i + cards[i].match_count; ++j) {
            cards[j].sum += cards[i].sum;
        }
    }

    std::cout << "Part 2: " << std::accumulate(cards.begin(), cards.end(), 0, [](int sum, const Scratchcard& card) {
        return sum + card.sum;
    }) << "\n";
}

int main(int argc, char** argv)
{
    std::vector<Scratchcard> cards;
    cards.reserve(kTestCase1.size());

    for (int index = 0; index < std::ssize(kTestCase1); ++index) {
        cards.push_back(parse(kTestCase1[index]));
    }

    process_part1(cards);
    process_part2_brute_force(cards);

    if (argc == 1) return 0;

    cards.clear();

    auto file = std::ifstream(std::string{argv[1]}, std::ios::in);
    if (!file.is_open()) {
        std::cout << "Error openning file\n";
        return 1;
    }

    std::string line;
    int index = 0;
    while (true) {
        std::getline(file, line);
        if (line.empty()) break;

        cards.push_back(parse(line));
        index++;
    }

    process_part1(cards);
    process_part2_brute_force(cards);
    process_part2(cards);

    return 0;
}

