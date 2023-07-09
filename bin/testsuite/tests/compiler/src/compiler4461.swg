func toto(x: ^s32)
{
    var p = &x[0]
    #assert @typeof(p) == ^s32
}