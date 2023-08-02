#global public
using Core

#[Swag.EnumFlags]
enum SoundFileValidityMask
{
    Zero = 0
    Format
    Frequency
    ChannelCount
    ChannelMask
    BitsPerSample
    Duration
    SampleCount
    ValidBitsPerSample
    Data
    MetaData
}

enum SoundFileFormat
{
    Unknown
    Pcm
    FloatIEEE
    Ogg
}

struct SoundFile
{
    sampleCount:        u64
    validity:           SoundFileValidityMask
    format              = SoundFileFormat.Unknown
    frequency:          u32
    channelCount:       u32
    channelMask:        u32
    bitsPerSample:      u32
    duration:           f32
    validBitsPerSample: u16
    dataSize:           u64
    dataSeek:           u64

    datas:  Array'u8
}