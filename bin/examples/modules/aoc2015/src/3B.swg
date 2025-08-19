/*
--- Part Two ---
The next year, to speed up the process, Santa creates a robot version of himself, Robo-Santa, to deliver presents with him.

Santa and Robo-Santa start at the same location (delivering two presents to the same starting house), then take turns moving based on instructions from the elf, who is eggnoggedly reading from the same script as the previous year.

This year, how many houses receive at least one present?

For example:

^v delivers presents to 3 houses, because Santa goes north, and then Robo-Santa goes south.
^>v< now delivers presents to 3 houses, and Santa and Robo-Santa end up back where they started.
^v^v^v^v^v now delivers presents to 11 houses, with Santa going one direction and Robo-Santa going the other.
*/

using Core

func day3B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "3.txt")

    struct Pos
    {
        x, y: s32
        #[Swag.Offset("x")]
        v:   u64
    }

    var pos: [2] Pos
    var set: HashSet'u64
    foreach #Bytes c, i in assume File.readAllText(dataPath)
    {
        let who = i & 1
        switch c
        {
        case '^':
            pos[who].y -= 1
        case 'v':
            pos[who].y += 1
        case '<':
            pos[who].x -= 1
        case '>':
            pos[who].x += 1
        }

        set.add(pos[who].v)
    }

    //Console.print("total: ", set.count)
    @assert(set.count == 2631)
}
