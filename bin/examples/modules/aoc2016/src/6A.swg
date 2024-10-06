/*
--- Day 6: Signals and Noise ---
Something is jamming your communications with Santa. Fortunately, your signal is only partially jammed, and protocol in situations like this is to switch to a simple repetition code to get the message through.

In this model, the same message is sent repeatedly. You've recorded the repeating message signal (your puzzle input), but the data seems quite corrupted - almost too badly to recover. Almost.

All you need to: is figure out which character is most frequent for each position. For example, suppose you had recorded the following messages:

eedadn
drvtee
eandsr
raavrd
atevrs
tsrnev
sdttsa
rasrtv
nssdts
ntnada
svetve
tesnvt
vntsnd
vrdear
dvrsen
enarar
The most common character in the first column is e; in the second, a; in the third, s, and so on. Combining these characters returns the error-corrected message, easter.

Given the recording in your puzzle input, what is the error-corrected version of the message being sent?
*/

using Core, Core.Hash

func day6A()
{
    day6A0()
    day6A1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day6A0()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var freq: [8, 26] u32
    var best: [8] { val: u8, freq: u32 }

    foreach l in lines
    {
        foreach c, i in l
        {
            var v = freq[i, c - 'a']
            v += 1
            freq[i, c - 'a'] = v

            if v > best[i].freq
            {
                best[i].freq = v
                best[i].val  = c
            }
        }
    }

    var pwd: String
    foreach v in best
    {
        pwd += v.val
    }

    @assert(pwd == "afwlyyyq")
}

func day6A1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var freq: [8, 26] u32
    var best: [8] u32
    var pwd:  String = "        "

    foreach l in lines
    {
        foreach c, i in l
        {
            var v = #ref &freq[i, c - 'a']
            v += 1

            if v > best[i]
            {
                best[i] = v
                pwd[i]  = c
            }
        }
    }

    @assert(pwd == "afwlyyyq")
}
