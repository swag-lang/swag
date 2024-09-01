/*
--- Part Two ---
Your device's communication system is correctly detecting packets, but still isn't working. It looks like it also needs to look for messages.

A start-of-message marker is just like a start-of-packet marker, except it consists of 14 distinct characters rather than 4.

Here are the first positions of start-of-message markers for all of the above examples:

mjqjpqmgbljsphdztnvjfqwrcgsmlb: first marker after character 19
bvwbjplbgvbhsrlpgdmjqwftvncz: first marker after character 23
nppdvjthqldpwncqszvftbrmjlhg: first marker after character 23
nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg: first marker after character 29
zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw: first marker after character 26
How many characters need to be processed before the first start-of-message marker is detected?
*/

using Core

func day6B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "6.txt")
    let lines    = assume File.readAllLines(dataPath)
    let str      = #constref lines[0]

    var arr: [128] s32
    var len  = 0

    const LEN = 14
    for i in str
    {
        arr[str[i]] += 1
        len += cast(s32) (arr[str[i]] == 1)

        if i >= LEN
        {
            arr[str[i - LEN]] -= 1
            len -= cast(s32) (arr[str[i - LEN]] == 0)
        }

        if len == LEN
        {
            //Console.print("total: ", i+1)
            @assert(i + 1 == 2308)
            break
        }
    }
}
