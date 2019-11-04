struct E
{
    x: s32
}

#test
{
    ptr := @typeof(E.x)
}