// Logic gate simulator: prints the truth table of every basic gate
#include <functional>
#include <iostream>
#include <string>
#include <vector>

struct Gate {
    std::string name;
    std::function<bool(bool, bool)> eval;
};

int main() {
    std::vector<Gate> gates = {
        {"AND", [](bool a, bool b) { return a && b; }},
        {"OR", [](bool a, bool b) { return a || b; }},
        {"NAND", [](bool a, bool b) { return !(a && b); }},
        {"NOR", [](bool a, bool b) { return !(a || b); }},
        {"XOR", [](bool a, bool b) { return a != b; }},
        {"XNOR", [](bool a, bool b) { return a == b; }},
    };

    std::cout << "NOT gate:  A | Y\n";
    for (int a = 0; a <= 1; a++) std::cout << "           " << a << " | " << !a << '\n';

    std::cout << "\n A B |";
    for (const Gate& g : gates) std::cout << ' ' << g.name << std::string(5 - g.name.size(), ' ');
    std::cout << "\n-----+" << std::string(gates.size() * 6, '-') << '\n';
    for (int a = 0; a <= 1; a++)
        for (int b = 0; b <= 1; b++) {
            std::cout << ' ' << a << ' ' << b << " |";
            for (const Gate& g : gates) std::cout << "   " << g.eval(a, b) << "  ";
            std::cout << '\n';
        }

    // NAND is a universal gate: build NOT, AND and OR from NAND only
    auto nand = [](bool a, bool b) { return !(a && b); };
    auto notN = [&](bool a) { return nand(a, a); };
    auto andN = [&](bool a, bool b) { return notN(nand(a, b)); };
    auto orN = [&](bool a, bool b) { return nand(notN(a), notN(b)); };
    bool allOk = true;
    for (int a = 0; a <= 1; a++)
        for (int b = 0; b <= 1; b++)
            allOk = allOk && notN(a) == !a && andN(a, b) == (a && b) && orN(a, b) == (a || b);
    std::cout << "\nNOT, AND and OR built only from NAND gates match the real gates: " << (allOk ? "yes" : "no") << '\n';
    return 0;
}
