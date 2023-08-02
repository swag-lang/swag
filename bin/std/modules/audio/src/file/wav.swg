#global namespace Wav
using Core, Win32

struct ChunkHeader
{
    chunkID:    ChunkID = undefined
    chunkSize:  u32
}

#if Core.BIG_ENDIAN
{
    #[Swag.ConstExpr]
    func SND_WAV_FOURCC_MARKER(a, b, c, d: u32) => ((a << 24) | (b << 16) | (c << 8) | d)
}
#else
{
    #[Swag.ConstExpr]
    func SND_WAV_FOURCC_MARKER(a, b, c, d: u32) => (a | (b << 8) | (c << 16) | (d << 24))
}

enum ChunkID: u32
{
    SND_WAV_FOURCC_RIFF = SND_WAV_FOURCC_MARKER("R"'u32, "I"'u32, "F"'u32, "F"'u32)
    SND_WAV_FOURCC_RIFX = SND_WAV_FOURCC_MARKER("R"'u32, "I"'u32, "F"'u32, "X"'u32)
    SND_WAV_FOURCC_WAVE = SND_WAV_FOURCC_MARKER("W"'u32, "A"'u32, "V"'u32, "E"'u32)
    SND_WAV_FOURCC_fmt  = SND_WAV_FOURCC_MARKER("f"'u32, "m"'u32, "t"'u32, " "'u32)
    SND_WAV_FOURCC_data = SND_WAV_FOURCC_MARKER("d"'u32, "a"'u32, "t"'u32, "a"'u32)
    SND_WAV_FOURCC_fact = SND_WAV_FOURCC_MARKER("f"'u32, "a"'u32, "c"'u32, "t"'u32)
    SND_WAV_FOURCC_PEAK = SND_WAV_FOURCC_MARKER("P"'u32, "E"'u32, "A"'u32, "K"'u32)
    SND_WAV_FOURCC_cue  = SND_WAV_FOURCC_MARKER("c"'u32, "u"'u32, "e"'u32, " "'u32)
    SND_WAV_FOURCC_LIST = SND_WAV_FOURCC_MARKER("L"'u32, "I"'u32, "S"'u32, "T"'u32)
    SND_WAV_FOURCC_slnt = SND_WAV_FOURCC_MARKER("s"'u32, "l"'u32, "n"'u32, "t"'u32)
    SND_WAV_FOURCC_wavl = SND_WAV_FOURCC_MARKER("w"'u32, "a"'u32, "v"'u32, "l"'u32)
    SND_WAV_FOURCC_INFO = SND_WAV_FOURCC_MARKER("I"'u32, "N"'u32, "F"'u32, "O"'u32)
    SND_WAV_FOURCC_plst = SND_WAV_FOURCC_MARKER("p"'u32, "l"'u32, "s"'u32, "t"'u32)
    SND_WAV_FOURCC_adtl = SND_WAV_FOURCC_MARKER("a"'u32, "d"'u32, "t"'u32, "l"'u32)
    SND_WAV_FOURCC_labl = SND_WAV_FOURCC_MARKER("l"'u32, "a"'u32, "b"'u32, "l"'u32)
    SND_WAV_FOURCC_ltxt = SND_WAV_FOURCC_MARKER("l"'u32, "t"'u32, "x"'u32, "t"'u32)
    SND_WAV_FOURCC_note = SND_WAV_FOURCC_MARKER("n"'u32, "o"'u32, "t"'u32, "e"'u32)
    SND_WAV_FOURCC_smpl = SND_WAV_FOURCC_MARKER("s"'u32, "m"'u32, "p"'u32, "l"'u32)
    SND_WAV_FOURCC_bext = SND_WAV_FOURCC_MARKER("b"'u32, "e"'u32, "x"'u32, "t"'u32)
    SND_WAV_FOURCC_levl = SND_WAV_FOURCC_MARKER("l"'u32, "e"'u32, "v"'u32, "l"'u32)
    SND_WAV_FOURCC_MEXT = SND_WAV_FOURCC_MARKER("M"'u32, "E"'u32, "X"'u32, "T"'u32)
    SND_WAV_FOURCC_DISP = SND_WAV_FOURCC_MARKER("D"'u32, "I"'u32, "S"'u32, "P"'u32)
    SND_WAV_FOURCC_acid = SND_WAV_FOURCC_MARKER("a"'u32, "c"'u32, "i"'u32, "d"'u32)
    SND_WAV_FOURCC_strc = SND_WAV_FOURCC_MARKER("s"'u32, "t"'u32, "r"'u32, "c"'u32)
    SND_WAV_FOURCC_PAD  = SND_WAV_FOURCC_MARKER("P"'u32, "A"'u32, "D"'u32, " "'u32)
    SND_WAV_FOURCC_afsp = SND_WAV_FOURCC_MARKER("a"'u32, "f"'u32, "s"'u32, "p"'u32)
    SND_WAV_FOURCC_clm  = SND_WAV_FOURCC_MARKER("c"'u32, "l"'u32, "m"'u32, " "'u32)
    SND_WAV_FOURCC_elmo = SND_WAV_FOURCC_MARKER("e"'u32, "l"'u32, "m"'u32, "o"'u32)
    SND_WAV_FOURCC_ISFT = SND_WAV_FOURCC_MARKER("I"'u32, "S"'u32, "F"'u32, "T"'u32)
    SND_WAV_FOURCC_ICRD = SND_WAV_FOURCC_MARKER("I"'u32, "C"'u32, "R"'u32, "D"'u32)
    SND_WAV_FOURCC_ICOP = SND_WAV_FOURCC_MARKER("I"'u32, "C"'u32, "O"'u32, "P"'u32)
    SND_WAV_FOURCC_IARL = SND_WAV_FOURCC_MARKER("I"'u32, "A"'u32, "R"'u32, "L"'u32)
    SND_WAV_FOURCC_IART = SND_WAV_FOURCC_MARKER("I"'u32, "A"'u32, "R"'u32, "T"'u32)
    SND_WAV_FOURCC_INAM = SND_WAV_FOURCC_MARKER("I"'u32, "N"'u32, "A"'u32, "M"'u32)
    SND_WAV_FOURCC_IENG = SND_WAV_FOURCC_MARKER("I"'u32, "E"'u32, "N"'u32, "G"'u32)
    SND_WAV_FOURCC_IPRD = SND_WAV_FOURCC_MARKER("I"'u32, "P"'u32, "R"'u32, "D"'u32)
    SND_WAV_FOURCC_ISRC = SND_WAV_FOURCC_MARKER("I"'u32, "S"'u32, "R"'u32, "C"'u32)
    SND_WAV_FOURCC_ISBJ = SND_WAV_FOURCC_MARKER("I"'u32, "S"'u32, "B"'u32, "J"'u32)
    SND_WAV_FOURCC_ICMT = SND_WAV_FOURCC_MARKER("I"'u32, "C"'u32, "M"'u32, "T"'u32)
}

enum WavFormat: u16
{
    Unknown    = 0
    Pcm        = 1
    Adpcm      = 2
    FloatIEEE  = 3
    Extensible = 0xFFFE // Microsoft
}

public const GUID_PCM        = GUID{0x00000001, 0x0000, 0x0010, [0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71]}
public const GUID_ADPCM      = GUID{0x00000002, 0x0000, 0x0010, [0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71]}
public const GUID_FLOAT_IEEE = GUID{0x00000003, 0x0000, 0x0010, [0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71]}

struct WaveFormat
{
    formatTag:      WavFormat   // Format type
    channels:       u16         // Number of channels (i.e. mono, stereo, etc.)
    samplesPerSec:  u32         // Sample rate
    avgBytesPerSec: u32         // For buffer estimation
    blockAlign:     u16         // Block size of data
}

struct PcmWaveFormat
{
    using wf: WaveFormat
    bitsPerSample:  u16
}

struct WaveFormatEx
{
    using pcmwf: PcmWaveFormat
    cbSize: u16     // The count in bytes of the size of extra information (after cbSize)
}

struct WaveFormatExtensible
{
    using fmtex: WaveFormatEx

    samples: union
    {
        validBitsPerSample: u16  // Valid bits in each sample container
        samplesPerBlock:    u16  // Samples per block of audio data valid
        reserved:           u16  // If wBitsPerSample=0 (but rarely used). Zero if neither case above applies.
    }

    channelMask: u32    // Positions of the audio channels
    subFormat:   GUID   // Format identifier GUID
}

func readChunkFmt(using file: *SoundFile, chunkSize: u32) throw
{
    if chunkSize < @sizeof(WaveFormat)
        throw "invalid wav format"

    var wf: WaveFormat
    if !stream.readComplete(&wf, @sizeof(wf))
        throw "invalid wav format"

    switch (wf.formatTag)
    {
    case Pcm:       file.format = .Pcm
    case FloatIEEE: file.format = .FloatIEEE
    default:        file.format = .Unknown
    }

    frequency     = wf.samplesPerSec
    channelCount  = wf.channels
    bitsPerSample = (wf.avgBytesPerSec * 8 / wf.channels) / wf.samplesPerSec

    validity = .Format
    validity |= .Frequency
    validity |= .ChannelCount

    ////////
    if chunkSize < @sizeof(PcmWaveFormat)
        return

    var pcmwf: PcmWaveFormat
    pcmwf.wf = wf
    validity |= .BitsPerSample

    if !stream.readComplete(&pcmwf.bitsPerSample, @sizeof(pcmwf.bitsPerSample))
        throw "invalid wav format"

    ////////
    if chunkSize < @sizeof(WaveFormatEx)
        return

    var wfx: WaveFormatEx
    wfx.pcmwf = pcmwf

    if !stream.readComplete(&wfx.cbSize, @sizeof(wfx.cbSize))
        throw "invalid wav format"

    ////////
    if chunkSize < @sizeof(WaveFormatExtensible)
        return

    var wfex: WaveFormatExtensible
    wfex.fmtex = wfx
    file.validBitsPerSample = wfex.samples.validBitsPerSample
    validity |= .ChannelMask
    validity |= .ValidBitsPerSample

    if !stream.readComplete(&wfex.samples, @sizeof(wfex.samples))
        throw "invalid wav format"
    if !stream.readComplete(&wfex.channelMask, @sizeof(wfex.channelMask))
        throw "invalid wav format"
    if !stream.readComplete(&wfex.subFormat, @sizeof(wfex.subFormat))
        throw "invalid wav format"

    if wf.formatTag == .Extensible
    {
        if Memory.equals(&wfex.subFormat, GUID_PCM)
            file.format = .Pcm
        elif Memory.equals(&wfex.subFormat, GUID_FLOAT_IEEE)
            file.format = .FloatIEEE
        else
            throw "unsupported wav format"
    }
}

func readChunkData(using file: *SoundFile, chunkSize: u32) throw
{
    validity |= .Data
    dataSeek = stream.getPosition()
    dataSize = chunkSize

    validity |= .SampleCount
    sampleCount = dataSize / (channelCount * (bitsPerSample / 8))
    if validity & .Frequency
    {
        duration = cast(f32) (sampleCount / cast(f64) frequency)
        validity |= .Duration
    }
}

func isFormatSupported(using file: *SoundFile)->bool
{
    switch file.format
    {
    case Pcm:
        switch bitsPerSample
        {
        case 8, 16, 24, 32: return true
        default:            return false
        }

    case FloatIEEE:
        return true
    }

    return false
}

// Load a wav file
public func loadFile(using file: *SoundFile, loadDatas = true, loadMetaDatas = false) throw
{
    enum Stage{FindRiff, FindFmt, FindData, BreakOnSuccess}
    var stage:  Stage = FindRiff
    var header: ChunkHeader

    var continueParsing = true
    while continueParsing
    {
        let seek = stream.getPosition()
        if seek & 1
            stream.skipPosition(1)

        if !stream.readComplete(&header, @sizeof(header))
            throw "corrupted wav file"

        switch header.chunkID
        {
        case SND_WAV_FOURCC_RIFF:
            if stage == .FindRiff
            {
                if !stream.readComplete(&header.chunkID, @sizeof(header.chunkID))
                    throw "invalid riff chunk"
                if header.chunkID == SND_WAV_FOURCC_WAVE
                    stage = .FindFmt
                else
                    throw "not a wav"
            }

        case SND_WAV_FOURCC_fmt:
            if stage == .FindFmt
            {
                file.readChunkFmt(header.chunkSize)
                if !isFormatSupported()
                    throw "unsupported format"
                stage = .FindData
            }

        case SND_WAV_FOURCC_data:
            if stage == .FindData
            {
                readChunkData(header.chunkSize)
                if loadDatas
                {
                    datas.resize(acast header.chunkSize)
                    if !stream.readComplete(datas.buffer, acast header.chunkSize)
                        throw "cannot read datas"
                }
                else
                {
                    stream.skipPosition(acast header.chunkSize)
                }

                continueParsing = loadMetaDatas
            }

        case SND_WAV_FOURCC_fact:
            stream.skipPosition(acast header.chunkSize)

        case SND_WAV_FOURCC_cue:
            stream.skipPosition(acast header.chunkSize)

        case SND_WAV_FOURCC_LIST:
            stream.skipPosition(acast header.chunkSize)

        case SND_WAV_FOURCC_labl:
            stream.skipPosition(acast header.chunkSize)

        case SND_WAV_FOURCC_note:
            stream.skipPosition(acast header.chunkSize)

        case SND_WAV_FOURCC_ltxt:
            stream.skipPosition(acast header.chunkSize)

        case SND_WAV_FOURCC_bext:
            stream.skipPosition(acast header.chunkSize)

        default:
            if header.chunkSize
                stream.skipPosition(acast header.chunkSize)
            else
                throw "invalid chunk"
        }
    }
}
