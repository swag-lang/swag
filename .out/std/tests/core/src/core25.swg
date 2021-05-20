
using core

#test
{
    var x: String = "toto"
    x += "_titiあ"
    @assert(x == "toto_titiあ")
    Console.silent(true)
    Console.print(cast(string) x)
    Console.silent(false)
}