#global testerror
struct AA
{
    x: s32
}

#assert @offsetof(AA.x == 0)
