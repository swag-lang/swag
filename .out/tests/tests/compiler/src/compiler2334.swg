#test
{
    str := "\xc2\x80"
    @assert(str[0] == 0xc2);
    @assert(str[1] == 0x80);
}