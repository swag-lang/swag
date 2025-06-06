/*
--- Part Two ---
Now, you just need to figure out how many orbital transfers you (YOU) need to take to get to Santa (SAN).

You start at the object YOU are orbiting; your destination is the object SAN is orbiting. An orbital transfer lets you move from any object to an object orbiting or orbited by that object.

For example, suppose you have the following map:

COM)B
B)C
C)D
D)E
E)F
B)G
G)H
D)I
E)J
J)K
K)L
K)YOU
I)SAN
Visually, the above map of orbits looks like this:

                          YOU
                         /
        G - H       J - K - L
       /           /
COM - B - C - D - E - F
               \
                I - SAN
In this example, YOU are in orbit around K, and SAN is in orbit around I. To move from K to I, a minimum of 4 orbital transfers are required:

K to J
J to E
E to D
D to I
Afterward, the map of orbits looks like this:

        G - H       J - K - L
       /           /
COM - B - C - D - E - F
               \
                I - SAN
                 \
                  YOU
What is the minimum number of orbital transfers required to move from the object YOU are orbiting to the object SAN is orbiting? (Between the objects they are orbiting - not between YOU and SAN.)
*/

using Core

func day6B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var map: HashTable'(string, string)
    foreach l in lines
    {
        let split = Tokenize.split(l, ')''u8)
        map.add(split[1], split[0])
    }

    var you, san: Array'string
    var ity = map.find("YOU")
    var its = map.find("SAN")

    while ity
    {
        you.add(ity.value)
        ity = map.tryFind(ity.value)
    }

    var total: u64
    while its
    {
        let pos = Slice.findLinear(you.toSlice(), its.value)
        if pos != Swag.U64.Max
        {
            total = san.count + pos
            break
        }

        san.add(its.value)
        its = map.find(its.value)
    }

    //Console.print("total: ", total)
    @assert(total == 499)
}
