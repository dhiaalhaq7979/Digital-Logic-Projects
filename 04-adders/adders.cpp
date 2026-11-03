// Half adder, full adder and a 4-bit ripple-carry adder built from logic gates
#include <bitset>
#include <iostream>

struct Sum { bool s, carry; };

Sum halfAdder(bool a, bool b) { return {a != b, a && b}; }          // S = A XOR B, C = A AND B

Sum fullAdder(bool a, bool b, bool cin) {                            // two half adders + OR gate
    Sum first = halfAdder(a, b);
    Sum second = halfAdder(first.s, cin);
    return {second.s, first.carry || second.carry};
}

// adds two 4-bit numbers bit by bit, the carry "ripples" to the next stage
unsigned rippleAdd4(unsigned x, unsigned y, bool& overflow) {
    bool carry = false;
    unsigned result = 0;
    for (int i = 0; i < 4; i++) {
        Sum r = fullAdder((x >> i) & 1, (y >> i) & 1, carry);
        result |= static_cast<unsigned>(r.s) << i;
        carry = r.carry;
    }
    overflow = carry;
    return result;
}

int main() {
    std::cout << "Half adder\n A B | S C\n-----+----\n";
    for (int a = 0; a <= 1; a++)
        for (int b = 0; b <= 1; b++) {
            Sum r = halfAdder(a, b);
            std::cout << ' ' << a << ' ' << b << " | " << r.s << ' ' << r.carry << '\n';
        }

    std::cout << "\nFull adder\n A B Cin | S Cout\n---------+-------\n";
    for (int a = 0; a <= 1; a++)
        for (int b = 0; b <= 1; b++)
            for (int c = 0; c <= 1; c++) {
                Sum r = fullAdder(a, b, c);
                std::cout << ' ' << a << ' ' << b << "  " << c << "  | " << r.s << "  " << r.carry << '\n';
            }

    std::cout << "\n4-bit ripple-carry adder (checked against normal + for all 256 inputs)\n";
    int errors = 0;
    for (unsigned x = 0; x < 16; x++)
        for (unsigned y = 0; y < 16; y++) {
            bool of;
            unsigned r = rippleAdd4(x, y, of);
            if (r + (of ? 16u : 0u) != x + y) errors++;
        }
    std::cout << "  mismatches: " << errors << '\n';
    for (auto [x, y] : {std::pair<unsigned, unsigned>{5, 3}, {9, 6}, {12, 7}, {15, 15}}) {
        bool of;
        unsigned r = rippleAdd4(x, y, of);
        std::cout << "  " << std::bitset<4>(x) << " + " << std::bitset<4>(y) << " = " << (of ? "1" : "0") << std::bitset<4>(r)
                  << "   (" << x << " + " << y << " = " << x + y << (of ? ", carry out = overflow in 4 bits" : "") << ")\n";
    }
    return 0;
}
