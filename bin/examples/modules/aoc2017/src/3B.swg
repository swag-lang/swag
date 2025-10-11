/*
--- Part Two ---
As a stress test on the system, the programs here clear the grid and then store the value 1 in square 1. Then, in the same allocation order as shown above, they store the sum of the values in all adjacent squares, including diagonals.

So, the first few squares' values are chosen as follows:

Square 1 starts with the value 1.
Square 2 has only one adjacent filled square (with value 1), so it also stores 1.
Square 3 has both of the above squares as neighbors and stores the sum of their values, 2.
Square 4 has all three of the aforementioned squares as neighbors and stores the sum of their values, 4.
Square 5 only has the first and fourth squares as neighbors, so it gets the value 5.
Once a square is written, its value does not change. Therefore, the first few squares would receive the following values:

147  142  133  122   59
304    5    4    2   57
330   10    1    1   54
351   11   23   25   26
362  747  806--->   ...
What is the first value written that is larger than your puzzle input?
*/

using Core

func day3B()
{
    const D = 312051

    var arr: HashTable'({ x, y: s32 }, s32)
    arr[{0, 0}] = 1

    func sum(x, y: s32, a: #decltype(arr))->s32
    {
        var s: s32
        for dx in -1 to 1 do
            for dy in -1 to 1 do
                s += a.get({x + dx, y + dy}, 0)
        return s
    }

    var x, y: s32
    var l = 0

    for
    {
        const O = [{dl: 1, dx: 1, dy: 0}, {0, 0, -1}, {1, -1, 0}, {0, 0, 1}]
        foreach v in O
        {
            l += v.dl
            for l
            {
                x += v.dx
                y += v.dy
                let s = sum(x, y, arr)
                if s >= D
                {
                    //Console.print("total: ", s)
                    @assert(s == 312453)
                    return
                }

                arr[{x, y}] = s
            }
        }
    }
}
