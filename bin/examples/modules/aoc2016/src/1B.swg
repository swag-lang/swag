/*
--- Part Two ---
Then, you notice the instructions continue on the back of the Recruiting Document. Easter Bunny HQ is actually at the first location you foreach twice.

For example, if your instructions are R8, R4, R4, R8, the first location you foreach twice is 4 blocks away, due East.

How many blocks away is the first location you foreach twice?
*/

using Core

func day1B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "1.txt")
    let lines    = assume File.readAllLines(dataPath)

    let split = Tokenize.split(lines[0], ',''u8)
    var dx    = 0
    var dy    = 1
    var done: HashSet'u64

    struct Pos
    {
        x, y: s32
        #[Swag.Offset("x")]
        crc: u64
    }

    var pos: Pos

    foreach it in split
    {
        let v = Latin1.trim(it)
        let n = assume StrConv.toS32(v[1 to ])
        switch v[0]
        {
        case 'R':
            swap(&dx, &dy)
            dy = -dy
        case 'L':
            swap(&dx, &dy)
            dx = -dx
        }

        for n
        {
            pos.x += dx
            pos.y += dy
            if done.contains(pos.crc)
            {
                let total = Math.abs(pos.x) + Math.abs(pos.y)
                //Console.print("total: ", total)
                @assert(total == 150)
                return
            }

            done.add(pos.crc)
        }
    }
}
