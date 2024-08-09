#include <cctype>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

void print_word(const char* input_line, const char* pattern) {
    for (int i = 0; i < 1000; ++i) {
        char cur = *(input_line + i);
        if (cur == '\0') {
            break;
        }
        std::cout << cur;
    }
    std::cout << " : ";
    for (int i = 0; i < 1000; ++i) {
        char cur = *(pattern + i);
        if (cur == '\0') {
            break;
        }
        std::cout << cur;
    }
    std::cout << std::endl;
}

// this is gonna be a long refactor
// list of missing features
// todo, group match and others do not handle . and other symbols
// https://app.codecrafters.io/courses/grep/stages/sh9?repo=2e976eb6-1248-4212-8746-c70fa948d07b
// random havard prof yapping (but its useful)
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
bool match_pattern(const char* input_line, const char* pattern);
bool match_all(const char* input_line, const char* pattern);

bool group_match(const char* input_line, const char* pattern) {
    std::vector<char> charMatches;
    bool inverse = false;
    int endBracket = 1;

    if (*input_line == '\0') {
        return false;
    }

    while (*(pattern + endBracket) != '\0') {
        char curChar = *(pattern + endBracket);
        ++endBracket;
        if (curChar == ']') {
            break;
        }

        if (curChar == '^') {
            inverse = true;
            continue;
        }

        charMatches.push_back(curChar);
    }

    bool containsChar = false;
    for (char curChar : charMatches) {
        if (curChar == *input_line) {
            containsChar = true;
            break;
        }
    }

    if (containsChar != inverse) {
        return match_pattern(input_line + 1, pattern + endBracket + 1);
    } else {
        return false;
    }
}

bool match_one_or_more(const char* input_line, const char* pattern) {
    char wanted = *pattern;
    int occr = 0;
    while (wanted == *(input_line + occr)) {
        ++occr;
    }
    return match_pattern(input_line + occr, pattern + 2);
}

bool match_optional(const char* input_line, const char* pattern) {
    char wanted = *pattern;
    if (*input_line == wanted) {
        return match_pattern(input_line + 1, pattern + 2);
    } else {
        return match_pattern(input_line, pattern + 2);
    }
}

bool either_match(const char* input_line, const char* pattern) {
    std::string newPattern = pattern;
    size_t beg = newPattern.find("(");
    size_t mid = newPattern.find("|");
    size_t end = newPattern.find(")");
    size_t endEnd = newPattern.size();

    std::string word1 = newPattern.substr(beg + 1, mid - 1 - beg - 1);
    std::string word2 = newPattern.substr(mid + 1, end - 1 - mid - 1);
    std::string remainder = newPattern.substr(end + 1, endEnd - end - 1);
    if (end + 1 >= endEnd) {
        remainder = "";
    }
    word1 += remainder;
    word2 += remainder;
    std::cout << word1 << std::endl;
    std::cout << word2 << std::endl;
    return match_all(input_line, word1.c_str()) ||
           match_all(input_line, word2.c_str());

    /*std::string word1 = std::substr()*/

    /*std::vector<char> word;*/
    /*std::vector<char> word2;*/
    /*int cnt = 0;*/
    /*int cnt2 = 0;*/
    /*get_word(word, pattern + 1, cnt);*/
    /*get_word(word2, pattern + cnt + 2, cnt2);*/
}

bool match_pattern(const char* input_line, const char* pattern) {
    print_word(input_line, pattern);
    if (*pattern == '\0') {
        return true;
    }

    if (*pattern == '.') {
        return match_pattern(input_line + 1, pattern + 1);
    }

    if (*pattern == '[') {
        return group_match(input_line, pattern);
    }

    if (*pattern == '(') {
        return either_match(input_line, pattern);
    }

    if (*pattern == '\\') {
        char escCode = *(pattern + 1);
        if (escCode == 'd' && isdigit(*input_line)) {
            return match_pattern(input_line + 1, pattern + 2);
        }
        if (escCode == 'w' && isalnum(*input_line)) {
            return match_pattern(input_line + 1, pattern + 2);
        }
        return false;
    }

    if (*pattern == '$') {
        return (*input_line == '\0');
    }

    if (*(pattern + 1) == '?') {
        return match_optional(input_line, pattern);
    }

    if (*input_line == *pattern) {
        char nextChar = *(pattern + 1);
        if (nextChar == '+') {
            return match_one_or_more(input_line, pattern);
        } else {
            return match_pattern(input_line + 1, pattern + 1);
        }
    }
    return false;
}

bool match_all(const char* input_line, const char* pattern) {
    do {
        std::cout << "---------------------------------------------"
                  << std::endl;
        if (*pattern == '^') {
            char startChar = *(pattern + 1);
            if (*input_line != startChar) {
                return false;
            }
            return match_pattern(input_line + 1, pattern + 2);
        }

        if (match_pattern(input_line, pattern)) {
            return true;
        }
    } while (*input_line++ != '\0');
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

    std::string input_line;
    std::getline(std::cin, input_line);
    const char* c_input_line = input_line.c_str();
    const char* c_pattern = pattern.c_str();

    try {
        if (match_all(c_input_line, c_pattern)) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
