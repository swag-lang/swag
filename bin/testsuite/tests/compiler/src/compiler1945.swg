func foozle(ptr: const *Swag.TypeInfo)->bool
{
    return ptr.flags & Swag.TypeInfoFlags.Integer ? true : false
}

#test
{
    @assert(foozle(s32))
    @assert(!foozle(f32))
    @assert(!foozle(string))
    @assert(foozle(u8))
}