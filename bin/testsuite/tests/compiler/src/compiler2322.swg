//#[Swag.PrintBc]
#[Swag.Safety("sanity", false)]
#test
{
    var ptr: ^u8
    let x = 1
    ptr = ptr + x
}
