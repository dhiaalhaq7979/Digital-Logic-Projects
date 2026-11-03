// Truth-table generator for Boolean expressions
// Supported: variables A-Z, constants 0/1, NOT (! or ~ or '), AND (& or *), OR (| or +), XOR (^), parentheses
// Example: (A+B)*!C   or   A'B + AB'  (implicit AND between neighbours)
#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

// insert explicit '&' where AND is implicit (AB, A(B), A'B, )( ...)
std::string addImplicitAnd(const std::string& raw) {
    std::string s;
    for (char c : raw) if (!std::isspace(static_cast<unsigned char>(c))) s += c;
    std::string out;
    for (size_t i = 0; i < s.size(); i++) {
        out += s[i];
        if (i + 1 < s.size()) {
            char a = s[i], b = s[i + 1];
            bool leftEnds = std::isalpha(static_cast<unsigned char>(a)) || a == '0' || a == '1' || a == ')' || a == '\'';
            bool rightStarts = std::isalpha(static_cast<unsigned char>(b)) || b == '0' || b == '1' || b == '(' || b == '!' || b == '~';
            if (leftEnds && rightStarts) out += '&';
        }
    }
    return out;
}

int precedence(char op) {
    switch (op) {
        case '!': return 4;
        case '&': return 3;
        case '^': return 2;
        case '|': return 1;
    }
    return 0;
}

// shunting-yard: infix -> postfix
std::string toPostfix(const std::string& expr) {
    std::string out;
    std::stack<char> ops;
    for (char c : expr) {
        if (c == '*') c = '&';
        if (c == '+') c = '|';
        if (c == '~') c = '!';
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '0' || c == '1') out += static_cast<char>(std::toupper(c));
        else if (c == '\'') out += '!';                       // postfix NOT applies to the operand just written
        else if (c == '(') ops.push(c);
        else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') { out += ops.top(); ops.pop(); }
            if (ops.empty()) throw std::runtime_error("unbalanced parentheses");
            ops.pop();
        } else if (c == '!') ops.push(c);                     // unary, right-associative
        else if (c == '&' || c == '|' || c == '^') {
            while (!ops.empty() && ops.top() != '(' && precedence(ops.top()) >= precedence(c)) { out += ops.top(); ops.pop(); }
            ops.push(c);
        } else throw std::runtime_error(std::string("unknown symbol '") + c + "'");
    }
    while (!ops.empty()) {
        if (ops.top() == '(') throw std::runtime_error("unbalanced parentheses");
        out += ops.top();
        ops.pop();
    }
    return out;
}

bool evaluate(const std::string& postfix, const std::map<char, bool>& values) {
    std::stack<bool> st;
    for (char c : postfix) {
        if (c == '0' || c == '1') st.push(c == '1');
        else if (std::isalpha(static_cast<unsigned char>(c))) st.push(values.at(c));
        else if (c == '!') {
            if (st.empty()) throw std::runtime_error("missing operand");
            bool v = st.top(); st.pop(); st.push(!v);
        } else {
            if (st.size() < 2) throw std::runtime_error("missing operand");
            bool b = st.top(); st.pop();
            bool a = st.top(); st.pop();
            st.push(c == '&' ? (a && b) : c == '|' ? (a || b) : (a != b));
        }
    }
    if (st.size() != 1) throw std::runtime_error("invalid expression");
    return st.top();
}

void printTable(const std::string& expression) {
    std::string post = toPostfix(addImplicitAnd(expression));
    std::set<char> varSet;
    for (char c : post) if (std::isalpha(static_cast<unsigned char>(c))) varSet.insert(c);
    std::vector<char> vars(varSet.begin(), varSet.end());
    int n = static_cast<int>(vars.size());

    std::cout << "Expression: " << expression << "\n  ";
    for (char v : vars) std::cout << v << ' ';
    std::cout << "| Y\n  " << std::string(2 * n, '-') << "+--\n";
    std::vector<std::string> minterms;
    for (int row = 0; row < (1 << n); row++) {
        std::map<char, bool> values;
        std::string term;
        std::cout << "  ";
        for (int i = 0; i < n; i++) {
            bool bit = (row >> (n - 1 - i)) & 1;
            values[vars[i]] = bit;
            term += std::string(1, vars[i]) + (bit ? "" : "'");
            std::cout << bit << ' ';
        }
        bool y = evaluate(post, values);
        if (y) minterms.push_back(term);
        std::cout << "| " << y << '\n';
    }
    std::cout << "  Sum of products (minterms): ";
    if (minterms.empty()) std::cout << "0";
    for (size_t i = 0; i < minterms.size(); i++) std::cout << minterms[i] << (i + 1 < minterms.size() ? " + " : "");
    std::cout << "\n\n";
}

int main(int argc, char* argv[]) {
    std::vector<std::string> expressions;
    for (int i = 1; i < argc; i++) expressions.push_back(argv[i]);
    if (expressions.empty()) expressions = {"(A+B)*!C", "A'B + AB'", "!(A&B) | (B^C)"};
    for (const std::string& e : expressions) {
        try { printTable(e); }
        catch (const std::exception& ex) { std::cout << "Expression: " << e << "\n  error: " << ex.what() << "\n\n"; }
    }
    return 0;
}
