func variadic(x: any)
{
    @print(cast(s64) cast(s32) x)
}
/*
//#[Swag.printbc]
func(T) isSorted(values: const [..] T)->bool
{
    countof := @countof(values)

    loop i: countof - 1
    {
        @print(cast(s64) cast(s32) values[i])
        variadic(cast(s32) values[i])
        @print(" ")
    }

    return true
}

#test
{
    var sl: [] s32 = @[1,2,3,4,5]
    isSorted(@mkslice(&sl[0], 5))
}
*/