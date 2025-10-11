struct Col { a, r, g, b: s32 }

func toto(c: Col) {}

#test
{
    let a = Col{1, 1, 1, 1}
    toto(a)
}
