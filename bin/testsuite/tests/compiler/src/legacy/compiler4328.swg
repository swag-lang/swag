alias A = s32
#test
{
    var x:  func(bool, y = 1)
    var x1: func(*A, y = 1)
    //var x2: func(A, y = 1)
    var x3: func(#type A, y = 1)
    var x4: func(A, const *void)
}
