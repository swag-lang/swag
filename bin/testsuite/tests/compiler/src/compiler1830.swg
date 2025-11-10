#global testerror "Err0031"

alias X = func()->s32

#test
{
    @assert(X == 0'u64)
}
