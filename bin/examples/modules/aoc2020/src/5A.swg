/*
--- Day 5: Binary Boarding ---
You board your plane only to discover a new problem: you dropped your boarding pass! You aren't sure which seat is yours, and all of the flight attendants are busy with the flood of people that suddenly made it through passport control.

You write a quick program to use your phone's camera to scan all of the nearby boarding passes (your puzzle input); perhaps you can find your seat through process of elimination.

Instead of zones or groups, this airline uses binary space partitioning to seat people. A seat might be specified like FBFBBFFRLR, where F means "front", B means "back", L means "left", and R means "right".

The first 7 characters will either be F or B; these specify exactly one of the 128 rows on the plane (numbered 0 through 127). Each letter tells you which half of a region the given seat is in. Start with the whole list of rows; the first letter indicates whether the seat is in the front (0 through 63) or the back (64 through 127). The next letter indicates which half of that region the seat is in, and so on until you're left with exactly one row.

For example, consider just the first seven characters of FBFBBFFRLR:

Start by considering the whole range, rows 0 through 127.
F means to take the lower half, keeping rows 0 through 63.
B means to take the upper half, keeping rows 32 through 63.
F means to take the lower half, keeping rows 32 through 47.
B means to take the upper half, keeping rows 40 through 47.
B keeps rows 44 through 47.
F keeps rows 44 through 45.
The final F keeps the lower of the two, row 44.
The last three characters will be either L or R; these specify exactly one of the 8 columns of seats on the plane (numbered 0 through 7). The same process as above proceeds again, this time with only three steps. L means to keep the lower half, while R means to keep the upper half.

For example, consider just the last 3 characters of FBFBBFFRLR:

Start by considering the whole range, columns 0 through 7.
R means to take the upper half, keeping columns 4 through 7.
L means to take the lower half, keeping columns 4 through 5.
The final R keeps the upper of the two, column 5.
So, decoding FBFBBFFRLR reveals that it is the seat at row 44, column 5.

Every seat also has a unique seat ID: multiply the row by 8, then add the column. In this example, the seat has ID 44 * 8 + 5 = 357.

Here are some other boarding passes:

BFFFBBFRRR: row 70, column 7, seat ID 567.
FFFBBBFRRR: row 14, column 7, seat ID 119.
BBFFBBFRLL: row 102, column 4, seat ID 820.
As a sanity check, look through your list of boarding passes. What is the highest seat ID on a boarding pass?
*/

using Core

func day5A()
{
    day5A0()
    day5A1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day5A0()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "5.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total: s32
    foreach l in lines
    {
        var row0 = 0
        var row1 = 127
        for c in 7
        {
            if l[c] == 'F':
                row1 = (row1 + row0) / 2
            else:
                row0 = 1 + (row1 + row0) / 2
        }
        var col0 = 0

        var col1 = 7
        for c in 3
        {
            if l[7 + c] == 'L':
                col1 = (col1 + col0) / 2
            else:
                col0 = 1 + (col1 + col0) / 2
        }

        total = Math.max(total, row0 * 8 + col0)
    }

    //Console.print("total: ", total)
    @assert(total == 838)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day5A1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "5.txt")

    var seats: Array'u64
    foreach l in assume File.readAllLines(dataPath)
    {
        l.replace("F", "0")
        l.replace("B", "1")
        l.replace("L", "0")
        l.replace("R", "1")
        seats.add(assume StrConv.parseB64(l).value)
    }

    seats.sort()
    @assert(seats.back() == 838)
}
