enum ChunkType: u32
{
    IHDR = @byteswap("I"'u32 << 24 | "H"'u32 << 16 | "D"'u32 << 8 | "R"'u32)
}