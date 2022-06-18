/*
    :LICENCE
    This code is based on the libspng library by Randy (https://https://libspng.org/)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Png
using Core

const spng_signature: [8] u8 = @[ 137, 80, 78, 71, 13, 10, 26, 10 ]
const type_ihdr:      [4] u8 = @[ 73, 72, 68, 82 ]

enum spng_state
{
    SPNG_STATE_INVALID = 0
    SPNG_STATE_INIT = 1
    SPNG_STATE_INPUT
    SPNG_STATE_OUTPUT = SPNG_STATE_INPUT
    SPNG_STATE_IHDR
    SPNG_STATE_FIRST_IDAT
    SPNG_STATE_DECODE_INIT
    SPNG_STATE_ENCODE_INIT = SPNG_STATE_DECODE_INIT
    SPNG_STATE_EOI
    SPNG_STATE_LAST_IDAT
    SPNG_STATE_AFTER_IDAT
    SPNG_STATE_IEND
}

struct spng_ihdr
{
    width:              u32
    height:             u32
    bit_depth:          u8
    color_type:         u8
    compression_method: u8
    filter_method:      u8
    interlace_method:   u8
};

struct spng_chunk_bitfield
{
    ihdr: bool
    plte: bool
    chrm: bool
    iccp: bool
    gama: bool
    sbit: bool
    srgb: bool
    text: bool
    bkgd: bool
    hist: bool
    trns: bool
    phys: bool
    splt: bool
    time: bool
    offs: bool
    exif: bool
    unknown: bool
};

struct spng_chunk
{
    offset:     uint
    length:     u32
    type:       [4] u8
    crc:        u32
};

#[Swag.Opaque]
public struct Encoder
{
    stream:             *ConcatBuffer
    data:               *u8
    png_base:           *u8
    data_size:          u64
    bytes_left:         u64
    state:              spng_state
    stored:             spng_chunk_bitfield
    ihdr:               spng_ihdr
    bytes_encoded:      uint
    current_chunk:      spng_chunk
    tmpBuf:             [256] u8
}

impl Encoder
{
    mtd write_u32(dst: *u8, val: u32)
    {
        dref cast(*u32) dst = val
    }

    mtd write_data(buf: const *u8, bytes: u32)
    {
        stream.addBytes(@mkslice(buf, bytes))
        bytes_encoded += bytes
    }

    mtd write_u32(val: u32)
    {
        v := val
        stream.addBytes(@mkslice(cast(const *u8) &v, 4))
        bytes_encoded += 4
    }

    mtd write_header(chunk_type: const [4] u8, chunk_length: uint)->*u8
    {
        total := chunk_length + 8 + 4
        stream.grow(total)

        current_chunk.length = cast(u32) chunk_length
        write_u32(current_chunk.length)

        Memory.copy(&current_chunk.type, chunk_type, 4)
        write_data(current_chunk.type, 4)

        current_chunk.crc = Hash.Crc32.compute(chunk_type)
        bytes_encoded += current_chunk.length;

        return stream.currentSP
    }

    mtd finish_chunk()
    {
        var v: Hash.Crc32
        v.crc = current_chunk.crc
        v.update(@mkslice(stream.currentSP, current_chunk.length))
        current_chunk.crc = v.crc
        write_u32(current_chunk.crc)
    }

    mtd write_chunk(chunk_type: const [4] u8, buf: const *u8, length: uint)
    {
        write_ptr := write_header(chunk_type, length)
        if length
            Memory.copy(write_ptr, data, length)
        finish_chunk()
    }

    mtd write_chunks_before_idat() throw
    {
        write_data(spng_signature, 8)

        dst := &tmpBuf[0]
        write_u32(dst, ihdr.width)
        write_u32(dst + 4, ihdr.height)
        dst[8]  = ihdr.bit_depth
        dst[9]  = ihdr.color_type
        dst[10] = ihdr.compression_method
        dst[11] = ihdr.filter_method
        dst[12] = ihdr.interlace_method
        write_chunk(type_ihdr, dst, 13)
    }

    mtd write_chunks_after_idat() throw
    {
    }

    mtd encode_chunks() throw
    {
        if state < .SPNG_STATE_FIRST_IDAT
        {
            write_chunks_before_idat()
            state = .SPNG_STATE_FIRST_IDAT
        }
        elif state == .SPNG_STATE_FIRST_IDAT
        {
            return
        }
        elif state == .SPNG_STATE_EOI
        {
            write_chunks_after_idat()
            state = .SPNG_STATE_IEND
        }
        else
            throw "SPNG_EOPSTATE"
    }
}

public impl IImageEncoder for Encoder
{
    mtd canEncode(fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".png"
    }

    mtd encode(result: *ConcatBuffer, image: Image, options: EncodeOptions) throw
    {
        data = image.pixels
        png_base = image.pixels
        data_size = image.size
        bytes_left = image.size
        state = .SPNG_STATE_OUTPUT

        stream = result
        bytes_encoded = 0

        stored.ihdr = true
        ihdr.width = image.width
        ihdr.height = image.height
        ihdr.bit_depth = image.bpp

        try encode_chunks()
    }
}