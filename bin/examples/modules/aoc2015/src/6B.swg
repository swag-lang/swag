/*
--- Part Two ---
You just finish implementing your winning light pattern when you realize you mistranslated Santa's message from Ancient Nordic Elvish.

The light grid you bought actually has individual brightness controls; each light can have a brightness of zero or more. The lights all start at zero.

The phrase turn on actually means that you should increase the brightness of those lights by 1.

The phrase turn off actually means that you should decrease the brightness of those lights by 1, to a minimum of zero.

The phrase toggle actually means that you should increase the brightness of those lights by 2.

What is the total brightness of all lights combined after following Santa's instructions?

For example:

turn on 0,0 through 0,0 would increase the total brightness by 1.
toggle 0,0 through 999,999 would increase the total brightness by 2000000.
*/

using Core

func day6B()
{
    //Time.Stopwatch.scopeMeasure()

    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var lights: Array's32
    lights.resize(1000 * 1000)

    var r = assume Parser.RegExp.create(#"\d+"#)
    foreach l in lines
    {
        let nums = r.grepAll(l)
        let x0   = assume StrConv.toU64(nums[0])
        let y0   = assume StrConv.toU64(nums[1])
        let x1   = assume StrConv.toU64(nums[2])
        let y1   = assume StrConv.toU64(nums[3])

        for x in x0 to x1
        {
            for y in y0 to y1
            {
                if l[6] == 'n' do
                    lights.ref2(x, y, 1000) += 1
                elif l[6] == 'f' and lights.ref2(x, y, 1000) do
                    lights.ref2(x, y, 1000) -= 1
                elif l[6] == ' ' do
                    lights.ref2(x, y, 1000) += 2
            }
        }
    }

    var total: s64
    foreach v in lights do
        total += v

    //Console.print("total: ", total)
    @assert(total == 14110788)
}
