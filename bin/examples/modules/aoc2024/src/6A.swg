/*
--- Day 6: Guard Gallivant ---
The Historians use their fancy device again, this time to whisk you all away to the North Pole prototype suit manufacturing lab... in the year 1518! It turns out that having direct access to history is very convenient for a group of historians.

You still have to be careful of time paradoxes, and so it will be important to avoid anyone from 1518 while The Historians search for the Chief. Unfortunately, a single guard is patrolling this part of the lab.

Maybe you can work out where the guard will go ahead of time so that The Historians can search safely?

You start by making a map (your puzzle input) of the situation. For example:

....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...
The map shows the current position of the guard with ^ (to indicate the guard is currently facing up from the perspective of the map). Any obstructions - crates, desks, alchemical reactors, etc. - are shown as #.

Lab guards in 1518 follow a very strict patrol protocol which involves repeatedly following these steps:

If there is something directly in front of you, turn right 90 degrees.
Otherwise, take a step forward.
Following the above protocol, the guard moves up several times until she reaches an obstacle (in this case, a pile of failed suit prototypes):

....#.....
....^....#
..........
..#.......
.......#..
..........
.#........
........#.
#.........
......#...
Because there is now an obstacle in front of the guard, she turns right before continuing straight in her new facing direction:

....#.....
........>#
..........
..#.......
.......#..
..........
.#........
........#.
#.........
......#...
Reaching another obstacle (a spool of several very long polymers), she turns right again and continues downward:

....#.....
.........#
..........
..#.......
.......#..
..........
.#......v.
........#.
#.........
......#...
This process continues for a while, but the guard eventually leaves the mapped area (after walking past a tank of universal solvent):

....#.....
.........#
..........
..#.......
.......#..
..........
.#........
........#.
#.........
......#v..
By predicting the guard's route, you can determine which specific positions in the lab will be in the patrol path. Including the guard's starting position, the positions visited by the guard before leaving the area are marked with an X:

....#.....
....XXXXX#
....X...X.
..#.X...X.
..XXXXX#X.
..X.X.X.X.
.#XXXXXXX.
.XXXXXXX#.
#XXXXXXX..
......#X..
In this example, the guard will visit 41 distinct positions on your map.

Predict the path of the guard. How many distinct positions will the guard visit before leaving the mapped area?
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
            if f != Swag.U64.Max do
                sx = cast() f
            else do
                sy += 1
        }

        arr.add(l.toSlice())
    }

    @assert(arr.count != 0)
    var dx = 0, dy = -1
    var total = 1
    for
    {
        if sx + dx < 0 or sx + dx >= w or sy + dy < 0 or sy + dy >= h do
            break

        if arr.ref2(sx + dx, sy + dy, w) == '#'
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

        if arr.ref2(sx, sy, w) != 'O'
        {
            total += 1
            arr.ref2(sx, sy, w) = 'O'
        }
    }

    @assert(total == 5461)
}
