/*
--- Part Two ---
For added security, yet another system policy has been put in place. Now, a valid passphrase must contain no two words that are anagrams of each other - that is, a passphrase is invalid if any word's letters can be rearranged to form any other word in the passphrase.

For example:

abcde fghij is a valid passphrase.
abcde xyz ecdab is not valid - the letters from the third word can be rearranged to form the first word.
a ab abc abd abf abj is a valid passphrase, because all letters need to be used when forming another word.
iiii oiii ooii oooi oooo is valid.
oiii ioii iioi iiio is not valid - any of these words can be rearranged to form any other word.
Under this new system policy, how many passphrases are valid?
*/

using Core

func day4B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "4.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total = 0
    foreach l in lines
    {
        let s    = Tokenize.split(l, " ")
        var set: HashSet'string

        foreach w in s
        {
            var str: String = w
            Slice.sort(str.toSlice())
            set.add(str)
        }

        total += cast(s32) (set.count == s.count)
    }

    @assert(total == 186)
}
