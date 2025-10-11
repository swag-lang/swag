/*
--- Part Two ---
The engineer finds the missing part and installs it in the engine! As the engine springs to life, you jump in the closest gondola, finally ready to ascend to the water source.

You don't seem to be going very fast, though. Maybe something is still wrong? Fortunately, the gondola has a phone labeled "help", so you pick it up and the engineer answers.

Before you can explain the situation, she suggests that you look out the window. There stands the engineer, holding a phone in one hand and waving with the other. You're going so slowly that you haven't even left the station. You exit the gondola.

The missing part wasn't the only issue - one of the gears in the engine is wrong. A gear is any * symbol that is adjacent to exactly two part numbers. Its gear ratio is the result of multiplying those two numbers together.

This time, you need to find the gear ratio of every gear and add them all up so that the engineer can figure out which gear needs to be replaced.

Consider the same engine schematic again:

467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..
In this schematic, there are two gears. The first is in the top left; it has part numbers 467 and 35, so its gear ratio is 16345. The second gear is in the lower right; its gear ratio is 451490. (The * adjacent to 617 is not a gear because it is only adjacent to one part number.) Adding up all of the gear ratios produces 467835.

What is the sum of all of the gear ratios in your engine schematic?
*/

using Core

private var symbols: Array'{ s, x, y: s32 }
private var nums: Array'{ num, x, lx, y: s32 }
private var parts: Array'{ num, x, lx, y: s32 }

private func hasSymbol(x, y: s32)->bool
{
    foreach s in symbols
    {
        if s.x == x and s.y == y do
            return true
    }

    return false
}

func day3B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "3.txt")
    let lines    = assume File.readAllLines(dataPath)

    foreach l, y in lines
    {
        for var x = 0; x < l.length; x += 1
        {
            var c = l[x]
            if c >= '0' and c <= '9'
            {
                let sx  = x
                var num = 0
                while x < l.length
                {
                    c = l[x]
                    if c < '0' or c > '9' do
                        break
                    num *= 10
                    num += c - '0'
                    x += 1
                }

                nums.add({num, sx, x - 1, cast(s32) y})
            }

            if c != '.' and x < l.length
            {
                symbols.add({c, x, cast(s32) y})
            }
        }
    }

    var total = 0'u64
    foreach n in nums
    {
        for var x = n.x; x <= n.lx; x += 1
        {
            if hasSymbol(x, n.y - 1) or
               hasSymbol(x, n.y + 1) or
               hasSymbol(x - 1, n.y - 1) or
               hasSymbol(x - 1, n.y) or
               hasSymbol(x - 1, n.y + 1) or
               hasSymbol(x + 1, n.y - 1) or
               hasSymbol(x + 1, n.y) or
               hasSymbol(x + 1, n.y + 1)
            {
                parts.add(n)
                break
            }
        }
    }

    foreach s in symbols
    {
        if s.s != '*' do
            continue

        var gear = 1
        var cpt  = 0
        foreach n in nums
        {
            if n.y < s.y - 1 or n.y > s.y + 1 do
                continue
            if (s.x >= n.x - 1 and s.x <= n.lx + 1)
            {
                gear *= n.num
                cpt += 1
            }
        }

        if cpt == 2
        {
            total += gear
        }
    }

    //Console.print("total: ", total)
    @assert(total == 93994191)
}
