/*
--- Part Two ---
Time to improve the polymer.

One of the unit types is causing problems; it's preventing the polymer from collapsing as much as it should. Your goal is to figure out which unit type is causing the most problems, remove all instances of it (regardless of polarity), fully react the remaining polymer, and measure its length.

For example, again using the polymer dabAcCaCBAcCcaDA from above:

Removing all A/a units produces dbcCCBcCcD. Fully reacting this polymer produces dbCBcD, which has length 6.
Removing all B/b units produces daAcCaCAcCcaDA. Fully reacting this polymer produces daCAcaDA, which has length 8.
Removing all C/c units produces dabAaBAaDA. Fully reacting this polymer produces daDA, which has length 4.
Removing all D/d units produces abAcCaCBAcCcaA. Fully reacting this polymer produces abCBAc, which has length 6.
In this example, removing all C/c units was best, producing the answer 4.

What is the length of the shortest polymer you can produce by removing all units of exactly one type and fully reacting the result?
*/

using Core

func day5B()
{
    day5B0()
    day5B1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day5B0()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "5.txt")
    let read     = assume File.readAllText(dataPath)

    var best = Swag.U64.Max
    for x in 26
    {
        var line = read
        var idx  = 0

        while idx < line.length - 1
        {
            while Latin1.toLower(line[idx]) == x + 'a' do
                line.remove(idx, 1)
            idx += 1
        }

        idx = 0
        while idx < line.length - 1
        {
            while Latin1.isLower(line[idx]) != Latin1.isLower(line[idx + 1]) and
                  Latin1.toLower(line[idx]) == Latin1.toLower(line[idx + 1])
            {
                line.remove(idx, 2)
                if idx >= line.length do
                    break
                if idx do
                    idx -= 1
            }

            idx += 1
        }

        best = Math.min(best, line.length)
    }

    //Console.print("total: ", best)
    @assert(best == 5122)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day5B1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "5.txt")
    let read     = assume File.readAllText(dataPath)

    var best = Swag.U64.Max
    foreach x in "abcdefghijklmnopqrstuvwxyz"
    {
        var line = read
        line.replace(cast(rune) x, "")
        line.replace(cast(rune) Latin1.toUpper(x), "")

        var result: String
        foreach c, i in line
        {
            if i and result.back() ^ c == 32 do
                result.removeBack()
            else do
                result += c
        }

        best = Math.min(best, result.length)
    }

    //Console.print("total: ", best)
    @assert(best == 5122)
}
