
#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <set>
#include <string_view>
#include <vector>

struct GameSet
{
    int r{0};
    int g{0};
    int b{0};
};

struct Game
{
    int id{0};
    std::vector<GameSet> sets;
};

auto parse_set(std::string_view input) noexcept -> GameSet
{
    GameSet set;

    auto set_component = [&set](std::string_view component) {
        int result{};
        auto [ptr, ec] = std::from_chars(component.data(), component.data() + component.size(), result);

        std::string_view non_int{ptr};

        if (non_int.starts_with(" red")) {
            set.r = result;
        } else if (non_int.starts_with(" green")) {
            set.g = result;
        } else {
            set.b = result;
        }
    };

    // std::cout << input << "\n";
    auto parsable = input;
    while (true) {
        auto component = parsable.find(",");
        if (component == std::string_view::npos) {
            set_component(parsable.substr(1));
            break;
        }

        set_component(parsable.substr(1, component));

        parsable = parsable.substr(component + 1);
    }

    std::cout << "set: " << set.r << "-" << set.g << "-" << set.b << "\n";

    return set;
}

auto parse_game(std::string_view line) noexcept -> Game
{
    auto id_part = line.find_first_of(':');

    auto game_info = line.substr(0, id_part);
    // id
    auto id_start = game_info.find_first_of(' ') + 1;
    auto id_string = game_info.substr(id_start, game_info.size() - id_start);

    int id{0};
    auto ec = std::from_chars(id_string.data(), id_string.data() + id_string.size(), id);

    Game game{.id = id};

    auto sets = line.substr(id_part + 1, line.size() - id_part + 1);
    while (true) {
        auto pos = sets.find_first_of(';');
        if (pos == std::string_view::npos) {
            game.sets.push_back(parse_set(sets));
            break;
        }

        auto set = sets.substr(0, pos);
        game.sets.push_back(parse_set(set));

        sets = sets.substr(pos + 1);
    }

    return game;
}

constexpr std::array<std::string_view, 5> kTestCase1{
    "Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green",
    "Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue",
    "Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red",
    "Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red",
    "Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green"};

int main(int argc, char** argv)
{
    std::vector<Game> games;
    for (auto game : kTestCase1) {
        games.push_back(parse_game(game));
    }

    int red_limit = 12;
    int green_limit = 13;
    int blue_limit = 14;

    std::cout << std::accumulate(games.begin(), games.end(), 0, [&](int sum, const auto& game) {
        auto is_over_limit = [&](const auto& set) {
            return set.r > red_limit || set.g > green_limit || set.b > blue_limit;
        };

        if (std::ranges::any_of(game.sets, is_over_limit)) {
            return sum;
        }

        return sum + game.id;
    }) << "\n";

    if (argc == 1) return 0;

    auto file = std::ifstream(std::string{argv[1]}, std::ios::in);
    if (!file.is_open()) {
        std::cout << "Error openning file\n";
        return 1;
    }

    std::vector<Game> input_games;

    std::string line;
    auto sum = 0;
    auto sum_p2 = 0;
    while (true) {
        std::getline(file, line);
        if (line.empty()) break;

        input_games.push_back(parse_game(line));
    }

    std::cout << "\n****\npart 1\n****\n";
    std::cout << std::accumulate(input_games.begin(), input_games.end(), 0, [&](int sum, const auto& game) {
        auto is_over_limit = [&](const auto& set) {
            return set.r > red_limit || set.g > green_limit || set.b > blue_limit;
        };

        if (std::ranges::any_of(game.sets, is_over_limit)) {
            return sum;
        }

        return sum + game.id;
    }) << "\n";

    std::cout << "\n****\npart 2\n****\n";
    std::cout << std::accumulate(input_games.begin(), input_games.end(), 0, [&](int sum, const Game& game) {
        auto max_r = std::ranges::max_element(game.sets, std::less<>(), &GameSet::r);
        auto max_g = std::ranges::max_element(game.sets, std::less<>(), &GameSet::g);
        auto max_b = std::ranges::max_element(game.sets, std::less<>(), &GameSet::b);

        return sum + (max_r->r * max_g->g * max_b->b);
    }) << "\n";
    return 0;
}

