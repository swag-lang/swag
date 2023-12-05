// https://adventofcode.com/ 
// Year 2023

using Core

func process(line: String)->u64
{
    const N = ["one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]
    const V = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

    var bestStart = Swag.U64.Max
    var bestEnd = 0'u64
    var bestStartValue, bestEndValue: u64

    visit n, i: N
    {
        let start = Utf8.indexOf(line, n)
        if start != Swag.U64.Max and start < bestStart
        {
            bestStart = start
            bestStartValue = i
        }

        let end = Utf8.lastIndexOf(line, n)
        if end != Swag.U64.Max and end >= bestEnd
        {
            bestEnd = end
            bestEndValue = i
        }
    }

    return V[bestStartValue]*10 + V[bestEndValue]
}

#test
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "1.txt")

    let lines = assume File.readAllLines(dataPath)
    var total = 0'u64
    visit l: lines
    {
        total += process(l)
    }

    //Console.print("total: ", total)
    @assert(total == 56324)
}

