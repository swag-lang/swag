#global public
#global namespace compress

#[Swag.opaque]
struct ZLibDecoder
{
    decoder: DeflateDecoder
}

impl ZLibDecoder
{
    private func init(using self, buffer: const [..] u8) throw
    {
        bs := ByteStream.create(buffer)
        if bs.length() < 6 throw "invalid format"

        // Compression method (8 is "deflate")
        cmf := bs.readU8()
        if cmf & 0xF != 8 throw "invalid compression method"

        // Window size
        cinfo := (cmf >>,safe 4) & 0xF
        if cinfo > 7 throw "unsupported window size"
        winSize := 1'uint << (cinfo + 8)

        // Check
        flg := bs.readU8()
        fcheck := flg & 0x1f
		fcheck_computed := ((cast(u32) cmf << 8) | flg) & 0x1f
		if fcheck != fcheck_computed throw "checksum failed"

        // Dict
        fdict := (flg >>,safe 5) & 1
        if fdict throw "unsupported dictionnary"

        // Compression level
        flevel := (flg >>,safe 6) & 3

        decoder.init(bs.toSlice(), winSize)
    }

    // Decompress stream
    func decompress(using self, buffer: const [..] u8)->Array'u8 throw
    {
        try init(buffer)

        var result: retval
        result = try decoder.decompress()

        // Now we should have the checksum
        decoder.stream.discardToNextByte()
        adler32 := decoder.stream.readBits(8) << 24 |
                   decoder.stream.readBits(8) << 16 |
                   decoder.stream.readBits(8) << 8  |
                   decoder.stream.readBits(8)
        if adler32 != decoder.adler.adler throw "invalid checksum"
        return result
    }
}