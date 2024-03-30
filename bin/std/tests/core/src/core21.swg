using Core

//#[Swag.PrintBc]
func XX(v: String)->[..] u8 => @mkslice(v.buffer, cast(u64) v.length)

//#[Swag.PrintBc]
#test
{
    var x: String = "coucou㐁㐄㐅"
    let y  = XX(x)
    Console.silent(true)
    Console.print(cast(string) y)
    Console.silent(false)
}
