/*
--- Part Two ---
The elves are also running low on ribbon. Ribbon is all the same width, so they only have to worry about the length they need to order, which they would again like to be exact.

The ribbon required to wrap a present is the shortest distance around its sides, or the smallest perimeter of any one face. Each present also requires a bow made out of ribbon as well; the feet of ribbon required for the perfect bow is equal to the cubic feet of volume of the present. Don't ask how they tie the bow, though; they'll never tell.

For example:

A present with dimensions 2x3x4 requires 2+2+3+3 = 10 feet of ribbon to wrap the present plus 2*3*4 = 24 feet of ribbon for the bow, for a total of 34 feet.
A present with dimensions 1x1x10 requires 1+1+1+1 = 4 feet of ribbon to wrap the present plus 1*1*10 = 10 feet of ribbon for the bow, for a total of 14 feet.
How many total feet of ribbon should they order?
*/

using Core

func day2B()
{
    day2B0()
    day2B1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day2B0()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "2.txt")

    var total: u64
    assume
    {
        var r = Parser.RegExp.create(#"(\d+)x(\d+)x(\d+)"#)
        foreach line in File.readAllLines(dataPath)
        {
            discard r.match(line)
            var f: [3] s32
            f[0] = StrConv.toS32(r.getCapture(1))
            f[1] = StrConv.toS32(r.getCapture(2))
            f[2] = StrConv.toS32(r.getCapture(3))
            Slice.sort(f[ to ])
            total += f[0] * f[1] * f[2] + 2 * f[0] + 2 * f[1]
        }
    }

    //Console.print("total: ", total)
    @assert(total == 3783758)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day2B1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "2.txt")

    var total: u64
    assume
    {
        var r = Parser.RegExp.create(#"(\d+)x(\d+)x(\d+)"#)
        foreach line in File.readAllLines(dataPath)
        {
            discard r.match(line)
            let f = [StrConv.toS32(r.getCapture(1)), StrConv.toS32(r.getCapture(2)), StrConv.toS32(r.getCapture(3))]
            Slice.sort(f[ to ])
            total += f[0] * f[1] * f[2] + 2 * f[0] + 2 * f[1]
        }
    }

    //Console.print("total: ", total)
    @assert(total == 3783758)
}
