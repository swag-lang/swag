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
    src, dst: u64
    range:    u64
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
        range.dst   = assume StrConv.toU64(values[0])
        range.src   = assume StrConv.toU64(values[1])
        range.range = assume StrConv.toU64(values[2])
        convert[cast(s32) mode].add(range)
    }

    //var total = 0'u64
    //Console.print("total: ", total)
    //@assert(total == 12263631)
}

