using Core, Core.Random

#test
{
    var x = GUID128.getRandom()
    var y = Format.toString("%", x)
    @assert(y[8] == '-''u8)
    //Console.printf("%", y)
}
