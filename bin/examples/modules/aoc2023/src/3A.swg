/*
--- Day 3: Gear Ratios ---
You and the Elf eventually reach a gondola lift station; he says the gondola lift will take you up to the water source, but this is as far as he can bring you. You go inside.

It doesn't take long to find the gondolas, but there seems to be a problem: they're not moving.

"Aaah!"

You turn around to see a slightly-greasy Elf with a wrench and a look of surprise. "Sorry, I wasn't expecting anyone! The gondola lift isn't working right now; it'll still be a while before I can fix it." You offer to help.

The engineer explains that an engine part seems to be missing from the engine, but nobody can figure out which one. If you can add up all the part numbers in the engine schematic, it should be easy to work out which part is missing.

The engine schematic (your puzzle input) consists of a visual representation of the engine. There are lots of numbers and symbols you don't really understand, but apparently any number adjacent to a symbol, even diagonally, is a "part number" and should be included in your sum. (Periods (.): not count as a symbol.)

Here is an example engine schematic:

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
In this schematic, two numbers are not part numbers because they are not adjacent to a symbol: 114 (top right) and 58 (middle right). Every other number is adjacent to a symbol and so is a part number; their sum is 4361.

Of course, the actual engine schematic is much larger. What is the sum of all of the part numbers in the engine schematic?
*/

using Core

private var symbols: Array'{ x, y: s32 }
private var nums: Array'{ num, x, lx, y: s32 }

private func hasSymbol(x, y: s32)->bool
{
    return symbols.contains({x, y})
}

func day3A()
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
                symbols.add({x, cast(s32) y})
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
                total += n.num
                break
            }
        }
    }

    //Console.print("total: ", total)
    @assert(total == 553825)
}
