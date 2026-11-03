// Number-base converter: decimal <-> binary / octal / hexadecimal, plus two's complement
#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>

const std::string DIGITS = "0123456789ABCDEF";

std::string fromDecimal(unsigned long long value, int base) {
    if (value == 0) return "0";
    std::string out;
    while (value > 0) {                 // repeated division, remainders read backwards
        out += DIGITS[value % base];
        value /= base;
    }
    std::reverse(out.begin(), out.end());
    return out;
}

unsigned long long toDecimal(const std::string& text, int base) {
    unsigned long long value = 0;
    for (char ch : text) {
        size_t d = DIGITS.find(static_cast<char>(std::toupper(ch)));
        if (d == std::string::npos || static_cast<int>(d) >= base)
            throw std::invalid_argument("digit '" + std::string(1, ch) + "' is not valid in base " + std::to_string(base));
        value = value * base + d;       // positional value: multiply by base and add the digit
    }
    return value;
}

std::string twosComplement8(int value) {
    if (value < -128 || value > 127) throw std::out_of_range("8-bit range is -128..127");
    unsigned v = static_cast<unsigned>(value) & 0xFF;
    std::string bits = fromDecimal(v, 2);
    return std::string(8 - bits.size(), '0') + bits;
}

int main() {
    std::cout << "Decimal ->  Binary            Octal   Hex\n";
    for (unsigned long long n : {0ULL, 10ULL, 45ULL, 255ULL, 1024ULL, 2026ULL})
        std::cout << "  " << n << std::string(9 - std::to_string(n).size(), ' ') << fromDecimal(n, 2)
                  << std::string(18 - fromDecimal(n, 2).size(), ' ') << fromDecimal(n, 8)
                  << std::string(8 - fromDecimal(n, 8).size(), ' ') << fromDecimal(n, 16) << '\n';

    std::cout << "\nOther bases -> decimal:\n";
    std::cout << "  binary 101101  = " << toDecimal("101101", 2) << '\n';
    std::cout << "  octal  755     = " << toDecimal("755", 8) << '\n';
    std::cout << "  hex    FF      = " << toDecimal("FF", 16) << '\n';
    std::cout << "  hex    7E3     = " << toDecimal("7e3", 16) << '\n';
    try { toDecimal("1021", 2); } catch (const std::exception& e) { std::cout << "  binary 1021    -> error: " << e.what() << '\n'; }

    std::cout << "\nRound trip check (hex -> binary through decimal): 3C = " << fromDecimal(toDecimal("3C", 16), 2) << '\n';

    std::cout << "\n8-bit two's complement:\n";
    for (int v : {5, -5, 127, -128, -1})
        std::cout << "  " << v << std::string(6 - std::to_string(v).size(), ' ') << "-> " << twosComplement8(v) << '\n';
    return 0;
}
