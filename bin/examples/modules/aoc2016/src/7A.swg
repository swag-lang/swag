/*
--- Day 7: Internet Protocol Version 7 ---
While snooping around the local network of EBHQ, you compile a list of IP addresses (they're IPv7, of course; IPv6 is much too limited). You'd like to figure out which IPs support TLS (transport-layer snooping).

An IP supports TLS if it has an Autonomous Bridge Bypass Annotation, or ABBA. An ABBA is any four-character sequence which consists of a pair of two different characters followed by the reverse of that pair, such as xyyx or abba. However, the IP also must not have an ABBA within any hypernet sequences, which are contained by square brackets.

For example:

abba[mnop]qrst supports TLS (abba outside square brackets).
abcd[bddb]xyyx does not support TLS (bddb is within square brackets, even though xyyx is outside square brackets).
aaaa[qwer]tyui does not support TLS (aaaa is invalid; the interior characters must be different).
ioxxoj[asdfgh]zxcvbn supports TLS (oxxo is outside square brackets, even though it's within a larger string).
How many IPs in your puzzle input support TLS?
*/

using Core

func day7A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    var cpt, total = 0
    var ok = false
    foreach l in lines
    {
        cpt = 0
        ok  = false

        for var i = 0; i < @countof(l) - 3; i += 1
        {
            if l[i] == '[' do
                cpt += 1
            elif cpt and l[i] == ']' do
                cpt -= 1
            elif l[i] == l[i + 3] and l[i + 1] == l[i + 2] and l[i] != l[i + 1]
            {
                if cpt
                {
                    ok = false
                    break
                }

                ok = true
                i += 3
            }
        }

        total += ok ? 1 : 0
    }

    //Console.print(total)
    @assert(total == 110)
}
