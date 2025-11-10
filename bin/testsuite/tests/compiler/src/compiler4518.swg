struct A { r, g: s32 }
//#[Swag.Strict]
alias AL = A
//alias AL1 = AL

func toto()->AL
{
    return cast(AL) {1, 2}
}
#test
{
    let x = toto()
    //@print(#typeof(x).name)
    @assert(x.g == 2)
}
