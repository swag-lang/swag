#global public

#[Swag.EnumFlags]
enum SoundFileInfoValidityMask
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

enum SoundFileInfoFormat
{
    Unknown
    Pcm
    FloatIEEE
    Ogg
}

struct SoundFileInfo
{
    sampleCount:        u64
    validity:           SoundFileInfoValidityMask
    format              = SoundFileInfoFormat.Unknown
    frequency:          u32
    channelCount:       u32
    channelMask:        u32
    bitsPerSample:      u32
    duration:           f32
    validBitsPerSample: u16
    dataSize:           u64
    dataSeek:           u64
}
