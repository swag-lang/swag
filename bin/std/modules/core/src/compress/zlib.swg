#global marked
#global public
#global namespace Compress
using Core.Errors

#[Swag.Opaque]
struct ZLib
{
    decoder:     Inflate
    encoder:     Deflate
}

impl ZLib
{
    // Decompress stream
    mtd compress(buffer: const [..] u8, level = Deflate.CompressionLevel.Default)->Array'u8 throw
    {
        var result: retval

        result.add(0x78)
        result.add(0x9C)

        self.encoder.init(level)
        try self.encoder.compress(&result, buffer)

        result.add(cast(u8) ((self.encoder.adler.adler >> 24) & 0xFF))
        result.add(cast(u8) ((self.encoder.adler.adler >> 16) & 0xFF))
        result.add(cast(u8) ((self.encoder.adler.adler >> 8) & 0xFF))
        result.add(cast(u8) (self.encoder.adler.adler & 0xFF))

        return result
    }

    // Decompress stream
    mtd decompress(buffer: const [..] u8)->Array'u8 throw
    {
        var bs = ByteStream.openRead(buffer)
        if bs.length() < 6:
            throw InvalidFormatError{"invalid format"}

        // Compression mtd (8 is "deflate")
        let cmf = bs.readU8()
        if cmf & 0xF != 8:
            throw InvalidFormatError{"invalid compression mtd"}

        // Window size
        let cinfo = (cmf >> 4) & 0xF
        if cinfo > 7:
            throw UnsupportedFormatError{"unsupported window size"}
        let winSize = 1'u32 << (cinfo + 8)

        // Check
        let flg = bs.readU8()

        // Dict
        let fdict = (flg >> 5) & 1
        if fdict:
            throw UnsupportedFormatError{"unsupported dictionnary"}

        // Compression level
        //var flevel = (flg >> 6) & 3

        var result: Array'u8
        try self.decoder.decompress(&result, bs.remainReadToSlice(), winSize)

        // Now we should have the checksum
        self.decoder.stream.discardToNextByte()
        let adler32 = self.decoder.stream.readBits(8) << 24 |
                      self.decoder.stream.readBits(8) << 16 |
                      self.decoder.stream.readBits(8) << 8 |
                      self.decoder.stream.readBits(8)
        if adler32 != self.decoder.adler.adler:
            throw InvalidFormatError{"invalid checksum"}

        return result
    }
}
