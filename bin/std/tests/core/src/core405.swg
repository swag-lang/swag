using Core, Random

#test
{
    x := UUID.getRandom()
    y := Format.toString("%", x)
    @assert(y[8] == "-"'u8)
    //Console.printf("%", y)
}