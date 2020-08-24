func(T) AA()
{
    #run
    {
        ptr := @typeof(T)
        if(ptr == s32)
            @assert(false)
    }
}

#test
{
    AA's32()
}