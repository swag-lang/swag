func toto()->s32
{
    return 0
}

#test
{
    #assert(#typeof(toto).sizeof == #typeof(#type func()->s32).sizeof)
    #assert(#typeof(toto).sizeof == 8)
    @assert(@typecmp(#typeof(toto), #type func()->s32, .UnScoped))
}
