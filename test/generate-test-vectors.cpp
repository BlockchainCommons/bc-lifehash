#include "../src/lifehash.hpp"

#include <iostream>
#include <string>
#include <vector>

enum class InputType { utf8, hex_data };

struct TestCase {
    InputType input_type;
    std::string input;         // UTF-8 string or hex-encoded bytes
    LifeHash::Version version;
    std::string version_name;
    size_t module_size;
    bool has_alpha;
};

static TestCase utf8(const std::string& s, LifeHash::Version v, const std::string& vname, size_t ms = 1, bool alpha = false) {
    return {InputType::utf8, s, v, vname, ms, alpha};
}

static TestCase hex_data(const std::string& hex, LifeHash::Version v, const std::string& vname, size_t ms = 1, bool alpha = false) {
    return {InputType::hex_data, hex, v, vname, ms, alpha};
}

static std::string escape_json_string(const std::string& s) {
    std::string result;
    for (unsigned char c : s) {
        if (c == '"') {
            result += "\\\"";
        } else if (c == '\\') {
            result += "\\\\";
        } else if (c < 0x20) {
            char buf[8];
            snprintf(buf, sizeof(buf), "\\u%04x", c);
            result += buf;
        } else {
            result += c;
        }
    }
    return result;
}

static void emit_vector(const TestCase& tc, bool last) {
    LifeHash::Image image;
    std::string input_type_str;

    if (tc.input_type == InputType::utf8) {
        image = LifeHash::make_from_utf8(tc.input, tc.version, tc.module_size, tc.has_alpha);
        input_type_str = "utf8";
    } else {
        auto data = LifeHash::hex_to_data(tc.input);
        image = LifeHash::make_from_data(data, tc.version, tc.module_size, tc.has_alpha);
        input_type_str = "hex";
    }

    std::cout << "  {\n";
    std::cout << "    \"input\": \"" << escape_json_string(tc.input) << "\",\n";
    std::cout << "    \"input_type\": \"" << input_type_str << "\",\n";
    std::cout << "    \"version\": \"" << tc.version_name << "\",\n";
    std::cout << "    \"module_size\": " << tc.module_size << ",\n";
    std::cout << "    \"has_alpha\": " << (tc.has_alpha ? "true" : "false") << ",\n";
    std::cout << "    \"width\": " << image.width << ",\n";
    std::cout << "    \"height\": " << image.height << ",\n";
    std::cout << "    \"colors\": [";
    for (size_t j = 0; j < image.colors.size(); j++) {
        if (j > 0) std::cout << ", ";
        std::cout << (int)image.colors[j];
    }
    std::cout << "]\n";
    std::cout << "  }";
    if (!last) std::cout << ",";
    std::cout << "\n";
}

int main() {
    using V = LifeHash::Version;

    std::vector<TestCase> cases = {
        // ── version2 (recommended default) ──────────────────────────────
        utf8("",                                                    V::version2, "version2"),
        utf8("Hello",                                               V::version2, "version2"),
        utf8("0",                                                   V::version2, "version2"),
        utf8("The quick brown fox jumps over the lazy dog",         V::version2, "version2"),
        utf8("Blockchain Commons",                                  V::version2, "version2"),
        utf8("deadbeef",                                            V::version2, "version2"),
        utf8("\xf0\x9f\x8c\x8a",                                   V::version2, "version2"), // 🌊
        utf8("caf\xc3\xa9",                                        V::version2, "version2"), // café

        // version2 with module_size=2
        utf8("Hello",                                               V::version2, "version2", 2),
        utf8("Blockchain Commons",                                  V::version2, "version2", 2),

        // version2 with alpha
        utf8("Hello",                                               V::version2, "version2", 1, true),
        utf8("Blockchain Commons",                                  V::version2, "version2", 1, true),

        // ── version1 ────────────────────────────────────────────────────
        utf8("Hello",                                               V::version1, "version1"),
        utf8("Blockchain Commons",                                  V::version1, "version1"),
        utf8("test",                                                V::version1, "version1"),
        utf8("The quick brown fox jumps over the lazy dog",         V::version1, "version1"),
        utf8("\xf0\x9f\x8c\x8a",                                   V::version1, "version1"), // 🌊

        // ── detailed ────────────────────────────────────────────────────
        utf8("Hello",                                               V::detailed, "detailed"),
        utf8("Blockchain Commons",                                  V::detailed, "detailed"),
        utf8("test",                                                V::detailed, "detailed"),
        utf8("The quick brown fox jumps over the lazy dog",         V::detailed, "detailed"),
        utf8("\xf0\x9f\x8c\x8a",                                   V::detailed, "detailed"), // 🌊

        // ── fiducial ────────────────────────────────────────────────────
        utf8("Hello",                                               V::fiducial, "fiducial"),
        utf8("Blockchain Commons",                                  V::fiducial, "fiducial"),
        utf8("test",                                                V::fiducial, "fiducial"),
        utf8("The quick brown fox jumps over the lazy dog",         V::fiducial, "fiducial"),
        utf8("\xf0\x9f\x8c\x8a",                                   V::fiducial, "fiducial"), // 🌊

        // ── grayscale_fiducial ──────────────────────────────────────────
        utf8("Hello",                                               V::grayscale_fiducial, "grayscale_fiducial"),
        utf8("Blockchain Commons",                                  V::grayscale_fiducial, "grayscale_fiducial"),
        utf8("test",                                                V::grayscale_fiducial, "grayscale_fiducial"),
        utf8("The quick brown fox jumps over the lazy dog",         V::grayscale_fiducial, "grayscale_fiducial"),
        utf8("\xf0\x9f\x8c\x8a",                                   V::grayscale_fiducial, "grayscale_fiducial"), // 🌊

        // ── make_from_data (raw binary input via hex) ───────────────────
        hex_data("",                                                V::version2, "version2"),
        hex_data("00ff80",                                          V::version2, "version2"),
        hex_data("deadbeef",                                        V::version2, "version2"),
    };

    std::cout << "[\n";
    for (size_t i = 0; i < cases.size(); i++) {
        emit_vector(cases[i], i + 1 == cases.size());
    }
    std::cout << "]\n";

    return 0;
}
