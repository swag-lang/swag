struct A { r, g : s32; }
//#[Swag.Strict]
typealias AL = A
//typealias AL1 = AL

func toto()->AL
{
    return cast(AL) {1, 2}
}
#test
{
    var x = toto()
    //@print(#typeof(x).name)
    @assert(x.g == 2)
}