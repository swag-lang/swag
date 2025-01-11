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
The process of updating the filesystem checksum is the same; now, this example's checksum would be 2858.

Start over, now compacting the amphipod's hard drive using this new method instead. What is the resulting filesystem checksum?
*/

using Core

func day9B()
{
    //Time.Stopwatch.scopeMeasure()
    
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "9.txt")
    var line     = assume File.readAllText(dataPath)

    var map: Array'{what, num: s64}
    var toCheck: s64
    foreach t in line
    {
        let r = t - '0'
        if #index & 1:        
            map.add({-1, r})
        else
        {
            map.add({toCheck, r})
            toCheck += 1
        }
    }

    //Time.Stopwatch.scopeMeasure()

    var last = map.count - #over 1
    toCheck -= 1
    for
    {
        while map[last].what != toCheck:
            last -= 1

        var i = 0'u64
        while i < last and (map[i].what != -1 or map[i].num < map[last].num):
            i += 1         
          
        if i < last
        {
            map[i].what = map[last].what       

            if map[i].num == map[last].num
            {
                map[last].what = -1
                last -= 1            
            }
            else
            {
                let n = map[i].num - map[last].num
                map[i].num = map[last].num
                map[last].what = -1
                map.insertAt(i + 1, {-1, n})
            }
        }

        if toCheck == 0:
            break
        toCheck -= 1
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
