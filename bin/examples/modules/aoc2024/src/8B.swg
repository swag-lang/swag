/*
--- Part Two ---
Watching over your shoulder as you work, one of The Historians asks if you took the effects of resonant harmonics into your calculations.

Whoops!

After updating your model, it turns out that an antinode occurs at any grid position exactly in line with at least two antennas of the same frequency, regardless of distance. This means that some of the new antinodes will occur at the position of each antenna (unless that antenna is the only one of its frequency).

So, these three T-frequency antennas now create many antinodes:

T....#....
...T......
.T....#...
.........#
..#.......
..........
...#......
..........
....#.....
..........
In fact, the three T-frequency antennas are all exactly in line with two antennas, so they are all also antinodes! This brings the total number of antinodes in the above example to 9.

The original example now has 34 antinodes, including the antinodes that appear on every antenna:

##....#....#
.#.#....0...
..#.#0....#.
..##...0....
....0....#..
.#...#A....#
...#..#.....
#....#.#....
..#.....A...
....#....A..
.#........#.
...#......##
Calculate the impact of the signal using this updated model. How many unique locations within the bounds of the map contain an antinode?
*/

using Core

//#[Swag.PrintBc]
func day8B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "8.txt")
    let lines    = assume File.readAllLines(dataPath)
    let h        = lines.count
    let w        = lines[0].length

    var antennas:  Array'{ f: u8, x, y: s64 }
    var antinodes: HashSet'{ x, y: u32 }

    for l in lines
    {
        foreach f, c in lines[l] where f != '.'
        {
            antennas.add({f, c, l})
        }
    }

    foreach antenna0, a in antennas
    {
        foreach antenna1, b in antennas where a != b and antenna0.f == antenna1.f
        {
            let dx = antenna1.x - antenna0.x
            let dy = antenna1.y - antenna0.y
            var ax = antenna0.x
            var ay = antenna0.y
            while ax >= 0 and ay >= 0 and ax < w and ay < h
            {
                antinodes.add({cast(u32) ax, cast(u32) ay})
                ax -= dx
                ay -= dy
            }
        }
    }

    @assert(antinodes.count == 994)
}
