/*
--- Part Two ---
The Elf looks quizzically at you. Did you misunderstand the assignment?

Looking for the instructions, you flip over the word search to find that this isn't actually an XMAS puzzle; it's an X-MAS puzzle in which you're supposed to find two MAS in the shape of an X. One way to achieve that is like this:

M.S
.A.
M.S
Irrelevant characters have again been replaced with . in the above diagram. Within the X, each MAS can be written forwards or backwards.

Here's the same example from before, but this time all of the X-MASes have been kept instead:

.M.S......
..A..MSMS.
.M.S.MAA..
..A.ASMSM.
.M.S.M....
..........
S.S.S.S.S.
.A.A.A.A..
M.M.M.M.M.
..........
In this example, an X-MAS appears 9 times.

Flip the word search from the instructions back over to the word search side and try again. How many times does an X-MAS appear?
*/

using Core

func day4B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "4.txt")
    let lines    = assume File.readAllLines(dataPath)

    func test(a, b, c, d, e: rune)->bool
    {
        var cpt = 0
        if a == 'M' and c == 'A' and e == 'S' do
            cpt += 1
        elif a == 'S' and c == 'A' and e == 'M' do
            cpt += 1

        if b == 'M' and c == 'A' and d == 'S' do
            cpt += 1
        elif b == 'S' and c == 'A' and d == 'M' do
            cpt += 1

        return cpt == 2
    }

    var total = 0
    for y in 0 until lines.count - 2
    {
        for x in 0 until lines[y].length - 2
        {
            if test(lines[y][x], lines[y][x + 2], lines[y + 1][x + 1], lines[y + 2][x], lines[y + 2][x + 2]) do
                total += 1
        }
    }

    @assert(total == 1939)
}
