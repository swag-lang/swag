#test
{
    @assert(@typeof(2'u32 * @sizeof(u32)) == u32)
    @assert(@typeof(2'uint * @sizeof(u32)) == uint)
}