func(T) toto(a: {x: T})->{x: T}
{
    return {x: 0}
}

#test
{
    let a = &toto's32
    let b = a({0's32})
    #assert @typeof(a).name == "func'(s32)(const &{s32})->{s32}"
}