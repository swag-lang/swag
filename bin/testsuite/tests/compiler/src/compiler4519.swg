struct A { r, g : s32; }
#[Swag.Strict]
typealias AL = A
//typealias AL1 = AL

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
func toto()->AL
{
    return cast(AL) {1, 2}
}

#test
{
    let x = toto()
    //@print(@typeof(x).name)
    @assert(x.g == 2)
}