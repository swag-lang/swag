/*
--- Part Two ---
Content with the amount of tree cover available, the Elves just need to know the best spot to build their tree house: they would like to be able to see a lot of trees.

To measure the viewing distance from a given tree, look up, down, left, and right from that tree; stop if you reach an edge or at the first tree that is the same height or taller than the tree under consideration. (If a tree is right on the edge, at least one of its viewing distances will be zero.)

The Elves don't care about distant trees taller than those found by the rules above; the proposed tree house has large eaves to keep it dry, so they wouldn't be able to see higher than the tree house anyway.

In the example above, consider the middle 5 in the second row:

30373
25512
65332
33549
35390
Looking up, its view is not blocked; it can see 1 tree (of height 3).
Looking left, its view is blocked immediately; it can see only 1 tree (of height 5, right next to it).
Looking right, its view is not blocked; it can see 2 trees.
Looking down, its view is blocked eventually; it can see 2 trees (one of height 3, then the tree of height 5 that blocks its view).
A tree's scenic score is found by multiplying together its viewing distance in each of the four directions. For this tree, this is 4 (found by multiplying 1 * 1 * 2 * 2).

However, you can: even better: consider the tree of height 5 in the middle of the fourth row:

30373
25512
65332
33549
35390
Looking up, its view is blocked at 2 trees (by another tree with a height of 5).
Looking left, its view is not blocked; it can see 2 trees.
Looking down, its view is also not blocked; it can see 1 tree.
Looking right, its view is blocked at 2 trees (by a massive tree of height 9).
This tree's scenic score is 8 (2 * 2 * 1 * 2); this is the ideal spot for the tree house.

Consider each tree on your map. What is the highest scenic score possible for any tree?
*/
using Core

func day8B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "8.txt")
    let lines    = assume File.readAllLines(dataPath)

    //Time.Stopwatch.scopeMeasure()

    var total: s32
    foreach l, line in lines
    {
        if line == 0 or line == lines.count - 1 do
            continue

        foreach #Bytes c, col in l
        {
            if col == 0 or col == l.length - 1 do
                continue

            var score0 = 0
            for #reverse x in 0 until col
            {
                score0 += 1
                if l[x] >= l[col] do
                    break
            }

            var score1 = 0
            for x in col + 1 to l.length - 1
            {
                score1 += 1
                if l[x] >= l[col] do
                    break
            }

            var score2 = 0
            for #reverse y in 0 until line
            {
                score2 += 1
                if lines[y][col] >= l[col] do
                    break
            }

            var score3 = 0
            for y in line + 1 to lines.count - 1
            {
                score3 += 1
                if lines[y][col] >= l[col] do
                    break
            }

            total = Math.max(score0 * score1 * score2 * score3, total)
        }
    }

    //Console.print("total: ", total)
    @assert(total == 201600)
}
