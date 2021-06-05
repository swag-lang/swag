#global testerror

#[Swag.safety("", true)]
#test
{
    x := 0xFFFFFFFF'u32
    var y: s32
    y = x + 1
    //@print(cast(s64) y)
}