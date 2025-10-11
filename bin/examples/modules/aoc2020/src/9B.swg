/*
--- Part Two ---
The final step in breaking the XMAS encryption relies on the invalid number you just found: you must find a contiguous set of at least two numbers in your list which sum to the invalid number from step 1.

Again consider the above example:

35
20
15
25
47
40
62
55
65
95
102
117
150
182
127
219
299
277
309
576
In this list, adding up all of the numbers from 15 through 40 produces the invalid number from step 1, 127. (Of course, the contiguous set of numbers in your actual list might be much longer.)

To find the encryption weakness, add together the smallest and largest number in this contiguous range; in this example, these are 15 and 47, producing 62.

What is the encryption weakness in your XMAS-encrypted list of numbers?
*/

using Core

func day9B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "9.txt")
    let lines    = assume File.readAllLines(dataPath)

    let target = 552655238
    foreach l, i in lines
    {
        var sum = 0
        var min = Swag.S32.Max
        var max = 0
        foreach l1 in lines[i to ]
        {
            let v1 = assume StrConv.toS32(l1)
            min = Math.min(min, v1)
            max = Math.max(max, v1)
            sum += v1
            if sum > target do
                break
            if sum == target
            {
                //Console.print("total: ", min+max)
                @assert(min + max == 70672245)
                return
            }
        }
    }
}
