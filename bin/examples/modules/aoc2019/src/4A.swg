/*
--- Day 4: Secure Container ---
You arrive at the Venus fuel depot only to discover it's protected by a password. The Elves had written the password on a sticky note, but someone threw it out.

However, they: remember a few key facts about the password:

It is a six-digit number.
The value is within the range given in your puzzle input.
Two adjacent digits are the same (like 22 in 122345).
Going from left to right, the digits never decrease; they only ever increase or stay the same (like 111123 or 135679).
Other than the range rule, the following are true:

111111 meets these criteria (double 11, never decreases).
223450 does not meet these criteria (decreasing pair of digits 50).
123789 does not meet these criteria (no double).
How many different passwords within the range given in your puzzle input meet these criteria?
*/

using Core

func day4A()
{
    day4A0()
    day4A1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day4A0()
{
    //Time.Stopwatch.scopeMeasure()

    var total = 0
    for x in 246515 to 739105
    {
        let str = Format.toString("%", x)

        var ok = false
        for i in str.length - 1
        {
            if str[i] > str[i + 1]
            {
                ok = false
                break
            }

            if str[i] == str[i + 1]
            {
                ok = true
            }
        }

        if ok do
            total += 1
    }

    //Console.print("total: ", total)
    @assert(total == 1048)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day4A1()
{
    //Time.Stopwatch.scopeMeasure()

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
                ok = true
            }
        }

        if ok do
            total += 1
    }

    //Console.print("total: ", total)
    @assert(total == 1048)
}
