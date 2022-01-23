#global skip
func(A, B) toto(x: {A, B})
{
    //@print(x.x)
}

func toto1(x: {s32, s32})
{
    @print(x.item0)
}

#test
{
    //toto1(@{1, 1})
    toto'(s32, s32)(@{1, 2})
}