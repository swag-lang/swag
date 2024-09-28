func strlen(value: const ^u8)->u32
{
    var idx = 0'u32
    while value[idx]: idx += 1
    return idx
}

//#[Swag.PrintBc]
//#[Swag.optimizebc(0)]
func toto(message: nl const ^u8)
{
    var msg: string = message ? @mkstring(message, cast(u64) strlen(message)) : ""
}

//#[Swag.PrintBc]
#test
{
    toto(null)
}