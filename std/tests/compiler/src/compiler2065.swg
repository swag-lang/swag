func strlen(value: const *u8)->u32
{
    idx := 0'u32
    while value[idx] idx += 1
    return idx
}

//#[swag.printbc]
//#[swag.optimizebc(0)]
func toto(message: const *u8)
{
    var msg: string = message ? cast(string) @mkslice(message, strlen(message)) : ""
}

//#[swag.printbc]
#test
{
    toto(null)
}