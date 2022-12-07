//#[Swag.PrintBc]
#test
{
    x := 1
    y := 1000

    var myRef: const ref s32 = &x
    @assert(myRef == 1)

    myRef = ref &y
    @assert(myRef == 1000)
}