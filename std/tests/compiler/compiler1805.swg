#test
{
    value := @typeof({1, 2})
    @assert(@countof(value.fields) == 2)
}