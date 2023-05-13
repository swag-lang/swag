struct MyStruct
{
}

impl MyStruct
{
    mtd opCount() => 4'u64
}

#test
{
    v := MyStruct{}

    // '@countof' will call 'opCount'
    @assert(@countof(v) == 4)

    // You can then loop on a struct value if 'opCount' has been defined
    cpt := 0
    loop v
        cpt += 1

    @assert(cpt == 4)
}