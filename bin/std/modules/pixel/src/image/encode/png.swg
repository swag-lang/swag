/*
    :LICENCE
    This code is based on the libspng library by Randy (https://https://libspng.org/)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Png
using Core

using namespace EncoderPrivate {

const SPNG_WRITE_SIZE = 8192'uint
const SPNG_EOI = 75

const adam7_x_start: [7] u32 = @[ 0, 4, 0, 2, 0, 1, 0 ]
const adam7_y_start: [7] u32 = @[ 0, 0, 4, 0, 2, 0, 1 ]
const adam7_x_delta: [7] u32 = @[ 8, 8, 4, 4, 2, 2, 1 ]
const adam7_y_delta: [7] u32 = @[ 8, 8, 8, 4, 4, 2, 2 ]

struct RowInfo
{
    scanline_idx:   u32
    row_num:        u32
    pass:           s32
    filter:         u8
}

#[Swag.EnumFlags]
enum ColorType: u8
{
    Paletted    = 1
    Color       = 2
    Alpha       = 4
    AlphaColor  = Alpha | Color
}

struct EncodeFlags
{
    filter_choice:  FilterChoice
    interlace:      bool
    same_layout:    bool
    to_bigendian:   bool
    progressive:    bool
    finalize:       bool
}

enum FilterChoice
{
    SPNG_DISABLE_FILTERING = 0
    SPNG_FILTER_CHOICE_NONE = 8
    SPNG_FILTER_CHOICE_SUB = 16
    SPNG_FILTER_CHOICE_UP = 32
    SPNG_FILTER_CHOICE_AVG = 64
    SPNG_FILTER_CHOICE_PAETH = 128
    SPNG_FILTER_CHOICE_ALL = (8|16|32|64|128)
}

const PngSignature:   [8] u8 = @[ 137, 80, 78, 71, 13, 10, 26, 10 ]
const TypeIHdr:       [4] u8 = @[ 73, 72, 68, 82 ]
const TypeIDat:       [4] u8 = @[ 73, 68, 65, 84 ]
const TypeIEnd:       [4] u8 = @[ 73, 69, 78, 68 ]

struct SubImage
{
    width:              u32
    height:             u32
    out_width:          u32
    scanline_width:     u32
}

struct Ihdr
{
    width:              u32
    height:             u32
    bit_depth:          u8
    color_type:         ColorType
    compression_method: u8
    filter_method:      u8
    interlace_method:   u8
};

struct ChunkBitfield
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

struct Chunk
{
    offset:     uint
    length:     u32
    type:       [4] u8
    crc:        u32
}
}

#[Swag.Opaque]
public struct Encoder
{
    stream:                 *ConcatBuffer
    data:                   *u8
    png_base:               *u8
    data_size:              u64
    bytes_left:             u64
    stored:                 ChunkBitfield
    ihdr:                   Ihdr
    bytes_encoded:          uint
    current_chunk:          Chunk
    tmpBuf:                 [256] u8
    write_ptr:              *u8
    crc:                    Hash.Crc32
    subimage:               [7] SubImage
    widest_pass:            s32
    last_pass:              s32
    bytes_per_pixel:        u32
    scanline_buf:           *u8
    prev_scanline_buf:      *u8
    row_buf:                *u8
    filtered_scanline_buf:  *u8
    filtered_scanline:      *u8
    scanline:               *u8
    prev_scanline:          *u8
    encode_flags:           EncodeFlags
    row_info:               RowInfo
    pixel_size:             u32
    bufferToCompress:       Array'u8
    swapRB:                 bool
}

impl Encoder
{
    mtd write_u32(dst: *u8, val: u32)
    {
        v := Math.nativeToBigEndian(val)
        dref cast(*u32) dst = v
    }

    mtd write_data(buf: const *u8, bytes: u32)
    {
        stream.addBytes(@mkslice(buf, bytes))
        bytes_encoded += bytes
    }

    mtd write_u32(val: u32)
    {
        v := Math.nativeToBigEndian(val)
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

        crc.init()
        crc.update(null)
        crc.update(chunk_type)
        current_chunk.crc = crc.crc
        bytes_encoded += current_chunk.length;

        dataPtr := stream.currentSP
        stream.seek(chunk_length)

        return dataPtr
    }

    mtd finish_chunk()
    {
        crc.update(@mkslice(write_ptr, current_chunk.length))
        current_chunk.crc = crc.crc
        write_u32(current_chunk.crc)
    }

    mtd write_chunk(chunk_type: const [4] u8, buf: const *u8, length: uint)
    {
        write_ptr = write_header(chunk_type, length)
        if length
            Memory.copy(write_ptr, buf, length)
        finish_chunk()
    }

    mtd write_chunks_before_idat() throw
    {
        write_data(PngSignature, 8)

        dst := &tmpBuf[0]
        write_u32(dst, ihdr.width)
        write_u32(dst + 4, ihdr.height)
        dst[8]  = ihdr.bit_depth
        dst[9]  = cast(u8) ihdr.color_type
        dst[10] = ihdr.compression_method
        dst[11] = ihdr.filter_method
        dst[12] = ihdr.interlace_method
        write_chunk(TypeIHdr, dst, 13)
    }

    mtd calculate_scanline_width(width: u32)->u32 throw
    {
        if !width
            throw "SPNG_EINTERNAL"

        res := cast(uint) num_channels() * ihdr.bit_depth
        if res > Swag.UInt.Max / width
            throw "SPNG_EOVERFLOW"
        res = res * width

        res += 15
        if res < 15
            throw "SPNG_EOVERFLOW"
        res /= 8
        if res > Swag.U32.Max
            throw "SPNG_EOVERFLOW"

        return cast(u32) res
    }

    mtd num_channels()->s32
    {
        switch ihdr.color_type
        {
        case Color:         return 3
        case AlphaColor:    return 4
        }

        return 0
    }

    mtd calculate_subimages() throw
    {
        if ihdr.interlace_method == 1
        {
            subimage[0].width = (ihdr.width + 7) >> 3
            subimage[0].height = (ihdr.height + 7) >> 3
            subimage[1].width = (ihdr.width + 3) >> 3
            subimage[1].height = (ihdr.height + 7) >> 3
            subimage[2].width = (ihdr.width + 3) >> 2
            subimage[2].height = (ihdr.height + 3) >> 3
            subimage[3].width = (ihdr.width + 1) >> 2
            subimage[3].height = (ihdr.height + 3) >> 2
            subimage[4].width = (ihdr.width + 1) >> 1
            subimage[4].height = (ihdr.height + 1) >> 2
            subimage[5].width = ihdr.width >> 1
            subimage[5].height = (ihdr.height + 1) >> 1
            subimage[6].width = ihdr.width
            subimage[6].height = ihdr.height >> 1
        }
        else
        {
            subimage[0].width = ihdr.width
            subimage[0].height = ihdr.height
        }

        loop i: 6
        {
            if subimage[i].width == 0 or subimage[i].height == 0
                continue

            subimage[i].scanline_width = calculate_scanline_width(subimage[i].width)
            if subimage[widest_pass].scanline_width < subimage[i].scanline_width
                widest_pass = i
            last_pass = i
        }
    }

    mtd encode_row(row: *u8, len: u32)->bool throw
    {
        pass := row_info.pass

        if !ihdr.interlace_method or pass == 6
            return encode_scanline(row, len)

        bit_depth := ihdr.bit_depth

        if bit_depth < 8
        {
            samples_per_byte := 8 / bit_depth
            mask             := cast(u8) ((1 << bit_depth) - 1)
            initial_shift    := 8 - bit_depth
            shift_amount     := initial_shift

            row_uc := row
            Memory.clear(scanline, cast(uint) subimage[pass].scanline_width)

            loop k: subimage[pass].width
            {
                ioffset := adam7_x_start[pass] + k * adam7_x_delta[pass]

                sample := row_uc[ioffset / samples_per_byte]
                sample = sample >> (initial_shift - ioffset * bit_depth % 8)
                sample = sample & mask
                sample = sample << shift_amount

                scanline[0] |= sample

                shift_amount -= bit_depth

                if shift_amount > 7
                {
                    shift_amount = initial_shift
                    scanline += 1
                }
            }

            return encode_scanline(scanline, len)
        }

        loop k: subimage[pass].width
        {
            ioffset := (adam7_x_start[pass] + k * adam7_x_delta[pass]) * pixel_size
            Memory.copy(scanline + k * pixel_size, row + ioffset, acast pixel_size)
        }

        return encode_scanline(scanline, len)
    }

    mtd encode_scanline(sl: *u8, len: u32)->bool throw
    {
        pass := row_info.pass

        scanline_width := subimage[pass].scanline_width
        if sl != scanline
        {
            if swapRB and bytes_per_pixel == 3
            {
                for i := 0; i < scanline_width - 1; i += 3
                {
                    scanline[i]     = sl[i + 2]
                    scanline[i + 1] = sl[i + 1]
                    scanline[i + 2] = sl[i]
                }
            }
            elif swapRB and bytes_per_pixel == 4
            {
                for i := 0; i < scanline_width - 1; i += 4
                {
                    scanline[i]     = sl[i + 2]
                    scanline[i + 1] = sl[i + 1]
                    scanline[i + 2] = sl[i]
                    scanline[i + 3] = sl[i + 3]
                }
            }
            else
            {
                Memory.copy(scanline, sl, cast(uint) scanline_width - 1)
            }
        }

        var filtered_sl: *u8
        filter := 0
        if !filter
            filtered_sl = scanline

        write_idat_bytes(filtered_sl - 1, scanline_width)

        swap(&prev_scanline, &scanline)
        if update_row_info()
        {
            finish_idat()
            return true
        }

        return false
    }

    mtd finish_idat() throw
    {
        if bufferToCompress.count == 0
            return

        var zlib: Compress.ZLib
        cmp := zlib.compress(bufferToCompress.toSlice())
        write_ptr = write_header(TypeIDat, cmp.count)

        Memory.copy(write_ptr, cmp.buffer, cmp.count)

        bufferToCompress.count = 0
        finish_chunk()
    }

    mtd write_idat_bytes(sl: *u8, len: u32) throw
    {
        bufferToCompress.add(@mkslice(sl, len))
    }

    mtd update_row_info()->bool
    {
        interlacing := ihdr.interlace_method
        ri  := &row_info
        sub := subimage

        if ri.scanline_idx == sub[ri.pass].height - 1
        {
            if ri.pass == last_pass
                return true

            ri.scanline_idx = 0
            ri.pass += 1

            while (!sub[ri.pass].width or !sub[ri.pass].height) and (ri.pass < last_pass)
                ri.pass += 1
        }
        else
        {
            ri.row_num += 1
            ri.scanline_idx += 1
        }

        if interlacing
            ri.row_num = adam7_y_start[ri.pass] + ri.scanline_idx * adam7_y_delta[ri.pass]

        return false
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
        // unsupported pixel format
        if image.pf.channels() < 3 or image.pf.bitDepth() != 8
            throw "unsupported pixel format"

        data = image.pixels
        png_base = image.pixels
        data_size = image.size
        bytes_left = image.size

        swapRB = image.pf == .BGR8 or image.pf == .BGRA8

        stream = result
        bytes_encoded = 0
        crc.init()

        stored.ihdr = true
        ihdr.width = image.width
        ihdr.height = image.height
        ihdr.bit_depth = 8
        ihdr.color_type = .Color
        if image.bpp == 32
            ihdr.color_type |= .Alpha

        write_chunks_before_idat()
        calculate_subimages()
        bytes_per_pixel = num_channels() * (ihdr.bit_depth / 8)

        scanline_buf_size := subimage[widest_pass].scanline_width
        scanline_buf_size += 32
        if scanline_buf_size < 32
            throw "SPNG_EOVERFLOW"

        scanline_buf = Memory.alloc(cast(uint) scanline_buf_size)
        prev_scanline_buf = Memory.alloc(cast(uint) scanline_buf_size)

        scanline = scanline_buf + 16
        prev_scanline = prev_scanline_buf + 16

        if encode_flags.filter_choice
        {
            filtered_scanline_buf = Memory.alloc(cast(uint) scanline_buf_size)
            filtered_scanline = filtered_scanline_buf + 16
        }

        sub := subimage
        ri  := &row_info

        pixel_size = image.bpp

        loop
        {
            ioffset := (image.height - row_info.row_num - 1) * image.width * image.bpp8
            if encode_row(image.pixels + ioffset, acast (image.width * image.bpp8))
                break
        }

        discard write_header(TypeIEnd, 0)
        finish_chunk()
    }
}