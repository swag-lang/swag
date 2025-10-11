/*
--- Day 4: Security Through Obscurity ---
Finally, you come across an information kiosk with a list of rooms. Of course, the list is encrypted and full of decoy data, but the instructions to decode the list are barely hidden nearby. Better remove the decoy data first.

Each room consists of an encrypted name (lowercase letters separated by dashes) followed by a dash, a sector ID, and a checksum in square brackets.

A room is real (not a decoy) if the checksum is the five most common letters in the encrypted name, in order, with ties broken by alphabetization. For example:

aaaaa-bbb-z-y-x-123[abxyz] is a real room because the most common letters are a (5), b (3), and then a tie between x, y, and z, which are listed alphabetically.
a-b-c-d-e-f-g-h-987[abcde] is a real room because although the letters are all tied (1 of each), the first five are listed alphabetically.
not-a-real-room-404[oarel] is a real room.
totally-real-room-200[decoy] is not.
Of the real rooms from the list above, the sum of their sector IDs is 1514.

What is the sum of the sector IDs of the real rooms?
*/

using Core

func day4A()
{
    assume
    {
        let dataPath = Path.combine(Path.getDirectoryName(#file), "4.txt")
        let lines    = File.readAllLines(dataPath)

        var total = 0's64
        foreach l in lines
        {
            var idx    = 0
            var count: [26] s32
            while Latin1.isLetter(l[idx]) or l[idx] == '-'
            {
                if l[idx] != '-' do
                    count[l[idx] - 'a'] += 1
                idx += 1
            }

            let res = StrConv.parseS64(l[idx to ])
            idx += res.eat

            var valid = true
            foreach c in l[idx + 1 to idx + 5]
            {
                var max, maxi = 0'u64
                for i in count where count[i] > max
                {
                    max  = count[i]
                    maxi = i
                }

                if c != maxi + 'a'
                {
                    valid = false
                    break
                }

                count[maxi] = 0
            }

            if valid do
                total += res.value
        }

        //Console.print("total: ", total)
        @assert(total == 361724)
    }
}
