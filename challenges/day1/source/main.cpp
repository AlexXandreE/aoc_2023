
#include <charconv>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

auto extract_number(std::string_view input) noexcept
{
    auto is_num = [](auto character) { return character >= '0' && character <= '9'; };
    auto first_char = std::find_if(input.begin(), input.end(), is_num);
    auto last_char = std::find_if(input.rbegin(), input.rend(), is_num);

    std::string final_string(std::string{} + first_char[0] + last_char[0]);

    int result{0};
    auto ec = std::from_chars(final_string.data(), final_string.data() + final_string.size(), result);

    return result;
}

constexpr std::array<std::string_view, 9> kValidInputs{"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
auto from_string(const std::string_view& input) noexcept
{
    auto first_digit = std::find_if(input.begin(), input.end(), isdigit);
    auto last_digit = std::find_if(input.rbegin(), input.rend(), isdigit);

    auto first_digit_index = static_cast<int>(std::distance(input.begin(), first_digit));
    auto last_digit_index = static_cast<int>(input.size() - std::distance(input.rbegin(), last_digit));

    auto [first_cardinal, last_cardinal] = [&]() -> std::pair<std::pair<int, int>, std::pair<int, int>> {
        auto first_string = input.substr(0, first_digit_index);
        auto last_string = input.substr(last_digit_index, input.size() - last_digit_index);

        std::pair<int, int> first = {input.size(), -1};
        std::pair<int, int> last = {-1, -1};

        for (int i = 0; i < kValidInputs.size(); ++i) {
            auto pos = first_string.find(kValidInputs[i]);
            if (pos != std::string::npos) {
                int absolute_index = static_cast<int>(pos);
                if (absolute_index < first.first) {
                    first.first = absolute_index;
                    first.second = i;
                }
            }

            pos = last_string.rfind(kValidInputs[i]);
            if (pos != std::string::npos) {
                int absolute_index = static_cast<int>(pos);
                if (absolute_index > last.first) {
                    last.first = absolute_index;
                    last.second = i;
                }
            }
        }

        return {first, last};
    }();

    auto first_coordinate =
        (first_digit_index < first_cardinal.first ? input[first_digit_index]
                                                  : std::to_string(first_cardinal.second + 1)[0]);

    auto last_coordinate =
        (last_digit_index > last_cardinal.first + last_digit_index ? input[last_digit_index - 1]
                                                                   : std::to_string(last_cardinal.second + 1)[0]);

    /*
    std::cout << input << "\nf - " << first_coordinate << " - "
              << "f2 - " << last_coordinate << "\n";
    */
    std::string final_string = std::string{} + first_coordinate + last_coordinate;

    int result{0};
    auto ec = std::from_chars(final_string.data(), final_string.data() + final_string.size(), result);

    return result;
}

int main(int argc, char** argv)
{
    const std::vector<std::string> test_input{"1abc2", "pqr3stu8vwx", "a1b2c3d4e5f", "treb7uchet"};

    std::vector<std::string> lines;

    std::vector<std::string> test_input2{
        "two1nine",
        "eightwothree",
        "abcone2threexyz",
        "xtwone3four",
        "4nineeightseven2",
        "zoneight234",
        "7pqrstsixteen"};

    auto accumulate = [](const auto& input, auto func) {
        return std::accumulate(
            input.begin(), input.end(), 0, [&](int sum, const auto& elem) { return sum + func(elem); });
    };

    std::cout << accumulate(test_input, extract_number) << "\n";
    std::cout << accumulate(test_input2, from_string) << "\n";

    if (argc == 2) {
        auto file = std::ifstream(std::string{argv[1]}, std::ios::in);
        if (!file.is_open()) {
            std::cout << "Error openning file\n";
            return 1;
        }

        std::string line;
        auto sum = 0;
        auto sum_p2 = 0;
        while (true) {
            std::getline(file, line);
            if (line.empty()) break;

            sum += extract_number(line);
            sum_p2 += from_string(line);
        }

        std::cout << "Final part 1 = " << sum << "\n";
        std::cout << "Final part 2 = " << sum_p2 << "\n";
    }

    return 0;
}

