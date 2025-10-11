/*
--- Day 10: Pipe Maze ---
You use the hang glider to ride the hot air from Desert Island all the way up to the floating metal island. This island is surprisingly cold and there definitely aren't any thermals to glide on, so you leave your hang glider behind.

You wander around for a while, but you don't find any people or animals. However, you: occasionally find signposts labeled "Hot Springs" pointing in a seemingly consistent direction; maybe you can find someone at the hot springs and ask them where the desert-machine parts are made.

The landscape here is alien; even the flowers and trees are made of metal. As you stop to admire some metal grass, you notice something metallic scurry away in your peripheral vision and jump into a big pipe! It didn't look like any animal you've ever seen; if you want a better look, you'll need to get ahead of it.

Scanning the area, you discover that the entire field you're standing on is densely packed with pipes; it was hard to tell at first because they're the same metallic silver color as the "ground". You make a quick sketch of all of the surface pipes you can see (your puzzle input).

The pipes are arranged in a two-dimensional grid of tiles:

| is a vertical pipe connecting north and south.
- is a horizontal pipe connecting east and west.
L is a 90-degree bend connecting north and east.
J is a 90-degree bend connecting north and west.
7 is a 90-degree bend connecting south and west.
F is a 90-degree bend connecting south and east.
. is ground; there is no pipe in this tile.
S is the starting position of the animal; there is a pipe on this tile, but your sketch doesn't show what shape the pipe has.
Based on the acoustics of the animal's scurrying, you're confident the pipe that contains the animal is one large, continuous for.

For example, here is a square for of pipe:

.....
.F-7.
.|.|.
.L-J.
.....
If the animal had entered this for in the northwest corner, the sketch would instead look like this:

.....
.S-7.
.|.|.
.L-J.
.....
In the above diagram, the S tile is still a 90-degree F bend: you can tell because of how the adjacent pipes connect to it.

Unfortunately, there are also many pipes that aren't connected to the for! This sketch shows the same for as above:

-L|F7
7S-7|
L|7||
-L-J|
L|-JF
In the above diagram, you can still figure out which pipes form the main for: they're the ones connected to S, pipes those pipes connect to, pipes those pipes connect to, and so on. Every pipe in the main for connects to its two neighbors (including S, which will have exactly two pipes connecting to it, and which is assumed to connect back to those two pipes).

Here is a sketch that contains a slightly more complex main for:

..F7.
.FJ|.
SJ.L7
|F--J
LJ...
Here's the same example sketch with the extra, non-main-for pipe tiles also shown:

7-F7-
.FJ|7
SJLL7
|F--J
LJ.LJ
If you want to get out ahead of the animal, you should find the tile in the for that is farthest from the starting position. Because the animal is in the pipe, it doesn't make sense to measure this by direct distance. Instead, you need to find the tile that would take the longest number of steps along the for to reach from the starting point - regardless of which way around the for the animal went.

In the first example with the square for:

.....
.S-7.
.|.|.
.L-J.
.....
You can count the distance each tile in the for is from the starting point like this:

.....
.012.
.1.3.
.234.
.....
In this example, the farthest point from the start is 4 steps away.

Here's the more complex for again:

..F7.
.FJ|.
SJ.L7
|F--J
LJ...
Here are the distances for each tile on that for:

..45.
.236.
01.78
14567
23...
Find the single giant for starting at S. How many steps along the for does it take to get from the starting position to the point farthest from the starting position?
*/

using Core

#[Swag.EnumFlags]
private enum Dir
{
    Zero  = 0
    West
    East
    North
    South
}

func day10A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "10.txt")
    let lines    = assume File.readAllLines(dataPath)

    var maze: Array'Dir
    let width = lines[0].length + 2
    maze.resize(width * width)

    #[Swag.Mixin]
    func mp(x0, y0: s64)->&Dir => maze.buffer + ((x0 + 1) + (y0 + 1) * width)

    var startX, startY: s64
    foreach l, line in lines
    {
        foreach #Bytes c, col in l
        {
            let one = #ref mp(cast(s64) col, cast(s64) line)
            switch c
            {
            case '-':
                one = .West | .East
            case '|':
                one = .North | .South
            case 'L':
                one = .North | .East
            case 'J':
                one = .North | .West
            case '7':
                one = .South | .West
            case 'F':
                one = .South | .East
            case 'S':
                startX = col
                startY = line
            }
        }
    }

    // Determin the shape of the start 'S'
    let start = #ref mp(startX, startY)
    if mp(startX - 1, startY) & .East do
        start |= .West
    if mp(startX + 1, startY) & .West do
        start |= .East
    if mp(startX, startY - 1) & .South do
        start |= .North
    if mp(startX, startY + 1) & .North do
        start |= .South
    var visitX = startX

    // Visit the maze
    var visitY = startY
    var total  = 0
    for
    {
        let one = mp(visitX, visitY)
        switch
        {
        case one & .East:
            visitX += 1
            mp(visitX, visitY) &= ~.West
        case one & .West:
            visitX -= 1
            mp(visitX, visitY) &= ~.East
        case one & .North:
            visitY -= 1
            mp(visitX, visitY) &= ~.South
        case one & .South:
            visitY += 1
            mp(visitX, visitY) &= ~.North
        }

        total += 1
        if visitX == startX and visitY == startY do
            break
    }

    total /= 2

    //Console.print("total: ", total, " ")
    @assert(total == 6870)
}
