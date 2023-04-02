func toto(x: ^s32)
{
    p := &x[0]
    #assert @typeof(p) == ^s32
}