struct A {
    x, y: s32
}

#test
{
    var x, y: A
    var ptrX = &x
    //ptrX.0 = y
}