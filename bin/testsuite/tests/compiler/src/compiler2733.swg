#test
{
    @assert(#typeof(2'u32 * #sizeof(u32)) == u32)
    @assert(#typeof(2'u64 * #sizeof(u32)) == u64)
}
