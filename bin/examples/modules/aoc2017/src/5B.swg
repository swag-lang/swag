/*
--- Part Two ---
Now, the jumps are even stranger: after each jump, if the offset was three or more, instead decrease it by 1. Otherwise, increase it by 1 as before.

Using this rule with the above example, the process now takes 10 steps, and the offset values after finding the exit are left as 2 3 2 3 -1.

How many steps does it now take to reach the exit?
*/

using Core

//#[Swag.PrintBc]
func day5B()
{
    //Time.Stopwatch.scopeMeasure()

    let dataPath = Path.combine(Path.getDirectoryName(#file), "5.txt")
    let lines    = assume File.readAllLines(dataPath)
    var jumps    = Slice.map(lines[ to ], func(x) => assume StrConv.toS64(x))

    var it, step = 0's64
    while it < jumps.count
    {
        let jit = #ref jumps.ref1(it)
        it += jit
        jit += -1 + 2 * cast(s64) (jit < 3)
        step += 1
    }

    @assert(step == 27688760)
}
