/*
--- Part Two ---
Your calculation isn't quite right. It looks like some of the digits are actually spelled out with letters: one, two, three, four, five, six, seven, eight, and nine also count as valid "digits".

Equipped with this new information, you now need to find the real first and last digit on each line. For example:

two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen
In this example, the calibration values are 29, 83, 13, 24, 42, 14, and 76. Adding these together produces 281.

What is the sum of all of the calibration values?
*/

using Core

private func process(line: String)->u64
{
    const N = ["one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]
    const V = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

    var bestStart = Swag.U64.Max
    var bestEnd   = 0'u64
    var bestStartValue, bestEndValue: u64

    foreach n, i in N
    {
        let start = Utf8.indexOf(line, n)
        if start != Swag.U64.Max and start < bestStart
        {
            bestStart      = start
            bestStartValue = i
        }

        let end = Utf8.lastIndexOf(line, n)
        if end != Swag.U64.Max and end >= bestEnd
        {
            bestEnd      = end
            bestEndValue = i
        }
    }

    return V[bestStartValue] * 10 + V[bestEndValue]
}

func day1B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "1.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total = 0'u64
    foreach l in lines
    {
        total += process(l)
    }

    //Console.print("total: ", total)
    @assert(total == 56324)
}
