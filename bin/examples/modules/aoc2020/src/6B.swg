/*
--- Part Two ---
As you finish the last group's customs declaration, you notice that you misread one word in the instructions:

You don't need to identify the questions to which anyone answered "yes"; you need to identify the questions to which everyone answered "yes"!

Using the same example as above:

abc

a
b
c

ab
ac

a
a
a
a

b
This list represents answers from five groups:

In the first group, everyone (all 1 person) answered "yes" to 3 questions: a, b, and c.
In the second group, there is no question to which everyone answered "yes".
In the third group, everyone answered yes to only 1 question, a. Since some people did not answer "yes" to b or c, they don't count.
In the fourth group, everyone answered yes to only 1 question, a.
In the fifth group, everyone (all 1 person) answered "yes" to 1 question, b.
In this example, the sum of these counts is 3 + 0 + 1 + 1 + 1 = 6.

For each group, count the number of questions to which everyone answered "yes". What is the sum of those counts?
*/

using Core

func day6B()
{
    day6B0()
    day6B1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day6B0()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var count: [26] s32
    var total, num: u64

    foreach l in lines
    {
        if l.length do
            num += 1

        foreach #Bytes c in l do
            count[c - 'a'] += 1

        if @index == lines.count - 1 or lines[@index + 1].length == 0
        {
            foreach c in count where c == num do
                total += 1
            @init(count)
            num = 0
        }
    }

    //Console.print("total: ", total)
    @assert(total == 3103)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day6B1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total:    u64
    var lineVal:  u32
    var interVal: u32 = ~0

    foreach l in lines
    {
        if l.length
        {
            lineVal = 0
            foreach #Bytes c in l do
                lineVal |= 1 << cast(u32) (c - 'a')
            interVal &= lineVal
        }

        if @index == lines.count - 1 or lines[@index + 1].length == 0
        {
            total += Math.countOnes(interVal)
            interVal = ~0
        }
    }

    //Console.print("total: ", total)
    @assert(total == 3103)
}
