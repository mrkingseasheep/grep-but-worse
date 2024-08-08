#include <cctype>
#include <iostream>
#include <string>
#include <vector>

// this is gonna be a long refactor
// https://app.codecrafters.io/courses/grep/stages/sh9?repo=2e976eb6-1248-4212-8746-c70fa948d07b
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
bool match_pattern(const std::string* input_line, const std::string* pattern);

bool group_match(const std::string* input_line, const std::string* pattern) {
    std::vector<char> charMatches;
    bool inverse = false;
    int endBracket = 0;

    for (char curChar : *input_line) {
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
    int startChar = 1 + inverse; // removes the ^ char, if contained
    for (char curChar : charMatches) {
        if (curChar == input_line->at(startChar)) {
            containsChar = true;
            break;
        }
    }

    if (containsChar && !inverse || !containsChar && inverse) {
        return match_pattern(input_line + 1, pattern + endBracket + 1);
    } else {
        return false;
    }
}

bool match_single_wildcard(const std::string* input_line,
                           const std::string* pattern) {
    switch (pattern->at(1)) {
    case 'd':
        return (isdigit(input_line->at(0)))
                   ? match_pattern(input_line + 1, pattern + 2)
                   : false;
        break;
    case 'w':
        return (isalnum(input_line->at(0)))
                   ? match_pattern(input_line + 1, pattern + 2)
                   : false;
        break;
    default:
        return false;
        break;
    }
}

bool match_pattern(const std::string* input_line, const std::string* pattern) {
    std::cout << *input_line << " : " << *pattern << std::endl;
    if (pattern->at(0) == '\0') {
        std::cout << "HIT" << std::endl;
        return true;
    }

    if (pattern->at(0) == '[') {
        return group_match(input_line, pattern);
    }

    if (pattern->at(0) == '\\') {
        return match_single_wildcard(input_line, pattern);
    }

    if (input_line->at(0) == pattern->at(0)) {
        return match_pattern(input_line + 1, pattern + 1);
    }
    return false;
}

bool match_all(const std::string* input_line, const std::string* pattern) {
    bool isValid = false;
    int curPos = -1;
    do {
        ++curPos;
        std::cout << "CHECKING" << curPos << std::endl;
        std::cout << *(input_line + curPos) << " : " << *pattern << std::endl;
        isValid = isValid || match_pattern(input_line + curPos, pattern);
    } while (input_line->at(curPos) != '\0');
    return isValid;
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

    try {
        if (match_all(&input_line, &pattern)) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
