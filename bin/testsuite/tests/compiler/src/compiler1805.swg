#test
{
    let value = #typeof({1, 2'f64})
    @assert(@countof(value.fields) == 2)
    @assert(value.fields[0].name == "item0")
    @assert(value.fields[1].name == "item1")
    @assert(value.fields[0].pointedType.fullname == "s32")
    @assert(value.fields[1].pointedType.fullname == "f64")
}