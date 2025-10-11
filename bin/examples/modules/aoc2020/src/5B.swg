/*
--- Part Two ---
Ding! The "fasten seat belt" signs have turned on. Time to find your seat.

It's a completely full flight, so your seat should be the only missing boarding pass in your list. However, there's a catch: some of the seats at the very front and back of the plane don't exist on this aircraft, so they'll be missing from your list as well.

Your seat wasn't at the very front or back, though; the seats with IDs +1 and -1 from yours will be in your list.

What is the ID of your seat?
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
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "5.txt")
    let lines    = assume File.readAllLines(dataPath)

    var here: [128, 8] bool

    foreach l in lines
    {
        var row0 = 0
        var row1 = 127
        for c in 7
        {
            if l[c] == 'F' do
                row1 = (row1 + row0) / 2
            else do
                row0 = 1 + (row1 + row0) / 2
        }
        var col0 = 0

        var col1 = 7
        for c in 3
        {
            if l[7 + c] == 'L' do
                col1 = (col1 + col0) / 2
            else do
                col0 = 1 + (col1 + col0) / 2
        }

        here[row0, col0] = true
    }

    for r in 128
    {
        var cpt = 0
        for c in 8 do
            cpt += cast(s32) here[r, c]

        if cpt == 7 // Search for one missing seat in the row
        {
            for c in 1 to 6 // Can't be on the side
            {
                if !here[r, c]
                {
                    //Console.print("total: ", r, " ", c, " ", r*8+c,"\n")
                    @assert(r * 8 + c == 714)
                    return
                }
            }
        }
    }
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day5B1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "5.txt")
    let lines    = assume File.readAllLines(dataPath)

    var seats: Array's32
    foreach l in lines
    {
        var row0 = 0
        var row1 = 127
        var col0 = 0
        var col1 = 7
        for c in 10
        {
            switch
            {
            case c < 7 and l[c] == 'F':
                row1 = (row1 + row0) / 2
            case c < 7:
                row0 = 1 + (row1 + row0) / 2
            case l[c] == 'L':
                col1 = (col1 + col0) / 2
            default:
                col0 = 1 + (col1 + col0) / 2
            }
        }

        seats.add(row0 * 8 + col0)
    }

    seats.sort()
    for i in seats
    {
        if seats[i + 1] - seats[i] != 1
        {
            //Console.print("total: ", seats[i]+1)
            @assert(seats[i] + 1 == 714)
            return
        }
    }
}
