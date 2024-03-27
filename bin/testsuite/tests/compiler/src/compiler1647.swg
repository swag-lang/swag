//#[Swag.PrintBc]
#run
{
    var arr:  [12] u8
    let cpt   = 3'u32
    let slice = cast([..] u8) @mkslice(&arr[0], cast(u64) cpt)
    @assert(@countof(slice) == 3)
}
