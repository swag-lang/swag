#global public
using Core, Core.Errors

// Determins which informations in the [[SoundFile]] struct are valid.
#[Swag.EnumFlags]
enum SoundFileValidityMask
{
    Zero               = 0
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

// SoundFile internal format.
enum SoundFileEncoding
{
    Unknown
    Pcm8        // Pcm, uncompressed, 8 bits per sample
    Pcm16       // Pcm, uncompressed, 16 bits per sample
    Pcm24       // Pcm, uncompressed, 24 bits per sample
    Pcm32       // Pcm, uncompressed, 32 bits per sample
    Float32     // Pcm, uncompressed, float 32 bits per sample
}

// Represents a sound file.
// The 'SoundFile' is not necessary fully loaded in memory, in case we want it to be streamed.
struct SoundFile
{
    fullname:               String
    datas:                  Array'u8                        // Prefetched datas (in encoding format)

    sampleCount:            u64                             // Total number of samples
    dataSize:               u64                             // Total size, in bytes, of datas
    dataSeek:               u64                             // The position in the file where the datas are stored

    encoding                = SoundFileEncoding.Unknown     // Encoding type of the datas

    validity:               SoundFileValidityMask           // What informations in this struct are valid
    frequency:              u32                             // Sound frequency
    channelCount:           u32                             // Number of channels (2 for stereo...)
    channelMask:            u32                             // Identifier of the channels
    bitsPerSample:          u32                             // Number of bits per sample in the datas
    duration:               f32                             // Duration, in seconds, of the sound
    validBitsPerSample:     u16                             // Number of valid bits per sample (<= bitsPerSample)
}

impl SoundFile
{
    // Load a 'SoundFile' from disk.
    // Will load the sound datas if 'loadDatas' is true.
    // Will load the sound metadatas if 'loadMetaData' is true.
    func load(fullname: string, loadDatas = true, loadMetaDatas = false)->SoundFile throw
    {
        let ext = Path.getExtensionLowerCase(fullname)

        // Check format
        switch ext
        {
        case ".wav":
            break
        default:
            throw UnsupportedFormatError{"unsupported file extension"}
        }

        var result: retval
        var stream  = File.open(fullname, File.FileMode.Open, File.FileAccess.Read, File.FileShare.Read)
        defer stream.close()

        switch ext
        {
        case ".wav":
            Wav.loadFile(&result, &stream, loadDatas, loadMetaDatas)
        }

        result.fullname = fullname
        return result
    }
}
