func write_chunk(chunk_type: TT, buf: #null const *u8, length: u64)
{
    @assert(chunk_type[0] == 73)
    @assert(chunk_type[1] == 72)
    @assert(chunk_type[2] == 68)
    @assert(chunk_type[3] == 82)
    @assert(length == 666)
}

alias TT = const [4] u8
const type_ihdr: TT = [73, 72, 68, 82]

#test
{
    write_chunk(type_ihdr, null, 666)
}
