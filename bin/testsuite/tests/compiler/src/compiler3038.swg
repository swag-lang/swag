/*struct(T: [..] typeinfo) AA
{
}*/

#test
{
    x := @[s32, bool]
    @assert(x[0] == s32)
    @assert(x[1] == bool)
}