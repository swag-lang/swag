// https://adventofcode.com/ 
// Year 2023

using Core

var symbols: Array'{s, x, y: s32}
var nums:    Array'{num, x, lx, y: s32}
var parts:   Array'{num, x, lx, y: s32}

func hasSymbol(x, y: s32)->bool
{
    visit s: symbols
    {
        if s.x == x and s.y == y
            return true
    }

    return false
}

#test
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
                symbols.add({c, x, cast(s32) y})
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
                parts.add(n)
                break
            }
        }
    }

    visit s: symbols
    {
        if s.s != `*`
            continue

        var gear = 1
        var cpt = 0
        visit n: nums
        {
            if n.y < s.y-1 or n.y > s.y+1
                continue
            if (s.x >= n.x-1 and s.x <= n.lx+1)
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

