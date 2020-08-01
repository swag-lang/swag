//#[swag.printbc]
#run
{
    var arr: [12] u8
    cpt := 3
    slice := cast([..] u8) @{&arr[0], cpt}
    @assert(@countof(slice) == 3)
}