func titi()->s32 => 0

//#[Swag.PrintBc]
func nextF32()->f32 => titi() / cast(f32) Swag.U32.Max

#test
{
    n := nextF32()
}