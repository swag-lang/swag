func toto()->s32 => 0

//#[swag.printbc]
#test
{
    //@assert(@typeof(s32) == s32);
   // @assert([2] s32 != *s32);
   @print(@typeof(toto).name)
   @print(" ")
   @print(@typeof(func()->s32).name)
   //@assert(@typeof(toto) == func()->s32)
}
