// https://adventofcode.com/ 
// Year 2023

using Core

private var symbols: Array'{x, y: s32}
private var nums:    Array'{num, x, lx, y: s32}

private func hasSymbol(x, y: s32)->bool
{
    visit s: symbols
    {
        if s.x == x and s.y == y
            return true
    }

    return false
}

func day3A()
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "3.txt")

    let lines = assume File.readAllLines(dataPath)

    visit l, y: lines
    {    
        for var x = 0; x < l.length; x += 1
        {
            var c = l[x]
            if c >= `0` and c <= `9`
            {
                let sx = x
                var num = 0
                while x < l.length
                {
                    c = l[x]
                    if c < `0` or c > `9`
                        break
                    num *= 10
                    num += c - `0`
                    x += 1
                }

                nums.add({num, sx, x-1, cast(s32) y})
            }

            if c != `.` and x < l.length
            {
                symbols.add({x, cast(s32) y})
            }
        }
    }

    var total = 0'u64
    visit n: nums
    {
        for var x = n.x; x <= n.lx; x += 1
        {
            if hasSymbol(x, n.y-1)   or
               hasSymbol(x, n.y+1)   or
               hasSymbol(x-1, n.y-1) or
               hasSymbol(x-1, n.y)   or
               hasSymbol(x-1, n.y+1) or
               hasSymbol(x+1, n.y-1) or
               hasSymbol(x+1, n.y)   or
               hasSymbol(x+1, n.y+1)
            {
                total += n.num
                break
            }
        }
    }

    //Console.print("total: ", total)
    @assert(total == 553825)
}

