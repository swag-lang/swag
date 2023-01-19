using Core, Random

#test
{
    x := UUID.v4()
    y := Format.toString("%", x)
    @assert(y[8] == "-"'u8)
    //Console.printf("%", y)
}