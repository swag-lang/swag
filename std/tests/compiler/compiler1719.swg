#test
{
    alias callback = func(s32)->s32
    func toDo(value: s32, ptr: callback)->s32 => ptr(value)

    @assert(toDo(4, @(x) => x * x) == 16)
}