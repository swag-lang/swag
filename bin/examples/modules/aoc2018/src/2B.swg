/*
--- Part Two ---
Confident that your list of box IDs is complete, you're ready to find the boxes full of prototype fabric.

The boxes will have IDs which differ by exactly one character at the same position in both strings. For example, given the following box IDs:

abcde
fghij
klmno
pqrst
fguij
axcye
wvxyz
The IDs abcde and axcye are close, but they differ by two characters (the second and fourth). However, the IDs fghij and fguij differ by exactly one character, the third (h and u). Those must be the correct boxes.

What letters are common between the two correct box IDs? (In the example above, this is found by removing the differing character from either ID, producing fgij.)
*/

using Core

func day2B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "2.txt")
    let lines    = assume File.readAllLines(dataPath)

    foreach a, i in lines
    {
        foreach b, j in lines
        {
            if i == j:
                continue
            var diff    = 0
            var result: String
            for k in b
            {
                if b[k] != a[k]:
                    diff += 1
                else:
                    result += b[k]
                if diff > 1:
                    break
            }

            if diff == 1
            {
                //Console.print("result ", result)
                @assert(result == "nvosmkcdtdbfhyxsphzgraljq")
                return
            }
        }
    }
}
