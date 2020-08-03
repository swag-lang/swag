//#[swag.printbc]
#test
{
    var x: const* swag.TypeInfo
    x = s32
    //@assert(x == s32)
    //@assert(x == @typeof(s32))
}
