func titi()->s32 => 0

//#[Swag.printbc]
func nextF32()->f32 => titi() / cast(f32) Swag.U32.Max

#test
{
    n := nextF32()
}