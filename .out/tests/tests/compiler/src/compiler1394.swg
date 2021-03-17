func titi()->s32 => 0

//#[swag.printbc]
func nextF32()->f32 => titi() / cast(f32) swag.MaxU32

#test
{
    n := nextF32()
}