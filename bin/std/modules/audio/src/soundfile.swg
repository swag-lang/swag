#global public
using Core

// Determins which informations in the 'SoundFile' struct are valid.
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

// SoundFile internal format.
enum SoundFileEncoding
{
    Unknown
    Pcm8
    Pcm16
    Pcm20
    Pcm24
    Pcm32
    Float32
}

// Represents a sound file.
// The 'SoundFile' is not necessary fully loaded in memory, in case we want it to be streamed.
struct SoundFile
{
    fullname:           String

    sampleCount:        u64     // Total number of samples
    dataSize:           u64     // Total size, in bytes, of datas
    dataSeek:           u64     // The position in the file where the datas are stored

    validity:           SoundFileValidityMask   // What informations in this struct are valid

    encoding            = SoundFileEncoding.Unknown     // Encoding type of the datas
    frequency:          u32     // Sound frequency
    channelCount:       u32     // Number of channels (2 for stereo...)
    channelMask:        u32     // Identifier of the channels
    bitsPerSample:      u32     // Number of bits per sample
    duration:           f32     // Duration, in seconds, of the file
    validBitsPerSample: u16

    datas:  Array'u8            // Prefetched datas (in encoding format)
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
            throw "unsupported file extension"
        }

        var result: retval
        var stream = File.open(fullname, File.FileMode.Open, File.FileAccess.Read, File.FileShare.Read)
        defer stream.close()

        switch ext
        {
        case ".wav": Wav.loadFile(&result, &stream, loadDatas, loadMetaDatas)
        }

        result.fullname = fullname
        return result
    }
}