//#[swag.printbc]
#test
{
    if @typeof(s32).flags & swag.TypeInfoFlags.Integer
    {
    }

    x := 2.0
    y := deref cast(*u32) &x
    @assert(y == 1073741824)
    @assert(deref cast(*f32) &y == 2.0)
}