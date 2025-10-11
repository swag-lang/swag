/*
--- Part Two ---
Once you give them the coordinates, the Elves quickly deploy an Instant Monitoring Station to the location and discover the worst: there are simply too many asteroids.

The only solution is complete vaporization by giant laser.

Fortunately, in addition to an asteroid scanner, the new monitoring station also comes equipped with a giant rotating laser perfect for vaporizing asteroids. The laser starts by pointing up and always rotates clockwise, vaporizing any asteroid it hits.

If multiple asteroids are exactly in line with the station, the laser only has enough power to vaporize one of them before continuing its rotation. In other words, the same asteroids that can be detected can be vaporized, but if vaporizing one asteroid makes another one detectable, the newly-detected asteroid won't be vaporized until the laser has returned to the same position by rotating a full 360 degrees.

For example, consider the following map, where the asteroid with the new monitoring station (and laser) is marked X:

.#....#####...#..
##...##.#####..##
##...#...#.#####.
..#.....X...###..
..#.#.....#....##
The first nine asteroids to get vaporized, in order, would be:

.#....###24...#..
##...##.13#67..9#
##...#...5.8####.
..#.....X...###..
..#.#.....#....##
Note that some asteroids (the ones behind the asteroids marked 1, 5, and 7) won't have a chance to be vaporized until the next full rotation. The laser continues rotating; the next nine to be vaporized are:

.#....###.....#..
##...##...#.....#
##...#......1234.
..#.....X...5##..
..#.9.....8....76
The next nine to be vaporized are then:

.8....###.....#..
56...9#...#.....#
34...7...........
..2.....X....##..
..1..............
Finally, the laser completes its first full rotation (1 through 3), a second rotation (4 through 8), and vaporizes the last asteroid (9) partway through its third rotation:

......234.....6..
......1...5.....7
.................
........X....89..
.................
In the large example above (the one with the best monitoring station location at 11,13):

The 1st asteroid to be vaporized is at 11,12.
The 2nd asteroid to be vaporized is at 12,1.
The 3rd asteroid to be vaporized is at 12,2.
The 10th asteroid to be vaporized is at 12,8.
The 20th asteroid to be vaporized is at 16,0.
The 50th asteroid to be vaporized is at 16,9.
The 100th asteroid to be vaporized is at 10,16.
The 199th asteroid to be vaporized is at 9,6.
The 200th asteroid to be vaporized is at 8,2.
The 201st asteroid to be vaporized is at 10,9.
The 299th and final asteroid to be vaporized is at 11,1.
The Elves are placing bets on which will be the 200th asteroid to be vaporized. Win the bet by determining which asteroid that will be; what: you get if you multiply its X coordinate by 100 and then add its Y coordinate? (For example, 8,2 becomes 802.)
*/

using Core

func day10B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "10.txt")
    let lines    = assume File.readAllLines(dataPath)

    struct Pos
    {
        pos:       Math.Vector2
        angle:     f32
        dist:      f32
    }

    var arr: Array'Pos
    var tot: Array's32

    foreach l, y in lines do
        foreach c, x in l do
            if c == '#' do
                arr.add({{cast(f32) x, cast(f32) y}})
    tot.resize(arr.count)

    for i in arr
    {
        let v0 = arr[i].pos
        for j in i + 1 until arr.count
        {
            let v1  = arr[j].pos
            var hit = false
            for k in arr
            {
                if k == i or k == j do
                    continue
                let f = Math.Geometry.distanceToSegment(arr[k].pos, v0, v1)
                if Math.isZeroEpsilon(f)
                {
                    hit = true
                    break
                }
            }

            if !hit
            {
                tot[i] += 1
                tot[j] += 1
            }
        }
    }

    let idxStart = Slice.max(tot[ to ]).index
    let posStart = arr[idxStart].pos
    arr.removeAt(idxStart)

    // Precompute angle between 0 and 2Pi, and distance to the start position
    foreach &it in arr
    {
        let v = it.pos - posStart
        it.angle = v.angle0To2Pi({0, -1})
        it.dist  = v.length()
    }

    // Sort from small angle to big angle. If angles are the same, sort by distance (closest first)
    arr.sort(func(x, y)
    {
        if Math.isEqualEpsilon(x.angle, y.angle) do
            return x.dist <=> y.dist
        return x.angle <=> y.angle
    })

    var total, index = 0'u64
    for
    {
        total += 1

        if total == 200
        {
            let result = (arr[index].pos.x * 100) + arr[index].pos.y
            @assert(result == 502)
            break
        }

        let curAngle = arr[index].angle
        arr.removeAtOrdered(index)

        var nindex = index
        while Math.isEqualEpsilon(arr[nindex].angle, curAngle) do
            nindex = (nindex + 1) % arr.count
        index = nindex
    }
}
