struct MyStruct {}

impl MyStruct
{
    mtd opCount() => 4'u64
}

/**
You can loop on a struct value if `opCount` has been defined.
*/
#test
{
    var v = MyStruct{}

    // '@countof' will call 'opCount'
    @assert(@countof(v) == 4)

    // You can then loop on a struct value if 'opCount' has been defined
    var cpt = 0
    loop v do
        cpt += 1

    @assert(cpt == 4)
}
