#global testerror "Err0027"

alias X = func()->s32

#test
{
    @assert(X == 0'u64)
}
