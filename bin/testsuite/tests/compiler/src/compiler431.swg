//#[Swag.PrintBc]
#test
{
    var cst    = 0x01_02_03_04
    let slice: [..] u8 = @mkslice(cast([*] u8) &cst, 4)
    len(slice)
}

//#[Swag.PrintBc]
func len(buffer: const [..] u8)
{
    var result = 0
    var total  = 0
    for buffer
    {
        result += 1
        total += cast(s32) buffer[@index]
    }

    @assert(total == 10)
    @assert(result == 4)
}
