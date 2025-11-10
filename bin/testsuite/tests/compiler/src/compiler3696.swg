func write_chunk(chunk_type: const [4] u8, buf: #null const *u8, length: u64)
{
    @assert(chunk_type[0] == 73)
    @assert(chunk_type[1] == 72)
    @assert(chunk_type[2] == 68)
    @assert(chunk_type[3] == 82)
    @assert(length == 666)
}

const type_ihdr: [4] u8 = [73, 72, 68, 82]

#test
{
    write_chunk(type_ihdr, null, 666)
}
