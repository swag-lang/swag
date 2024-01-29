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
    var x, y, dx, dy: s32
    var l = 0

    const D = 312051

    var arr: HashTable'({x, y: s32}, s32)
    arr.add({0, 0}, 1)

    #[Swag.Macro]
    func sum(x, y: s32)->s32
    {
        var s: s32
        loop dx: -1 to 1
        {
            loop dy: -1 to 1
            {
                let it = #up arr.find({x+dx, y+dy})
                if !it do continue
                s += it.value
            }
        }
        return s
    }

    loop
    {
        const O = [{1, 1, 0}, {0, 0, -1}, {1, -1, 0}, {0, 0, 1}]
        visit v: O
        {
            l += v.item0; dx = v.item1; dy = v.item2
            loop l
            {
                x += dx; y += dy
                let s = sum(x, y)
                arr.add({x, y}, s)
                if s >= D
                {
                    @assert(s == 312453)
                    //Console.print("total: ", s)
                    return
                }
            }
        }
    }
}

