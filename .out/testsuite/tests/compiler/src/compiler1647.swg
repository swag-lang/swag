//#[Swag.printbc]
#run
{
    var arr: [12] u8
    cpt := 3'u32
    slice := cast([..] u8) @mkslice(&arr[0], cast(uint) cpt)
    @assert(@countof(slice) == 3)
}