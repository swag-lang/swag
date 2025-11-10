#[Swag.Mixin]
func mix()
{
    #alias0 += #alias1
}

//#[Swag.PrintBc]
#test
{
    let z = 4
    var x, y = 0
    mix(|x, z|)
    mix(|y, z|)
    @assert(x == 4)
    @assert(y == 4)
}
