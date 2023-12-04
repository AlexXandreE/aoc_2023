
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
#include <vector>

constexpr std::array<std::string_view, 10> kTestCase1{
    "467..114..",
    "...*......",
    "..35..633.",
    "......#...",
    "617*......",
    ".....+.58.",
    "..592.....",
    "......755.",
    "...$.*....",
    ".664.598..",
};

struct EnginePart
{
    int line{0};
    std::pair<int, int> range;

    int value{0};
    bool active{false};
};

struct Symbol
{
    int line{0};
    int column{0};

    std::vector<EnginePart> multiply_parts;  // part 2
};

void parse(
    std::string_view input,
    std::vector<EnginePart>& parts,
    std::vector<Symbol>& symbols,
    int line_index) noexcept
{
    for (int i = 0; i < std::ssize(input); ++i) {
        if (input[i] == '.') continue;

        if (std::isdigit(static_cast<unsigned char>(input[i])) != 0) {
            int value{};

            auto number_info = input.substr(i);
            auto [ptr, ec] = std::from_chars(number_info.data(), number_info.data() + number_info.size(), value);

            auto length = (ptr - number_info.data()) - 1;
            parts.push_back(EnginePart{line_index, std::pair<int, int>{i, i + length}, value});
            i += length;
            continue;
        }

        symbols.push_back(Symbol{.line = line_index, .column = i});
    }
}

// only accepts * as symbols
void parse_p2(
    std::string_view input,
    std::vector<EnginePart>& parts,
    std::vector<Symbol>& symbols,
    int line_index) noexcept
{
    for (int i = 0; i < std::ssize(input); ++i) {
        if (input[i] == '*') {
            symbols.push_back(Symbol{.line = line_index, .column = i});
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(input[i])) != 0) {
            int value{};

            auto number_info = input.substr(i);
            auto [ptr, ec] = std::from_chars(number_info.data(), number_info.data() + number_info.size(), value);

            auto length = (ptr - number_info.data()) - 1;
            parts.push_back(EnginePart{line_index, std::pair<int, int>{i, i + length}, value});
            i += length;
        }
    }
}

int main(int argc, char** argv)
{

    std::cout << "Hello world\n";

    std::vector<EnginePart> parts;
    std::vector<Symbol> symbols;

    for (int index = 0; index < std::ssize(kTestCase1); ++index) {
        parse(kTestCase1[index], parts, symbols, index);
    }

    auto compute_sum = [&]() {
        for (auto& part : parts) {

            auto in_range = std::ranges::any_of(symbols, [&part](const Symbol& symbol) {
                return (symbol.line == part.line - 1 || symbol.line == part.line || symbol.line == part.line + 1) &&
                       (symbol.column >= part.range.first - 1 && symbol.column <= part.range.second + 1);
            });

            part.active = in_range;
        }

        int sum = 0;
        for (const auto& part : parts | std::ranges::views::filter(&EnginePart::active)) {
            sum += part.value;
        }

        return sum;
    };

    std::cout << "Example: " << compute_sum() << "\n";

    {  // Example part 2
        parts.clear();
        symbols.clear();

        int index = 0;
        for (int i = 0; i < std::ssize(kTestCase1); ++i) {
            parse_p2(kTestCase1[i], parts, symbols, i);
        }

        for (auto& part : parts) {

            for (auto& symbol : symbols) {
                auto in_range =
                    (symbol.line == part.line - 1 || symbol.line == part.line || symbol.line == part.line + 1) &&
                    (symbol.column >= part.range.first - 1 && symbol.column <= part.range.second + 1);

                if (in_range) symbol.multiply_parts.push_back(part);
            }
        }

        int sum = 0;
        for (auto& symbol : symbols) {
            if (symbol.multiply_parts.size() != 2) continue;
            sum += symbol.multiply_parts[0].value * symbol.multiply_parts[1].value;
        }

        std::cout << "Example Part 2: " << sum << "\n";
    }

    if (argc == 1) return 0;

    {  // part 1
        auto file = std::ifstream(std::string{argv[1]}, std::ios::in);
        if (!file.is_open()) {
            std::cout << "Error openning file\n";
            return 1;
        }

        parts.clear();
        symbols.clear();

        std::string line;
        int index = 0;
        while (true) {
            std::getline(file, line);
            if (line.empty()) break;

            parse(line, parts, symbols, index);
            index++;
        }

        std::cout << "Part 1: " << compute_sum() << "\n";
    }
    parts.clear();
    symbols.clear();

    auto file = std::ifstream(std::string{argv[1]}, std::ios::in);
    if (!file.is_open()) {
        std::cout << "Error openning file\n";
        return 1;
    }

    {  // part 2
        std::string line;
        int index = 0;
        while (true) {
            std::getline(file, line);
            if (line.empty()) break;

            parse_p2(line, parts, symbols, index);
            index++;
        }

        for (auto& part : parts) {

            for (auto& symbol : symbols) {
                auto in_range =
                    (symbol.line == part.line - 1 || symbol.line == part.line || symbol.line == part.line + 1) &&
                    (symbol.column >= part.range.first - 1 && symbol.column <= part.range.second + 1);

                if (in_range) symbol.multiply_parts.push_back(part);
            }
        }

        int sum = 0;
        for (auto& symbol : symbols) {
            if (symbol.multiply_parts.size() != 2) continue;
            sum += symbol.multiply_parts[0].value * symbol.multiply_parts[1].value;
        }

        std::cout << "Part 2: " << sum << "\n";
    }

    return 0;
}

