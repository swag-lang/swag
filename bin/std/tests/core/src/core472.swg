using Core

//#[Swag.PrintBc]
#test
{
    let a = Hash.Crc64.compute("toto", .ECMA182)
    @assert(a == 0x1F530541FB733253)
    //Console.printf("%{X}", a)

    let b = Hash.Crc64.compute("toto", .ISO)
    @assert(b == 0x3C286C2850000000)
    //Console.printf("%{X}", b)
}
