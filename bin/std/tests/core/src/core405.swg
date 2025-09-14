using Core, Core.Random

#test
{
    let x = GUID128.getRandom()
    let y = Format.toString("%", x)
    @assert(y[8] == '-''u8)
    //Console.printf("%", y)
}
