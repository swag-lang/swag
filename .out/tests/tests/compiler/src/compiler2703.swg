#global testerror

struct(T, N: s32) AB
{
    #assert @typeof(T) == s32 || @typeof(T) == f32
    min: [N] T
    max: [N] T
}

#test
{
    var ABf: AB'(bool, 2)
    //var ABf: AB'(f32, 0)
}