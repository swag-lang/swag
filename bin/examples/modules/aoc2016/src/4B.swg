/*
--- Part Two ---
With all the decoy data out of the way, it's time to decrypt this list and get moving.

The room names are encrypted by a state-of-the-art shift cipher, which is nearly unbreakable without the right software. However, the information kiosk designers at Easter Bunny HQ were not expecting to deal with a master cryptographer like yourself.

To decrypt a room name, rotate each letter forward through the alphabet a number of times equal to the room's sector ID. A becomes B, B becomes C, Z becomes A, and so on. Dashes become spaces.

For example, the real name for qzmt-zixmtkozy-ivhz-343 is very encrypted name.

What is the sector ID of the room where North Pole objects are stored?
*/

using Core

func day4B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "4.txt")
    let lines    = assume File.readAllLines(dataPath)

    foreach l in lines
    {
        var idx  = 0
        var str: String
        while !Latin1.isDigit(l[idx])
        {
            if l[idx] != '-' do
                str += l[idx] - 'a'
            idx += 1
        }

        let res = assume StrConv.parseS64(l[idx to ])

        for i in str do
            str[i] = 'a' + cast(u8) ((str[i] + res.value) % 26)

        if Utf8.contains(str, "north")
        {
            //Console.print(str, " ", res.value, "\n")
            @assert(res.value == 482)
            return
        }
    }
}
