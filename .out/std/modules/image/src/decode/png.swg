#global namespace png
using swag, core

enum ChunkType: u32
{
    IHDR = "I"'u32 << 24 | "H"'u32 << 16 | "D"'u32 << 8 | "R"'u32
	PLTE = "P"'u32 << 24 | "L"'u32 << 16 | "T"'u32 << 8 | "E"'u32
	BKGD = "b"'u32 << 24 | "K"'u32 << 16 | "G"'u32 << 8 | "D"'u32
	TRNS = "t"'u32 << 24 | "R"'u32 << 16 | "N"'u32 << 8 | "S"'u32
	IDAT = "I"'u32 << 24 | "D"'u32 << 16 | "A"'u32 << 8 | "T"'u32

	ITXt = "i"'u32 << 24 | "T"'u32 << 16 | "X"'u32 << 8 | "t"'u32
	TEXt = "t"'u32 << 24 | "E"'u32 << 16 | "X"'u32 << 8 | "t"'u32
	ZTXt = "z"'u32 << 24 | "T"'u32 << 16 | "X"'u32 << 8 | "t"'u32

	ICCP = "i"'u32 << 24 | "C"'u32 << 16 | "C"'u32 << 8 | "P"'u32
	PHYs = "p"'u32 << 24 | "H"'u32 << 16 | "Y"'u32 << 8 | "s"'u32
	GAMA = "g"'u32 << 24 | "A"'u32 << 16 | "M"'u32 << 8 | "A"'u32
	TIME = "t"'u32 << 24 | "I"'u32 << 16 | "M"'u32 << 8 | "E"'u32

	SPLT = "s"'u32 << 24 | "P"'u32 << 16 | "L"'u32 << 8 | "T"'u32
	SRGB = "s"'u32 << 24 | "R"'u32 << 16 | "G"'u32 << 8 | "B"'u32
	HIST = "h"'u32 << 24 | "I"'u32 << 16 | "S"'u32 << 8 | "T"'u32
	CHRM = "c"'u32 << 24 | "H"'u32 << 16 | "R"'u32 << 8 | "M"'u32
	SBIT = "s"'u32 << 24 | "B"'u32 << 16 | "I"'u32 << 8 | "T"'u32

	EXIf = "e"'u32 << 24 | "X"'u32 << 16 | "I"'u32 << 8 | "f"'u32
    IEND = "I"'u32 << 24 | "E"'u32 << 16 | "N"'u32 << 8 | "D"'u32
	IDOT = "i"'u32 << 24 | "D"'u32 << 16 | "O"'u32 << 8 | "T"'u32
	CbGI = "C"'u32 << 24 | "b"'u32 << 16 | "H"'u32 << 8 | "I"'u32
}

struct ChunkHeader
{
    length: u32
    type:   ChunkType = ?
}

#[opaque]
public struct Decoder
{
    stream: compress.CompressStream
}

impl Decoder
{
    func doIt(using self, buffer: const [..] u8)->Image throw
    {
        var result: retval
        stream.init(buffer)

        seenIEnd := false

        var signature: u64 = ?
        stream.peekBytes(@sizeof(signature), cast(*u8) &signature)
        if signature != 0x0a1a0a0d474e5089 throw "not a png"

        //while !seenIEnd
        {
            var ch: ChunkHeader = ?
            stream.peekBytes(@sizeof(ch), cast(*u8) &ch)
            if stream.eof throw "invalid format"

            switch ch.type
            {
            case IHDR:
                break
            }
        }

        return result
    }
}

public impl IDecoder for Decoder
{
    func canDecode(using self, fileName: string)->bool
    {
        ext := path.getExtensionLowerCase(fileName)
        return ext == ".png"
    }

    func decode(using self, buffer: const [..] u8)->Image throw
    {
        return try doIt(buffer)
    }
}