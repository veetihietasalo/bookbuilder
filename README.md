# bookbuilder

Rebuilds full-depth order books from raw exchange market data, in C++26. It starts with Nasdaq TotalView-ITCH 5.0.

The goal: replay a full trading day, rebuild the book of every symbol, check each book against a naive reference, and measure throughput and per-message latency. Venues beyond Nasdaq come later.

## Status

Early: 26 lines of C++ so far. Working today:

- `decode.cpp` reads the ITCH 5.0 file framing (a 2-byte big-endian length, then the message) from stdin and prints how many messages of each type it saw.
- `smoke.cpp` checks the C++26 reflection toolchain: it prints a struct's name with `std::meta::identifier_of(^^AddOrder)`.

Not started: message structs, the book, the reference book, the benchmark. There are no results yet. Numbers will go here once they are measured on a verified full-day file.

## Roadmap

- [x] Toolchain: GCC 16 with C++26 reflection (`-freflection`)
- [x] Decode ITCH 5.0 framing
- [x] Count message types over a day
- [ ] Parse each message type into its own struct (reflection-generated big-endian decoders)
- [ ] Naive reference book
- [ ] Differential test: every book checked against the reference
- [ ] Fast book and a benchmark table (throughput, per-message latency, perf counters)
- [ ] Venue-agnostic event layer, second venue: IEX DEEP
- [ ] Eurex EOBI (via Databento MBO)

## Design

An ITCH message says which order changed, not where that order sits. An `Order Executed` carries an order ID and a share count, with no price and no side. So the book keeps two structures:

| Structure | Key | Answers |
|---|---|---|
| `orders` | order ID | where is order 42: side, price, shares left |
| `bids` / `asks` | price, sorted | the best bid or ask, and how much is there |

Every update goes through both: find the order by ID, then adjust its price level. When an order or a level reaches zero shares it is removed. Otherwise the book shows liquidity that is gone.

## Build and run

Needs Linux and GCC 16+.

```bash
g++ -std=c++26 -O2 decode.cpp -o decode
zcat 12302019.NASDAQ_ITCH50.gz | ./decode
g++ -std=c++26 -freflection smoke.cpp -o smoke && ./smoke    # prints AddOrder
```

Nasdaq publishes free full-day sample files, about 3.5 GB compressed each: https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH/. The data is not in the repo.

Checked with GCC 16.2.1 on Fedora 44 (WSL2). `decode` ran on the first 60 MB of the compressed 30 Dec 2019 file, not on a full day.

## What I learned

- ITCH files are length-prefixed: 2 bytes big-endian, then the message, whose first byte is its type. That is enough to walk a day and count types before any message is parsed.
- Execute, cancel and delete messages refer to an order by ID only. The book needs a lookup by ID as well as a price-sorted view, and each update touches both.
- A book that never removes empty orders and levels drifts away from the market. Removal is part of the design from the start.
- C++26 reflection needs a bleeding-edge compiler (GCC 16 with `-freflection`). The plan is to generate the big-endian decoders from the struct definitions instead of writing each one by hand.

## Related

[MiniHFT](https://github.com/veetihietasalo/MiniHFT) already has an L3 book and a replay of the same Nasdaq day. bookbuilder is a separate project with its own roadmap.

## References

- [Nasdaq TotalView-ITCH 5.0 specification](https://nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf)

MIT, see [LICENSE](LICENSE).
