/*
--- Day 8: I Heard You Like Registers ---
You receive a signal directly from the CPU. Because of your recent assistance with jump instructions, it would like you to compute the result of a series of unusual register instructions.

Each instruction consists of several parts: the register to modify, whether to increase or decrease that register's value, the amount by which to increase or decrease it, and a condition. If the condition fails, skip the instruction without modifying the register. The registers all start at 0. The instructions look like this:

b inc 5 if a > 1
a inc 1 if b < 5
c dec -10 if a >= 1
c inc -20 if c == 10
These instructions would be processed as follows:

Because a starts at 0, it is not greater than 1, and so b is not modified.
a is increased by 1 (to 1) because b is less than 5 (it is 0).
c is decreased by -10 (to 10) because a is now greater than or equal to 1 (it is 1).
c is increased by -20 (to -10) because c is equal to 10.
After this process, the largest value in any register is 1.

You might also encounter <= (less than or equal to) or != (not equal to). However, the CPU doesn't have the bandwidth to tell you what all the registers are named, and leaves that to you to determine.

What is the largest value in any register after completing the instructions in your puzzle input?
*/

using Core

func day8A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "8.txt")
    let lines    = assume File.readAllLines(dataPath)

    var re     = assume Parser.RegExp.create(#"(\w+) (\w+) (-?\d+) if (\w+) ([[:ascii:]]+) (-?\d+)"#)
    var regs:  HashTable'(string, s32)
    var total: s32

    foreach l in lines
    {
        @assert(re.match(l))
        var v0 = assume StrConv.toS32(re[3])
        let v1 = assume StrConv.toS32(re[6])
        if !regs.contains(re[1]) do
            regs.add(re[1], 0)
        if !regs.contains(re[4]) do
            regs.add(re[4], 0)
        if re[2] == "dec" do
            v0 = -v0

        let test  = regs[re[4]]
        var valid = false
        switch re[5]
        {
        case "==":
            valid = test == v1
        case "!=":
            valid = test != v1
        case ">=":
            valid = test >= v1
        case "<=":
            valid = test <= v1
        case ">":
            valid = test > v1
        case "<":
            valid = test < v1
        }

        if valid do
            regs.find(re[1]).value += v0
    }

    foreach k, v in regs do
        total = Math.max(total, v)
    @assert(total == 4066)
}
