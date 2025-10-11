/*
--- Day 7: The Sum of Its Parts ---
You find yourself standing on a snow-covered coastline; apparently, you landed a little off course. The region is too hilly to see the North Pole from here, but you: spot some Elves that seem to be trying to unpack something that washed ashore. It's quite cold out, so you decide to risk creating a paradox by asking them for directions.

"Oh, are you the search party?" Somehow, you can understand whatever Elves from the year 1018 speak; you assume it's Ancient Nordic Elvish. Could the device on your wrist also be a translator? "Those clothes don't look very warm; take this." They hand you a heavy coat.

"We: need to find our way back to the North Pole, but we have higher priorities at the moment. You see, believe it or not, this box contains something that will solve all of Santa's transportation problems - at least, that's what it looks like from the pictures in the instructions." It doesn't seem like they can read whatever language it's in, but you can: "Sleigh kit. Some assembly required."

"'Sleigh'? What a wonderful name! You must help us assemble this 'sleigh' at once!" They start excitedly pulling more parts out of the box.

The instructions specify a series of steps and requirements about which steps must be finished before others can begin (your puzzle input). Each step is designated by a single letter. For example, suppose you have the following instructions:

Step C must be finished before step A can begin.
Step C must be finished before step F can begin.
Step A must be finished before step B can begin.
Step A must be finished before step D can begin.
Step B must be finished before step E can begin.
Step D must be finished before step E can begin.
Step F must be finished before step E can begin.
Visually, these requirements look like this:

  -->A--->B--
 /    \      \
C      -->D----->E
 \           /
  ---->F-----
Your first goal is to determine the order in which the steps should be completed. If more than one step is ready, choose the step which is first alphabetically. In this example, the steps would be completed as follows:

Only C is available, and so it is done first.
Next, both A and F are available. A is first alphabetically, so it is done next.
Then, even though F was available earlier, steps B and D are now also available, and B is the first alphabetically of the three.
After that, only D and F are available. E is not available because only some of its prerequisites are complete. Therefore, D is completed next.
F is the only choice, so it is done next.
Finally, E is completed.
So, in this example, the correct order is CABDFE.

In what order should the steps in your instructions be completed?
*/

using Core

func day7A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    var steps: HashTable'(u8, { prev, next: String })
    foreach l in lines
    {
        steps.findOrAdd(l[5]).value.next.append(l[36])
        steps.findOrAdd(l[36]).value.prev.append(l[5])
    }

    var first = Swag.U8.Max
    var last: u8
    foreach k, v in steps
    {
        if v.next.length == 0 do
            last = k
        if v.prev.length == 0 do
            first = Math.min(first, k)
    }

    var result: String
    for
    {
        result.append(first)
        if first == last do
            break

        first = Swag.U8.Max
        foreach k, v in steps
        {
            if Utf8.contains(result.toSlice(), cast(rune) k) do
                continue

            var bad = false
            foreach c in v.prev
            {
                if !Utf8.contains(result.toSlice(), cast(rune) c)
                {
                    bad = true
                    break
                }
            }

            if !bad and k < first do
                first = k
        }
    }

    //Console.print("result: ", result)
    @assert(result == "GJFMDHNBCIVTUWEQYALSPXZORK")
}
