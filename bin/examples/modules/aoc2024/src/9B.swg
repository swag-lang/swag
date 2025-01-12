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
    day9BA()
    day9BB()
}

func day9BA()
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

    var last = map.count - #over 1
    while toCheck
    {
        toCheck -= 1
        while map[last].what != toCheck:
            last -= 1

        var i = 0'u64
        while i < last and (map[i].what != -1 or map[i].num < map[last].num):
            i += 1         
          
        if i >= last:
            continue

        map[i].what = map[last].what       
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

func day9BB()
{
/*    func file_sum(a, n, block_id: s32)->s32
    {
        let s = (((2 * a) + n - 1) * n) / 2
        return s * block_id;
    }

    Time.Stopwatch.scopeMeasure()
    
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "9.txt")
    var line     = assume File.readAllText(dataPath)  

    var checksum2 = 0'u64
    var files_by_size: Array'(Array's32)
    var moved: HashTable'(s32, u8)

    let n = cast(s32) line.length
    let last_block_ptr = n % 2 == 0 ? n - 2 : n - 1
    var block_ind = 0

    for var i = last_block_ptr; i >= 0; i -= 2:
        files_by_size[line[i] - '0'].add(i)

    var ptr = 0
    while ptr < n
    {
        if ptr % 2 == 0
        {
            if !moved.contains(ptr)
            {
                checksum2 += file_sum(block_ind, line[ptr] - '0', ptr/2)
                block_ind += (line[ptr] - '0')
            } else 
            {
                block_ind += (moved[ptr] - '0')
            }

            ptr += 1
        } 
        else 
        {
            var cur_space = line[ptr] - '0'
            var max_ind = -1
            var vec_ind = -1

            for var j = 1; j <= cur_space; j += 1
            {
                let temp = #ref files_by_size[j]
                if !temp.isEmpty() and (max_ind < temp.front()) and temp.front() > ptr
                {
                    max_ind = temp.front()
                    vec_ind = j
                }
            }

            if max_ind == -1
            {
                ptr += 1
                block_ind += cur_space
                continue
            }

            cur_space = line[max_ind] - '0'
            checksum2 += file_sum(block_ind, cur_space, max_ind/2)
            files_by_size[vec_ind].removeBack()

            moved[max_ind] = line[max_ind]
            block_ind += cur_space
            line[max_ind] = '0'
            line[ptr] -= cur_space

            if line[ptr] == '0':
                ptr += 1
        }
    }

    @print(checksum2) */
}
