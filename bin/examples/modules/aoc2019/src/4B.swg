/*
--- Part Two ---
An Elf just remembered one more important detail: the two adjacent matching digits are not part of a larger group of matching digits.

Given this additional criterion, but still ignoring the range rule, the following are now true:

112233 meets these criteria because the digits never decrease and all repeated digits are exactly two digits long.
123444 no longer meets the criteria (the repeated 44 is part of a larger group of 444).
111122 meets the criteria (even though 1 is repeated more than twice, it still contains a double 22).
How many different passwords within the range given in your puzzle input meet all of the criteria?
*/

using Core

func day4B()
{
    var total = 0
    for x in 246515 to 739105
    {
        var div  = 100_000
        var str: [6] u8 = undefined
        for i in 6
        {
            str[i] = cast(u8) ((x / div) % 10)
            div /= 10
        }

        var ok = false
        for i in 5
        {
            if str[i] > str[i + 1]
            {
                ok = false
                break
            }

            if str[i] == str[i + 1]
            {
                if !i or str[i - 1] != str[i]
                {
                    if i >= 4 or str[i + 1] != str[i + 2]
                    {
                        ok = true
                    }
                }
            }
        }

        if ok do
            total += 1
    }

    //Console.print("total: ", total)
    @assert(total == 677)
}
