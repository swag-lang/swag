/*
    :LICENCE
    Based on stb_vorbis.c by Sean Barrett (https://github.com/nothings/stb/tree/master).
    The original code has been modified for Swag.
    MIT License
*/

#global #[Swag.Warn("wrn0002", Swag.WarnLevel.Disable)]
#global #[Swag.Warn("wrn0005", Swag.WarnLevel.Disable)]
using Core

const PAGEFLAG_continued_packet = 1
const PAGEFLAG_first_page       = 2
const PAGEFLAG_last_page        = 4

const VORBIS_packet_id      = 1
const VORBIS_packet_comment = 3
const VORBIS_packet_setup   = 5

const VORBIS_MAX_CHANNELS = 16
const EOP = -1
const INVALID_BITS = -1
const NO_CODE = 255

const STB_VORBIS_FAST_HUFFMAN_LENGTH = 10
const FAST_HUFFMAN_TABLE_SIZE        = (1 << STB_VORBIS_FAST_HUFFMAN_LENGTH)
const FAST_HUFFMAN_TABLE_MASK        = (FAST_HUFFMAN_TABLE_SIZE - 1)

#[Swag.Opaque]
public struct ProbedPage
{
    pageStart, pageEnd: u32
    lastDecodedSample:  u32
}

public struct Vorbis
{
    channels:       u32
    sampleRate:     u32
    vendor:         String
    comments:       Array'String

    stream:         const ^u8
    streamEnd:      const ^u8

    firstDecode:    bool
    eof:            bool
    lastSeg:        bool

    codebooks:      Array'Codebook
    pageCrcTests:   s32 = -1
    segmentCount:   s32
    lastPage:       u32
    validBits:      s32
    acc:            u32
    packetBytes:    s32
    blocksize0:     s32
    blocksize1:     s32
    lastSegWhich:   s32
    segments:       [255] u8
    pageFlag:       u8
    bytesInSeg:     u8

    nextSeg:            s32
    endSegWithKnownLoc: s32
    knownLocForPacket:  u32

    pFirst: ProbedPage
    pLast:  ProbedPage
}

typealias codetype = f32

#[Swag.Opaque]
public struct Codebook
{
    dimensions, entries:    s32
    codeword_lengths:       ^u8
    minimum_value:          f32
    delta_value:            f32
    value_bits:             u8
    lookup_type:            u8
    sequence_p:             u8
    sparse:                 u8
    lookup_values:          u32
    multiplicands:          *codetype
    codewords:              *u32
    fast_huffman:           [FAST_HUFFMAN_TABLE_SIZE] s16
    sorted_codewords:       *u32
    sorted_values:          *s32
    sorted_entries:         s32
}

impl Vorbis
{
    func ilog(n: s32)->s32
    {
        const log2_4: [16] s8 = [0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4]

        if n < 0
            return 0 // signed n returns 0

        // 2 compares if n < 16, 3 compares otherwise (4 if signed or n > 1<<29)
        if n < (1 << 14)
            if n < (1 << 4)      return  0 + log2_4[n]
            elif n < (1 << 9)    return  5 + log2_4[n >> 5]
            else                 return 10 + log2_4[n >> 10]
        elif n < (1 << 24)
            if n < (1 << 19)     return 15 + log2_4[n >> 15]
            else                 return 20 + log2_4[n >> 20]
        elif n < (1 << 29)       return 25 + log2_4[n >> 25]
        else                     return 30 + log2_4[n >> 30]
    }

    mtd get8()->u8
    {
        if stream >= streamEnd
        {
            eof = true
            return 0
        }

        let res = dref stream
        stream += 1
        return res
    }

    mtd get32()->u32
    {
        var x: u32
        x = get8()
        x += get8() << 8
        x += get8() << 16
        x += cast(u32) get8() << 24
        return x
    }

    mtd getn(data: ^u8, n: s32)->bool
    {
        if stream + n > streamEnd
        {
            eof = true
            return false
        }

        Memory.copy(data, stream, acast n)
        stream += n
        return true
    }

    mtd get8PacketRaw()->s32
    {
        if !bytesInSeg
        {
            if lastSeg
                return EOP
            let len = catch nextSegment()
            if @errmsg or len == 0
                return EOP
        }

        bytesInSeg -= 1
        packetBytes += 1
        return get8()
    }

    mtd get8Packet()->s32
    {
        let x = get8PacketRaw()
        validBits = 0
        return x
    }

    mtd get32Packet()->s32
    {
        var x: u32 = get8Packet()
        x += get8Packet() << 8
        x += get8Packet() << 16
        x += cast(u32) get8Packet() << 24
        return x
    }

    mtd skip(n: s32)
    {
        stream += n
        if stream >= streamEnd
            eof = true
    }

    mtd getBits(n: s32)->u32
    {
        if validBits < 0
            return 0
        if validBits < n
        {
            if n > 24
            {
                // the accumulator technique below would not work correctly in this case
                var z = getBits(24)
                z += getBits(n - 24) << 24
                return z
            }

            if validBits == 0
                acc = 0
            while validBits < n
            {
                let z = get8PacketRaw()
                if z == EOP
                {
                    validBits = INVALID_BITS
                    return 0
                }

                acc += z << validBits
                validBits += 8
            }
        }

        let z = acc & ((1 << n) - 1)
        acc >>= n
        validBits -= n
        return z
    }

    // only run while parsing the header (3 times)
    func vorbisValidate(data: ^u8)->bool
    {
        const VorbisHeader = [`v`'u8, `o`, `r`, `b`, `i`, `s`]
        return Memory.compare(data, VorbisHeader, 6) == 0
    }

    mtd startDecoder() throw
    {
        firstDecode = true

        // first page, first packet
        ///////////////////////

        startPage()

        // validate page flag
        if !(pageFlag & PAGEFLAG_first_page)
            throw "VORBIS_invalid_first_page"
        if pageFlag & PAGEFLAG_last_page
            throw "VORBIS_invalid_first_page"
        if pageFlag & PAGEFLAG_continued_packet
            throw "VORBIS_invalid_first_page"

        // check for expected packet length
        var header: [6] u8
        if segmentCount != 1
            throw "VORBIS_invalid_first_page"
        if segments[0] != 30
        {
            // check for the Ogg skeleton fishead identifying header to refine our error
            if segments[0] == 64 and getn(header, 6) and
               header[0] == `f` and header[1] == `i` and header[2] == `s` and header[3] == `h` and header[4] == `e` and header[5] == `a` and get8() == `d` and
               get8() == 0
            {
                throw "VORBIS_ogg_skeleton_not_supported"
            }
            else
            {
                throw "VORBIS_invalid_first_page"
            }
        }

        // check packet header
        if get8() != VORBIS_packet_id
            throw "VORBIS_invalid_first_page"
        if !getn(header, 6)
            throw "VORBIS_unexpected_eof"
        if !vorbisValidate(header)
            throw "VORBIS_invalid_first_page"
        // vorbis_version
        if get32() != 0
            throw "VORBIS_invalid_first_page"
        channels = get8()
        if !channels
            throw "VORBIS_invalid_first_page"
        if channels > VORBIS_MAX_CHANNELS
            throw "VORBIS_too_many_channels"
        sampleRate = get32()
        if !sampleRate
            throw "VORBIS_invalid_first_page"
        discard get32() // bitrate_maximum
        discard get32() // bitrate_nominal
        discard get32() // bitrate_minimum

        var x: s32 = get8()
        let log0 = x & 15
        let log1 = x >> 4
        blocksize0 = 1 << log0
        blocksize1 = 1 << log1
        if log0 < 6 or log0 > 13
            throw "VORBIS_invalid_setup"
        if log1 < 6 or log1 > 13
            throw "VORBIS_invalid_setup"
        if log0 > log1
            throw "VORBIS_invalid_setup"

        // framingFlag
        x = get8()
        if !(x & 1)
            throw "VORBIS_invalid_first_page"

        // second packet!
        ///////////////////////

        startPage()
        startPacket()
        if !nextSegment()
            throw "VORBIS_invalid_first_page"

        if get8Packet() != VORBIS_packet_comment
            throw "VORBIS_invalid_setup"
        loop i: 6
            header[i] = cast(u8) get8Packet()
        if !vorbisValidate(header)
            throw "VORBIS_invalid_setup"

        // file vendor
        var len = get32Packet()
        vendor.reserve(acast len)
        loop len
            vendor += cast(u8) get8Packet()

        // user comments
        let commentListLength = get32Packet()
        comments.reserve(acast commentListLength)
        loop i: commentListLength
        {
            var str: String
            len = get32Packet()
            str.reserve(acast len)
            loop len
                str += cast(u8) get8Packet()
            comments.add(str)
        }

        // framing_flag
        x = get8Packet()
        if !(x & 1)
            throw "VORBIS_invalid_setup"

        skip(acast bytesInSeg)
        bytesInSeg = 0
        loop
        {
            len = nextSegment()
            skip(len)
            bytesInSeg = 0
            if !len break
        }

        // third packet!
        ///////////////////////

        startPacket()
        /*if (!is_whole_packet_present(f))
        {
            // convert error in ogg header to write type
            if (f->error == VORBIS_invalid_stream)
                f->error = VORBIS_invalid_setup;
            return FALSE;
        } */

        if get8Packet() != VORBIS_packet_setup
            throw "VORBIS_invalid_setup"
        loop i: 6
            header[i] = cast(u8) get8Packet()
        if !vorbisValidate(header)
            throw "VORBIS_invalid_setup"

        // codebooks
        let codebookCount = getBits(8) + 1
        codebooks.resize(acast codebookCount)
        loop i: codebookCount
        {
            let c = codebooks.buffer + i
            x = getBits(8); if x != 0x42 throw "VORBIS_invalid_setup"
            x = getBits(8); if x != 0x43 throw "VORBIS_invalid_setup"
            x = getBits(8); if x != 0x56 throw "VORBIS_invalid_setup"
            x = getBits(8)
            c.dimensions = (getBits(8) << 8) + x
            x = getBits(8)
            let y = getBits(8)
            c.entries = (getBits(8) << 16) + (y << 8) + x
            let ordered = getBits(1)
            c.sparse = acast (ordered ? 0 : getBits(1))
            if c.dimensions == 0 and c.entries != 0
                throw "VORBIS_invalid_setup"

            var lengths: Array'u8
            lengths.resize(acast c.entries)

            var total: s32
            if ordered
            {
                var current_entry = 0
                var current_length = cast(u8) (getBits(5) + 1)
                while current_entry < c.entries
                {
                    let limit = c.entries - current_entry
                    let n = getBits(ilog(limit))
                    if current_length >= 32
                        throw "VORBIS_invalid_setup"
                    if current_entry + n > c.entries
                        throw "VORBIS_invalid_setup"
                    Memory.set(lengths.buffer + current_entry, current_length, acast n)
                    current_entry += n
                    current_length += 1
                }
            }
            else
            {
                loop j: c.entries
                {
                    let present = c.sparse ? getBits(1) : 1
                    if present
                    {
                        lengths[j] = cast(u8) (getBits(5) + 1)
                        total += 1
                        if lengths[j] == 32
                            throw "VORBIS_invalid_setup"
                    }
                    else
                        lengths[j] = acast NO_CODE
                }
            }
        }
    }

    mtd capturePattern()->bool
    {
        if 0x4f != get8() return false
        if 0x67 != get8() return false
        if 0x67 != get8() return false
        if 0x53 != get8() return false
        return true
    }

    mtd startPageNoCapturePattern() throw
    {
        // stream structure version
        if get8() != 0
            throw "VORBIS_invalid_stream_structure_version"

        // header flag
        pageFlag = get8()

        // absolute granule position
        let loc0 = get32()
        let loc1 = get32()

        // stream serial number -- vorbis doesn't interleave, so discard
        discard get32()

        // page sequence number
        let n = get32()
        lastPage = n

        // crc32
        discard get32()

        // page_segments
        segmentCount = get8()
        if !getn(segments, segmentCount)
            throw "VORBIS_unexpected_eof"

        // assume we _don't_ know any the sample position of any segments
        endSegWithKnownLoc = -2
        if loc0 != ~0 or loc1 != ~0
        {
            // determine which packet is the last one that will complete
            var i = segmentCount-1
            while i >= 0
            {
                if segments[i] < 255
                    break
                i -= 1
            }

            // 'i' is now the index of the _last_ segment of a packet that ends
            if i >= 0
            {
                endSegWithKnownLoc = i
                knownLocForPacket = loc0
            }

        }

        if firstDecode
        {
            var len = 0
            loop i: segmentCount
                len += segments[i]
            len += 27 + segmentCount

            pFirst.pageEnd = pFirst.pageStart + len
            pFirst.lastDecodedSample = loc0
        }

        nextSeg = 0
    }

    mtd startPage() throw
    {
        if !capturePattern()
            throw "VORBIS_missing_capture_pattern"
        startPageNoCapturePattern()
    }

    mtd startPacket() throw
    {
        while nextSeg == -1
        {
            startPage()
            if pageFlag & PAGEFLAG_continued_packet
                throw "VORBIS_continued_packet_flag_invalid"
        }

        lastSeg     = false
        validBits   = 0
        packetBytes = 0
        bytesInSeg  = 0
    }

    mtd nextSegment()->s32 throw
    {
        if lastSeg
            return 0
        if nextSeg == -1
        {
            lastSegWhich = segmentCount - 1 // in case start_page fails
            catch startPage()
            if @errmsg
            {
                lastSeg = true
                return 0
            }

            if !(pageFlag & PAGEFLAG_continued_packet)
                throw "VORBIS_continued_packet_flag_invalid"
        }

        let len = segments[nextSeg]
        nextSeg += 1
        if len < 255
        {
            lastSeg = true
            lastSegWhich = nextSeg - 1
        }

        if nextSeg >= segmentCount
            nextSeg = -1
        bytesInSeg = len
        return len
    }
}

public impl Vorbis
{
    mtd openPushData(dataBlock: const ^u8, length: u32)->u32 throw
    {
        @init(self)
        stream = dataBlock
        streamEnd = dataBlock + length
        catch startDecoder()
        if @errmsg
        {
            if eof
                return 0
            throw @errmsg
        }

        return 0
    }
}