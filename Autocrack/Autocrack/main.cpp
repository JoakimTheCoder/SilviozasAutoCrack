#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <windows.h>

bool edit_string_to(std::string filePath, std::string oldText, std::string newText) {
    if (newText.size() > oldText.size()) return false;

    std::ifstream in(filePath, std::ios::binary);
    if (!in) return false;

    std::vector<char> buffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    bool replaced = false;
    for (size_t i = 0; i + oldText.size() <= buffer.size(); ++i) {
        if (std::memcmp(&buffer[i], oldText.data(), oldText.size()) == 0) {
            std::memcpy(&buffer[i], newText.data(), newText.size());
            for (size_t j = newText.size(); j < oldText.size(); ++j)
                buffer[i + j] = '\0';
            replaced = true;
        }
    }

    if (replaced) {
        std::ofstream out(filePath, std::ios::binary | std::ios::trunc);
        if (!out) return false;
        out.write(buffer.data(), buffer.size());
        out.close();
    }

    return replaced;
}
static inline uint8_t hexCharToVal(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    throw std::invalid_argument("invalid hex char");
}

std::vector<uint8_t> parseHexBytes(std::string s) {
    std::string t;
    for (char c : s) if (!isspace(static_cast<unsigned char>(c))) t.push_back(c);
    if (t.size() % 2 != 0) throw std::invalid_argument("hex string length must be even");

    std::vector<uint8_t> out;
    for (size_t i = 0; i < t.size(); i += 2) {
        uint8_t b = (hexCharToVal(t[i]) << 4) | hexCharToVal(t[i + 1]);
        out.push_back(b);
    }
    return out;
}

bool patchBytesFromOffset(std::string path, size_t offset, std::string hexBytes) {
    std::vector<uint8_t> patch;
    try {
        patch = parseHexBytes(hexBytes);
    }
    catch (...) {
        return false;
    }

    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) return false;

    std::streamsize sz = in.tellg();
    in.seekg(0, std::ios::beg);
    std::vector<uint8_t> buf(static_cast<size_t>(sz));
    if (!in.read(reinterpret_cast<char*>(buf.data()), sz)) return false;
    in.close();

    if (offset + patch.size() > buf.size()) buf.resize(offset + patch.size(), 0x00);

    for (size_t i = 0; i < patch.size(); ++i) buf[offset + i] = patch[i];

    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(buf.data()), buf.size());
    return out.good();
}

void patch(std::string path, size_t offset, std::string bytes) {
    if (patchBytesFromOffset(path, offset, bytes))
        printf("Successfully patched!\n");
    else
    {
        printf("Failed to patch. (Close the program if it's open)\n");
        while (true) Sleep(1);
    }
}


void edit_string(std::string filePath, std::string oldText, std::string newText) {
    if (!edit_string_to(filePath, oldText, newText))
        printf("Failed to replace \"%s\"  ->  \"%s\"  (Not found. It was probably replaced already)\n", oldText.c_str(), newText.c_str());
    else
        printf("Successfully replaced \"%s\"  ->  \"%s\"\n", oldText.c_str(), newText.c_str());
}

std::string original_ownerid = "fRfa2mUrtR";
std::string original_secret = "dcce38172ead56bccb8fba908deb3b7baac048f0383c28f60d76f515f77b950a";
std::string new_ownerid = "KUJxDZyPai";
std::string new_secret = "e45b151a42f87533b967fdb497d48e874b4668085f0600fa1b69a102c070ddcd";


int main() {
    /*   Works with V3.87461. (If new version is out, tell joakimmer1k0)    */


    std::string path = R"(C:\Users\Public\Proxy_Stuff\Silviozas Premium Proxy.exe)";
    
    edit_string(path, "[Debugger Detected]", "]21211322 12312311["); // silviozas's proxy server detects cracking with this text packet, which then gives silviozas info of ur pc.
    edit_string(path, original_ownerid, new_ownerid);
    edit_string(path, original_secret, new_secret);

    patch(path, 0x624077, "90 90 89 05 B9 28 2A 00 EB 0E 8B 05 A1 28 2A 00"); // 950a (first check)
    patch(path, 0x624087, "90 90 89 05 99 28 2A 00 48 8D 15 06 DD 1D 00 48"); // 950a (first check)

    patch(path, 0x624138, "90 90 89 05 F8 27 2A 00 EB 0E 8B 05 E0 27 2A 00"); // a (first check)
    patch(path, 0x624148, "90 90 89 05 D8 27 2A 00 48 8D 84 24 50 02 00 00"); // a (first check)

    patch(path, 0x5F1FD7, "90 90 89 05 59 49 2D 00 EB 0E 8B 05 41 49 2D 00"); // 950a
    patch(path, 0x5F1FE7, "90 90 89 05 39 49 2D 00 48 8D 15 5E 68 20 00 48"); // 950a

    patch(path, 0x5F2098, "90 90 89 05 98 48 2D 00 EB 0E 8B 05 80 48 2D 00"); // a
    patch(path, 0x5F20A8, "90 90 89 05 78 48 2D 00 E8 CB F8 02 00 48 8D 15"); // a

    patch(path, 0x6219AE, "75 05 E9 EB 23 00 00 48 8D 84 24 10 12 00 00 48");



    /* PING REPLY (SERVER) */
    patch(path, 0x5EB960, "75 1A 8B 05 C0 AF 2D 00 FF C0 89 05 B8 AF 2D 00");
    patch(path, 0x5EBA12, "75 1A 8B 05 0E AF 2D 00 FF C0 89 05 06 AF 2D 00");
    patch(path, 0x59B3D0, "75 1A 8B 05 50 B5 32 00 FF C0 89 05 48 B5 32 00");
    patch(path, 0x59B482, "75 1A 8B 05 9E B4 32 00 FF C0 89 05 96 B4 32 00");
    patch(path, 0x406750, "75 1A 8B 05 D0 01 4C 00 FF C0 89 05 C8 01 4C 00");
    patch(path, 0x406802, "75 1A 8B 05 1E 01 4C 00 FF C0 89 05 16 01 4C 00");

    /* PING REPLY (CLIENT) */
    patch(path, 0x496D0B, "75 0E C6 05 FF FB 42 00 01 B0 01 E9 DB 11 00 00");


    /* CAN'T MOVE */
    patch(path, 0x40AC1C, "75 05 E9 2F 12 00 00 48 8B 84 24 10 1F 00 00 0F");
    patch(path, 0x5EFE2C, "75 05 E9 2F 12 00 00 48 8B 84 24 10 1F 00 00 0F");
    patch(path, 0x59F89C, "75 05 E9 2F 12 00 00 48 8B 84 24 10 1F 00 00 0F");
    patch(path, 0x40A5B7, "75 05 E9 94 18 00 00 48 8B 84 24 10 1F 00 00 0F");
    patch(path, 0x5EF7C7, "75 05 E9 94 18 00 00 48 8B 84 24 10 1F 00 00 0F");
   // patch(path, 0x408EF3, "75 05 E9 58 2F 00 00 48 8B 44 24 40 0F B6 00 89");


    /* CAN'T COLLECT ITEMS */
    patch(path, 0x59F237, "75 05 E9 94 18 00 00 48 8B 84 24 10 1F 00 00 0F");
    patch(path, 0x40114E, "75 05 E9 C8 62 00 00 48 8B 84 24 C0 42 00 00 0F");
    patch(path, 0x595DCE, "75 05 E9 C8 62 00 00 48 8B 84 24 C0 42 00 00 0F");

    /* OBJECTS DON'T UPDATE */
    patch(path, 0x5E635E, "75 05 E9 C8 62 00 00 48 8B 84 24 C0 42 00 00 0F");


    /* CHECK IN WORLD JOINING */
    patch(path, 0x496C4A, "0F 85 CB 00 00 00 48 8B 84 24 70 0A 00 00 0F B6");


    //patch(path, 0x5F1FE7, "90 90 89 05 39 49 2D 00 48 8D 15 5E 68 20 00 48"); // 950a
    //patch(path, 0x5F20A8, "90 90 89 05 78 48 2D 00 E8 CB F8 02 00 48 8D 15"); // a


    while (true) Sleep(1);
    return 0;
}