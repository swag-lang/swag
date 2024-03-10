using Core, Core.Random

#test
{
    var x = UUID.getRandom()
    var y = Format.toString("%", x)
    @assert(y[8] == `-`'u8)
    //Console.printf("%", y)
}