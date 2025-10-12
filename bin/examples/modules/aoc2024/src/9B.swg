/*
--- Part Two ---
Upon completion, two things immediately become clear. First, the disk definitely has a lot more contiguous free space, just like the amphipod hoped. Second, the computer is running much more slowly! Maybe introducing all of that file system fragmentation was a bad idea?

The eager amphipod already has a new plan: rather than move individual blocks, he'd like to try compacting the files on his disk by moving whole files instead.

This time, attempt to move whole files to the leftmost span of free space blocks that could fit the file. Attempt to move each file exactly once in order of decreasing file ID number starting with the file with the highest file ID number. If there is no span of free space to the left of a file that is large enough to fit the file, the file does not move.

The first example from above now proceeds differently:

00...111...2...333.44.5555.6666.777.888899
0099.111...2...333.44.5555.6666.777.8888..
0099.1117772...333.44.5555.6666.....8888..
0099.111777244.333....5555.6666.....8888..
00992111777.44.333....5555.6666.....8888..
The process of updating the filesystem total is the same; now, this example's total would be 2858.

Start over, now compacting the amphipod's hard drive using this new method instead. What is the resulting filesystem total?
*/

using Core

func day9B()
{
    day9BA()
    day9BB()
}

//#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
//#[Swag.PrintAsm]
func day9BA()
{
    Time.Stopwatch.scopeMeasure()

    /*var x = 0
    for 100_000_000:
        x += 1
    @print(x)*/

    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "9.txt")
    let line     = assume File.readAllText(dataPath)

    var map: Array'{ what, num: s64 }

    var toCheck: s64
    foreach t in line
    {
        let r = t - '0'
        if @index & 1 do
            map.add({-1, r})
        else
        {
            map.add({toCheck, r})
            toCheck += 1
        }
    }

    var last = map.count - #wrap 1
    while toCheck
    {
        toCheck -= 1
        while map[last].what != toCheck do
            last -= 1

        var i = 0'u64
        while i < last and (map[i].what != -1 or map[i].num < map[last].num) do
            i += 1

        if i >= last do
            continue

        map[i].what    = map[last].what
        map[last].what = -1

        if map[i].num != map[last].num
        {
            let n = map[i].num - map[last].num
            map[i].num = map[last].num
            map.insertAt(i + 1, {-1, n})
        }
    }

    var idx, total: u64
    foreach v in map
    {
        if v.what == -1
        {
            idx += v.num
        }
        else
        {
            for v.num
            {
                total += idx * v.what
                idx += 1
            }
        }
    }

    @assert(total == 6183632723350)
}

//#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
func day9BB()
{
    func fileSum(a, n, block_id: s64)->u64
    {
        let s = cast(u64) ((((2 * a) + n - 1) * n) / 2)
        return s * block_id
    }

    // Time.Stopwatch.scopeMeasure()

    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "9.txt")
    var line     = assume File.readAllText(dataPath)

    var filesBySize: [10] Array's32
    var moved:       HashTable'(s32, u8)

    for var i = 0; i < line.length; i += 2 do
        filesBySize[line[i] - '0'].add(i)

    var total = 0'u64
    var idx, blockIdx = 0
    while idx < line.length
    {
        if idx % 2 == 0
        {
            if !moved.contains(idx)
            {
                total += fileSum(blockIdx, line[idx] - '0', idx / 2)
                blockIdx += line[idx] - '0'
            }
            else
            {
                blockIdx += moved[idx] - '0'
            }

            idx += 1
        }
        else
        {
            var numSpaces = line[idx] - '0'
            var maxIdx, vecIdx = -1

            if numSpaces
            {
                for j in 1 to numSpaces
                {
                    let temp = &filesBySize[j]
                    if temp.isEmpty() do
                        continue
                    if maxIdx < temp.back() and temp.back() > idx
                    {
                        maxIdx = temp.back()
                        vecIdx = j
                    }
                }
            }

            if maxIdx == -1
            {
                idx += 1
                blockIdx += numSpaces
                continue
            }

            numSpaces = line[maxIdx] - '0'
            total += fileSum(blockIdx, numSpaces, maxIdx / 2)
            filesBySize[vecIdx].removeBack()

            moved[maxIdx] = line[maxIdx]
            blockIdx += numSpaces
            line[maxIdx] = '0'
            line[idx] -= numSpaces

            if line[idx] == '0' do
                idx += 1
        }
    }

    @assert(total == 6183632723350)
}
