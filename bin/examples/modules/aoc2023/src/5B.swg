/*
--- Part Two ---
Everyone will starve if you only plant such a small number of seeds. Re-reading the almanac, it looks like the seeds: line actually describes ranges of seed numbers.

The values on the initial seeds: line come in pairs. Within each pair, the first value is the start of the range and the second value is the length of the range. So, in the first line of the example above:

seeds: 79 14 55 13
This line describes two ranges of seed numbers to be planted in the garden. The first range starts with seed number 79 and contains 14 values: 79, 80, ..., 91, 92. The second range starts with seed number 55 and contains 13 values: 55, 56, ..., 66, 67.

Now, rather than considering four seed numbers, you need to consider a total of 27 seed numbers.

In the above example, the lowest location number can be obtained from seed number 82, which corresponds to soil 84, fertilizer 84, water 84, light 77, temperature 45, humidity 46, and location 46. So, the lowest location number is 46.

Consider all of the initial seed numbers listed in the ranges on the first line of the almanac. What is the lowest location number that corresponds to any of the initial seed numbers?
*/

using Core

private enum Mode
{
    ToSoil
    ToFertilizer
    ToWater
    ToLight
    ToTemperature
    ToHumidity
    ToLocation
    Invalid
}

private struct Range
{
    src, srcE:     u64
    dst:           u64
}

private var seeds: Array'u64
private var convert: [7] Array'Range

func day5B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "5.txt")
    let lines    = assume File.readAllLines(dataPath)

    var mode = Mode.Invalid
    foreach l in lines
    {
        if l.isEmpty() do
            continue

        switch l.toString()
        {
        case "seed-to-soil map:":
            mode = .ToSoil
            continue
        case "soil-to-fertilizer map:":
            mode = .ToFertilizer
            continue
        case "fertilizer-to-water map:":
            mode = .ToWater
            continue
        case "water-to-light map:":
            mode = .ToLight
            continue
        case "light-to-temperature map:":
            mode = .ToTemperature
            continue
        case "temperature-to-humidity map:":
            mode = .ToHumidity
            continue
        case "humidity-to-location map:":
            mode = .ToLocation
            continue
        }

        if mode == .Invalid
        {
            let tokens = Tokenize.split(l, ':''u8)
            let values = Tokenize.split(tokens[1], ' ''u8)
            foreach v in values do
                seeds.add(assume StrConv.toU64(v))
            continue
        }

        let values = Tokenize.split(l, ' ''u8)
        var range: Range
        let rangeV = assume StrConv.toU64(values[2])
        range.dst  = assume StrConv.toU64(values[0])
        range.src  = assume StrConv.toU64(values[1])
        range.srcE = range.src + rangeV - 1
        convert[cast(s32) mode].add(range)
    }

    var ranges: Array'{ start, end: u64 }
    for var x = 0; x < seeds.count; x += 2 do
        ranges.add({seeds[x], seeds[x] + seeds[x + 1] - 1})

    for i in 7
    {
        var newRanges: Array'{ start, end: u64 }
        for var idx = 0; idx < ranges.count; idx += 1
        {
            let range = ranges[idx]
            #scope(Done)
            {
                foreach conv in convert[i]
                {
                    if range.start > conv.srcE or range.end < conv.src do
                        continue

                    if range.start < conv.src do
                        ranges.add({range.start, conv.src - 1})
                    if range.end > conv.srcE do
                        ranges.add({conv.srcE + 1, range.end})

                    let v0 = Math.max(range.start, conv.src)
                    let v1 = Math.min(range.end, conv.srcE)
                    newRanges.add({v0 - conv.src + conv.dst, v1 - conv.src + conv.dst})
                    break to Done
                }

                newRanges.add(range)
            }
        }

        ranges = newRanges
    }

    var total = Swag.U64.Max
    foreach r in ranges do
        total = Math.min(total, r.start)

    //Console.print("total: ", total)
    @assert(total == 69323688)
}
