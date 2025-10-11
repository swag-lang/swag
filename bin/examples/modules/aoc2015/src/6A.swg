/*
--- Day 6: Probably a Fire Hazard ---
Because your neighbors keep defeating you in the holiday house decorating contest year after year, you've decided to deploy one million lights in a 1000x1000 grid.

Furthermore, because you've been especially nice this year, Santa has mailed you instructions on how to display the ideal lighting configuration.

Lights in your grid are numbered from 0 to 999 in each direction; the lights at each corner are at 0,0, 0,999, 999,999, and 999,0. The instructions include whether to turn on, turn off, or toggle various inclusive ranges given as coordinate pairs. Each coordinate pair represents opposite corners of a rectangle, inclusive; a coordinate pair like 0,0 through 2,2 therefore refers to 9 lights in a 3x3 square. The lights all start turned off.

To defeat your neighbors this year, all you have to: is set up your lights by doing the instructions Santa sent you in order.

For example:

turn on 0,0 through 999,999 would turn on (or leave on) every light.
toggle 0,0 through 999,0 would toggle the first line of 1000 lights, turning off the ones that were on, and turning on the ones that were off.
turn off 499,499 through 500,500 would turn off (or leave off) the middle four lights.
After following the instructions, how many lights are lit?
*/

using Core

func day6A()
{
    day6A0()
    day6A1()
    day6A2()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day6A0()
{
    //Time.Stopwatch.scopeMeasure()

    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var lights: HashSet'u64

    var r0 = assume Parser.RegExp.create(#"turn on (\d+),(\d+) through (\d+),(\d+)"#)
    var r1 = assume Parser.RegExp.create(#"turn off (\d+),(\d+) through (\d+),(\d+)"#)
    var r2 = assume Parser.RegExp.create(#"toggle (\d+),(\d+) through (\d+),(\d+)"#)

    foreach l in lines
    {
        if r0.match(l)
        {
            for x in cast(u64) assume StrConv.toS32(r0.getCapture(1)) to cast(u64) assume StrConv.toS32(r0.getCapture(3)) do
                for y in assume StrConv.toS32(r0.getCapture(2)) to assume StrConv.toS32(r0.getCapture(4)) do
                    lights.add((x << 32) | y)
        }
        elif r1.match(l)
        {
            for x in cast(u64) assume StrConv.toS32(r1.getCapture(1)) to cast(u64) assume StrConv.toS32(r1.getCapture(3)) do
                for y in assume StrConv.toS32(r1.getCapture(2)) to assume StrConv.toS32(r1.getCapture(4)) do
                    lights.remove((x << 32) | y)
        }
        elif r2.match(l)
        {
            for x in cast(u64) assume StrConv.toS32(r2.getCapture(1)) to cast(u64) assume StrConv.toS32(r2.getCapture(3)) do
                for y in assume StrConv.toS32(r2.getCapture(2)) to assume StrConv.toS32(r2.getCapture(4)) do
                    if lights.contains((x << 32) | y) do
                        lights.remove((x << 32) | y)
                    else do
                        lights.add((x << 32) | y)
        }
    }

    //Console.print("total: ", lights.count)
    @assert(lights.count == 377891)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day6A1()
{
    //Time.Stopwatch.scopeMeasure()

    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var lights: Array'bool
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
                    lights.ref2(x, y, 1000) = true
                elif l[6] == 'f' do
                    lights.ref2(x, y, 1000) = false
                else do
                    lights.ref2(x, y, 1000) = !lights.ref2(x, y, 1000)
            }
        }
    }

    var total: u64
    foreach v in lights do
        total += cast(s32) v

    //Console.print("total: ", total)
    @assert(total == 377891)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day6A2()
{
    //Time.Stopwatch.scopeMeasure()

    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var lights: BitArray
    lights.reserve(1000 * 1000'u64, false)

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
                let coord = x + (y * 1000)
                if l[6] == 'n' do
                    lights.set(coord, true)
                elif l[6] == 'f' do
                    lights.set(coord, false)
                else do
                    lights.invert(coord)
            }
        }
    }

    var total: u64
    foreach v in lights do
        total += cast(s32) v

    //Console.print("total: ", total)
    @assert(total == 377891)
}
