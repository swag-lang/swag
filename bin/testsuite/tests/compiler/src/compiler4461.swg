func toto(x: nn ^s32)
{
    let p = &x[0]
    #assert #typeof(p) == ^s32
}