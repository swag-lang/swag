// kicked : 334388
//
// kicked : 79193

#[Swag.NoInline]
#[Swag.PrintAsm]
func toto(x, y: s32)->s32
{
    return x <=> y
}

#test
{
    discard toto(1, 2)
}