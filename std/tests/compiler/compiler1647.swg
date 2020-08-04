//#[swag.printbc]
#run
{
    var arr: [12] u8
    cpt := 3'u32
    slice := cast([..] u8) @sliceof(&arr[0], cpt)
    @assert(@countof(slice) == 3)
}