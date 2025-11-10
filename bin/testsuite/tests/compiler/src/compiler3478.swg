func(T) toto(cb: func||(T, T)) {}

#test
{
    toto(func(x, y: s32) {})
}
