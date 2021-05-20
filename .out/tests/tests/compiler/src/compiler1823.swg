func toto()->s32 => 0
func titi()->s32 => 0

//#[Swag.printbc]
#test
{
    @assert(@typeof(s32).name == "s32");
    @assert(@typeof(s32) == s32);
    @assert([2] s32 != *s32);
    @assert(@typeof(toto) != @typeof(titi))
    //@assert(@typeof(toto) != func()->s32)
    //@assert(@typeof(toto).rawType == func()->s32)
}
