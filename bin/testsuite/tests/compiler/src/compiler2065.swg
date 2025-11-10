func strlen(value: const [*] u8)->u32
{
    var idx = 0'u32
    while value[idx] do
        idx += 1
    return idx
}

//#[Swag.PrintBc]
//#[Swag.optimizebc(0)]
func toto(message: #null const [*] u8)
{
    var msg: string = message ? @mkstring(message, cast(u64) strlen(message)) : ""
}

//#[Swag.PrintBc]
#test
{
    toto(null)
}
