# bookbuilder

Rebuilds full-depth order books from raw exchange market data, in C++26.

Starts with Nasdaq TotalView-ITCH 5.0: replay a full trading day, rebuild every symbol's book, verify it, measure it. Built to take more venues later.

## Status

Early. Working today:

- Decodes the ITCH 5.0 file framing (2-byte big-endian length prefix per message)
- Counts message types across a trading day

Numbers will go here once they're measured on a verified full-day file.

## Roadmap

- [x] Toolchain: GCC 16 with C++26 reflection (`-freflection`)
- [x] Decode ITCH 5.0 framing
- [x] Count message types over a day
- [ ] Parse each message type into its own struct (reflection-generated big-endian decoders)
- [ ] Naive reference book
- [ ] Differential test: every book checked against the reference
- [ ] Fast book + benchmark table (throughput, per-message latency, perf counters)
- [ ] Venue-agnostic event layer, second venue: IEX DEEP
- [ ] Eurex EOBI (via Databento MBO)

## Design

An ITCH message tells you *which* order changed. It doesn't tell you where that order sits. An `Order Executed` carries an order ID and a share count, with no price and no side. So the book keeps two structures:

| Structure | Key | Answers |
|---|---|---|
| `orders` | order ID | where is order 42: side, price, shares left |
| `bids` / `asks` | price, sorted | what's the best bid/ask and how much is there |

Every update goes through both: find the order by ID, then adjust its price level. When an order or a level hits zero shares, it's removed. Otherwise the book shows liquidity that no longer exists.

## Build and run

Needs Linux and GCC 16+.

```bash
g++ -std=c++26 -O2 decode.cpp -o decode
zcat 12302019.NASDAQ_ITCH50.gz | ./decode
```

Data isn't in the repo. Nasdaq publishes free full-day sample files at
https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH/ (about 3.5 GB compressed each).

## References

- [Nasdaq TotalView-ITCH 5.0 specification](https://nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf)
