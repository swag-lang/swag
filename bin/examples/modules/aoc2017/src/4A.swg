/*
--- Day 4: High-Entropy Passphrases ---
A new system policy has been put in place that requires all accounts to use a passphrase instead of simply a password. A passphrase consists of a series of words (lowercase letters) separated by spaces.

To ensure security, a valid passphrase must contain no duplicate words.

For example:

aa bb cc dd ee is valid.
aa bb cc dd aa is not valid - the word aa appears more than once.
aa bb cc dd aaa is valid - aa and aaa count as different words.
The system's full passphrase list is available as your puzzle input. How many passphrases are valid?
*/

using Core

func day4A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "4.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total = 0
    foreach l in lines
    {
        let s    = Tokenize.split(l, " ")
        let set: HashSet'string = s[ to ]
        total += cast(s32) (set.count == s.count)
    }

    @assert(total == 455)
}
