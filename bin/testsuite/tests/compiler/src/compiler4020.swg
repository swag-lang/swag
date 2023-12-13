//#[Swag.PrintBc]
#test
{
    var x = 1
    var y = 1000

    var myRef: const ref s32 = &x
    @assert(myRef == 1)

    myRef = const ref &y
    @assert(myRef == 1000)
}