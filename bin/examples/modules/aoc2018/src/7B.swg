/*
--- Part Two ---
As you're about to begin construction, four of the Elves offer to help. "The sun will set soon; it'll go faster if we work together." Now, you need to account for multiple people working on steps simultaneously. If multiple steps are available, workers should still begin them in alphabetical order.

Each step takes 60 seconds plus an amount corresponding to its letter: A=1, B=2, C=3, and so on. So, step A takes 60+1=61 seconds, while step Z takes 60+26=86 seconds. No time is required between steps.

To simplify things for the example, however, suppose you only have help from one Elf (a total of two workers) and that each step takes 60 fewer seconds (so that step A takes 1 second and step Z takes 26 seconds). Then, using the same instructions as above, this is how each second would be spent:

Second   Worker 1   Worker 2   Done
   0        C          .
   1        C          .
   2        C          .
   3        A          F       C
   4        B          F       CA
   5        B          F       CA
   6        D          F       CAB
   7        D          F       CAB
   8        D          F       CAB
   9        D          .       CABF
  10        E          .       CABFD
  11        E          .       CABFD
  12        E          .       CABFD
  13        E          .       CABFD
  14        E          .       CABFD
  15        .          .       CABFDE
Each row represents one second of time. The Second column identifies how many seconds have passed as of the beginning of that second. Each worker column shows the step that worker is currently doing (or . if they are idle). The Done column shows completed steps.

Note that the order of the steps has changed; this is because steps now take time to finish and multiple workers can begin multiple steps simultaneously.

In this example, it would take 15 seconds for two workers to complete these steps.

With 5 workers and the 60+ second step durations described above, how long will it take to complete all of the steps?
*/

using Core

func day7B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    var steps: HashTable'(u8, { prev, next: String })
    foreach l in lines
    {
        steps.findOrAdd(l[5]).value.next.append(l[36])
        steps.findOrAdd(l[36]).value.prev.append(l[5])
    }

    var first: Array'u8
    var last:  u8
    foreach k, v in steps
    {
        if v.next.length == 0 do
            last = k
        if v.prev.length == 0 do
            first.add(k)
    }

    var result:  String
    var current: Array'u8
    var workers: [5] { val: u8, tick: s32 }

    foreach cc, i in first
    {
        workers[i].val  = cc
        workers[i].tick = (cc - 'A') + 61
        current.add(cc)
    }

    var total = 0
    for
    {
        total += 1

        var done: Array'u64
        foreach &w in workers
        {
            if w.val == 0 do
                continue
            w.tick -= 1
            if w.tick == 0 do
                done.add(@index)
        }

        var avail: Array'u8
        foreach d in done
        {
            let w = &workers[d]
            let c = w.val
            w.val = 0

            result.append(c)
            current.remove(c)
            if c == last
            {
                //Console.print("total: ", total)
                @assert(total == 1050)
                return
            }

            foreach k, v in steps
            {
                if result.contains(k) or current.contains(k) do
                    continue
                if !Slice.allOf(v.prev.toSlice(), func|&result|(x) => result.contains(x)) do
                    continue
                avail.add(k)
            }
        }

        //Slice.sort(avail.toSlice(), bigFirst: true)
        foreach &w1 in workers
        {
            if w1.val or avail.isEmpty() do
                continue
            let cc = avail.popBack()
            w1.val  = cc
            w1.tick = (cc - 'A') + 61
            current.add(cc)
        }
    }
}
