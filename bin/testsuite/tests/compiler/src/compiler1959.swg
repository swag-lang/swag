//#[Swag.PrintBc]
#test
{
    if #typeof(s32).flags & Swag.TypeInfoFlags.Integer
    {
    }

    var x = 2.0
    var y = dref cast(*u32) &x
    @assert(y == 1073741824)
    @assert(dref cast(*f32) &y == 2.0)
}
