using Core

//#[Swag.PrintBc]
#test
{
    let a = Hash.Crc64.compute("toto")
    @assert(a == 0x1F530541FB733253)
    //Console.printf("%{X}", a)
}
