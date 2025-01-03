/*
--- Part Two ---
While the Elves get to work printing the correctly-ordered updates, you have a little time to fix the rest of them.

For each of the incorrectly-ordered updates, use the page ordering rules to put the page numbers in the right order. For the above example, here are the three incorrectly-ordered updates and their correct orderings:

75,97,47,61,53 becomes 97,75,47,61,53.
61,13,29 becomes 61,29,13.
97,13,75,29,47 becomes 97,75,47,29,13.
After taking only the incorrectly-ordered updates and ordering them correctly, their middle page numbers are 47, 29, and 47. Adding these together produces 123.

Find the updates which are not in the correct order. What do you get if you add up the middle page numbers after correctly ordering just those updates?
*/

using Core

func day6A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var arr: Array'u8
    var sx, sy: s32
    let w = lines[0].length
    let h = lines.count

    foreach l in lines
    {
        if !sx
        {
            let f = l.indexOf("^")
            if f != Swag.U64.Max:
                sx = cast() f
            else:
                sy += 1
        }

        arr.add(l.toSlice())
    }


    var dx = 0, dy = -1
    var total = 1
    for
    {
        if sx+dx < 0 or sx+dx >= w or sy+dy < 0 or sy+dy >= h:
            break

        if arr.ref2(sx+dx,sy+dy,w) == '#'
        {
            if dx == 0 and dy == -1
            {
                dx = 1
                dy = 0
            }
            elif dx == 1 and dy == 0
            {
                dx = 0
                dy = 1
            }  
            elif dx == 0 and dy == 1
            {
                dx = -1
                dy = 0
            }  
            else
            {
                dx = 0
                dy = -1
            }    

            continue                
        }

        sx += dx
        sy += dy

        if arr.ref2(sx,sy,w) != 'O'
        {
            total += 1
            arr.ref2(sx,sy,w) = 'O'
        }
    }

    @assert(total == 5461)
}
