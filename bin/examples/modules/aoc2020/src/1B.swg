/*
--- Part Two ---
The Elves in accounting are thankful for your help; one of them even offers you a starfish coin they had left over from a past vacation. They offer you a second one if you can find three numbers in your expense report that meet the same criteria.

Using the above example again, the three entries that sum to 2020 are 979, 366, and 675. Multiplying them together produces the answer, 241861950.

In your expense report, what is the product of the three entries that sum to 2020?
*/

using Core

func day1B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "1.txt")
    let lines    = assume File.readAllLines(dataPath)

    var arr: Array's32
    foreach v in lines do
        arr.add(assume StrConv.toS32(v))

    arr.sort()

    foreach v0, i0 in arr
    {
        foreach v1, i1 in arr[i0 to ]
        {
            let v2 = 2020 - (v0 + v1)
            if v2 < 0 do
                continue

            if Slice.containsSorted(arr[i1 to ], v2)
            {
                //Console.print("total: ", v0*v1*v2)
                @assert(v0 * v1 * v2 == 165795564)
                return
            }
        }
    }
}
