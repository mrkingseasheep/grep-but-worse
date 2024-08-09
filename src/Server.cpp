#include <cctype>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

bool has_pattern(const char* input, const char* pattern,
                 std::string& prevPattern);
bool match_all(const char* input, const char* pattern);

void print_word(const char* input, const char* pattern,
                std::string& prevPattern) {
    do {
        std::cout << *input;
    } while (*input++ != '\0');
    std::cout << " : ";
    do {
        std::cout << *pattern;
    } while (*pattern++ != '\0');
    std::cout << std::endl;
    std::cout << "=========================================> PREV CMD: "
              << prevPattern << std::endl;
}

bool has_espace_char(const char* input, const char* pattern,
                     std::string& prevPattern) {
    if (*(pattern + 1) == 'd') {
        if (isdigit(*input)) {
            prevPattern = "\\d";
            return has_pattern(input + 1, pattern + 2, prevPattern);
        }
    }

    if (*(pattern + 1) == 'w') {
        if (isalnum(*input)) {
            prevPattern = "\\w";
            return has_pattern(input + 1, pattern + 2, prevPattern);
        }
    }

    if (*(pattern + 1) == '\\') {
        if (*pattern == *input) {
            prevPattern = "\\\\";
            return has_pattern(input + 2, pattern + 2, prevPattern);
        }
    }

    return false;
}

bool has_char_group(const char* input, const char* pattern,
                    std::string& prevPattern) {
    // find all char in group
    std::vector<char> charGroup;
    bool wantedGroup = !(*(pattern + 1) == '^');
    int idx = !wantedGroup + 1;
    bool containsGroup = false;
    prevPattern = "[";
    if (!wantedGroup) {
        prevPattern += "^";
    }

    while (*(pattern + idx) != ']') {
        charGroup.push_back(*(pattern + idx));
        prevPattern += *(pattern + idx);
        ++idx;
    }
    prevPattern += "]";

    for (auto i = charGroup.cbegin(); i != charGroup.cend(); ++i) {
        // matching special escape codes
        if (*i == '\\') {
            if (*(i + 1) == '\\' && *input == '\\') {
                containsGroup = true;
            } else if (*(i + 1) == 'w' && isalnum(*input)) {
                containsGroup = true;
            } else if (*(i + 1) == 'd' && isdigit(*input)) {
                containsGroup = true;
            } // other cases are possible, I'm too lazy to implement them
        }

        // matching normal chars
        if (*i == *input) {
            containsGroup = true;
        }

        if (containsGroup && wantedGroup) {
            return has_pattern(input + 1, pattern + idx + 1, prevPattern);
        }
    }

    if (!containsGroup && !wantedGroup) {
        return has_pattern(input + 1, pattern + idx + 1, prevPattern);
    }

    return false;
}

bool has_pattern(const char* input, const char* pattern,
                 std::string& prevPattern) {
    print_word(input, pattern, prevPattern);

    // if pattern is empty, rest of input doesn't matter
    if (*pattern == '\0') {
        return true;
    }

    if (*pattern == '\\') {
        return has_espace_char(input, pattern, prevPattern);
    }

    if (*pattern == '[') {
        return has_char_group(input, pattern, prevPattern);
    }

    if (*pattern == '.') {
        if (*input != '\0') {
            prevPattern = ".";
            return has_pattern(input + 1, pattern + 1, prevPattern);
        }
        return false;
    }

    if (*pattern == '$') {
        return *input == '\0';
    }

    // if both chars are the same
    if (*input == *pattern) {
        prevPattern = *pattern;
        return has_pattern(input + 1, pattern + 1, prevPattern);
    }

    // in case nothing happens
    return false;
}

bool root_match(const char* input, const char* pattern) {
    std::string prevPattern; // litterally only for the + operator

    // get rid of edge case
    if (*pattern == '^') {
        return *input == *(pattern + 1);
    }

    do {
        std::cout << "-------------------------------------\n";
        if (has_pattern(input, pattern, prevPattern)) {
            return true;
        }
    } while (*input++ != '\0');
    return false;
}

int main(int argc, char* argv[]) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];

    if (flag != "-E") {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }

    std::string input;
    std::getline(std::cin, input);
    const char* c_input = input.c_str();
    const char* c_pattern = pattern.c_str();

    try {
        if (root_match(c_input, c_pattern)) {
            std::cout << "-------------------------------------\n";
            std::cout << "<PATTERN FOUND>" << std::endl;
            return 0;
        } else {
            std::cout << "-------------------------------------\n";
            std::cout << "<PATTERN NOT FOUND>" << std::endl;
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
