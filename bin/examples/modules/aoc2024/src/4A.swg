/*
--- Day 4: Ceres Search ---
"Looks like the Chief's not here. Next!" One of The Historians pulls out a device and pushes the only button on it. After a brief flash, you recognize the interior of the Ceres monitoring station!

As the search for the Chief continues, a small Elf who lives on the station tugs on your shirt; she'd like to know if you could help her with her word search (your puzzle input). She only has to find one word: XMAS.

This word search allows words to be horizontal, vertical, diagonal, written backwards, or even overlapping other words. It's a little unusual, though, as you don't merely need to find one instance of XMAS - you need to find all of them. Here are a few ways XMAS might appear, where irrelevant characters have been replaced with .:


..X...
.SAMX.
.A..A.
XMAS.S
.X....
The actual word search will be full of letters instead. For example:

MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX
In this word search, XMAS occurs a total of 18 times; here's the same word search again, but where letters not involved in any XMAS have been replaced with .:

....XXMAS.
.SAMXMS...
...S..A...
..A.A.MS.X
XMASAMX.MM
X.....XA.A
S.S.S.S.SS
.A.A.A.A.A
..M.M.M.MM
.X.X.XMASX
Take a look at the little Elf's word search. How many times does XMAS appear?
*/

using Core

func day4A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "4.txt")
    let lines    = assume File.readAllLines(dataPath)

    func test(a, b, c, d: rune)->bool
    {
        if a == 'X' and b == 'M' and c == 'A' and d == 'S' do
            return true
        if a == 'S' and b == 'A' and c == 'M' and d == 'X' do
            return true
        return false
    }

    var total = 0
    for y in lines
    {
        for x in lines[y]
        {
            if x <= lines[y].length - 4 and
               test(lines[y][x], lines[y][x + 1], lines[y][x + 2], lines[y][x + 3]) do
                total += 1
            if y <= lines.count - 4 and
               test(lines[y][x], lines[y + 1][x], lines[y + 2][x], lines[y + 3][x]) do
                total += 1

            if y >= 3 and x <= lines[y].length - 4 and
               test(lines[y][x], lines[y - 1][x + 1], lines[y - 2][x + 2], lines[y - 3][x + 3]) do
                total += 1
            if y <= lines.count - 4 and x <= lines[y].length - 4 and
               test(lines[y][x], lines[y + 1][x + 1], lines[y + 2][x + 2], lines[y + 3][x + 3]) do
                total += 1
        }
    }

    @assert(total == 2547)
}
