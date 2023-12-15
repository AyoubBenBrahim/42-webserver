#include <Utils.hpp>

// the config file does not contain '[]'!
bool	bracketsBalance(const std::string& str) {
    std::stack<char> stack;
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        if (c == '{' || c == '[') {
            stack.push(c);
        } else if (c == '}' || c == ']') {
            if (stack.empty()) {
                return (false); // Closing bracket with no matching opening bracket
            }
            if ((c == '}' &&  stack.top() == '{') || (c == ']' &&  stack.top() == '[')) {
                stack.pop();
            } else {
                return (false); // Mismatched brackets
            }
        }
    }
    return (stack.empty()); // Check for extra opening brackets
}
