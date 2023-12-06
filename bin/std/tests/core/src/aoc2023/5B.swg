// https://adventofcode.com/ 
// Year 2023

using Core

enum Mode
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

struct Range
{
    src, srcE: u64
    dst: u64
}

var seeds:   Array'u64
var convert: [7] Array'Range

#test
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "5.txt")

    let lines = assume File.readAllLines(dataPath)

    var mode = Mode.Invalid
    visit l: lines
    {
        if l.isEmpty() continue

        switch l.toString()
        {
        case "seed-to-soil map:":             mode = .ToSoil; continue
        case "soil-to-fertilizer map:":       mode = .ToFertilizer; continue
        case "fertilizer-to-water map:":      mode = .ToWater; continue
        case "water-to-light map:":           mode = .ToLight; continue
        case "light-to-temperature map:":     mode = .ToTemperature; continue
        case "temperature-to-humidity map:":  mode = .ToHumidity; continue
        case "humidity-to-location map:":     mode = .ToLocation; continue    
        }

        if mode == .Invalid
        {
            let tokens = Tokenize.split(l, `:`)
            let values = Tokenize.split(tokens[1], ` `)
            visit v: values seeds.add(assume StrConv.toU64(v))
            continue
        }

        let values = Tokenize.split(l, ` `)
        var range: Range
        let rangeV = assume StrConv.toU64(values[2])
        range.dst = assume StrConv.toU64(values[0])
        range.src = assume StrConv.toU64(values[1])
        range.srcE = range.src + rangeV - 1
        convert[cast(s32) mode].add(range)
    }

    var ranges: Array'{start, end: u64}
    for var x = 0; x < seeds.count; x += 2
        ranges.add({seeds[x], seeds[x]+seeds[x+1]-1})

    loop i: 7
    {
        var newRanges: Array'{start, end: u64}
        for var idx = 0; idx < ranges.count; idx += 1
        {
            let range = ranges[idx]
            var done = false
            visit conv: convert[i]
            {
                if range.start > conv.srcE or range.end < conv.src
                    continue

                if range.start < conv.src
                    ranges.add({range.start, conv.src - 1})
                if range.end > conv.srcE
                    ranges.add({conv.srcE + 1, range.end})    

                let v0 = Math.max(range.start, conv.src)                   
                let v1 = Math.min(range.end, conv.srcE)
                newRanges.add({v0 - conv.src + conv.dst, v1 - conv.src + conv.dst})              
                done = true
                break
            }       

            if !done
                newRanges.add(range)
        }

        ranges = newRanges
    }

    var total = Swag.U64.Max
    visit r: ranges
        total = Math.min(total, r.start)

    Console.print("total: ", total)
    @assert(total == 69323688)
}

