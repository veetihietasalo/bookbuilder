#include <cstdint>
#include <cstdio>
#include <print>

int main() {
    unsigned char buf[65536];
    std::uint64_t counts[256] = {};   // 256 counters, all starting at 0
    while (true) {
        unsigned char hdr[2];
        if (std::fread(hdr, 1, 2, stdin) != 2) break;   // 2-byte header
        int len = hdr[0] * 256 + hdr[1];                // big-endian length
        std::fread(buf, 1, len, stdin);                 // the message itself
        counts[buf[0]] += 1;                            // count this message's type
    }
    for (int t = 0; t < 256; ++t) {
        if (counts[t] > 0) std::println("{} {}", char(t), counts[t]);
    }
}
