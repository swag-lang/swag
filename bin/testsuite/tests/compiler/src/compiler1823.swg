func toto()->s32 => 0
func titi()->s32 => 0

//#[Swag.PrintBc]
#test
{
    @assert(#typeof(s32).fullname == "s32")
    @assert(#typeof(s32) == s32)
    @assert(#type [2] s32 != *s32)
    @assert(#typeof(toto) != #typeof(titi))
    //@assert(#typeof(toto) != func()->s32)
    //@assert(#typeof(toto).rawType == func()->s32)
}
