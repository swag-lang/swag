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
    Pcm
    FloatIEEE
}

// Represents a sound file.
// The 'SoundFile' is not necessary fully loaded in memory, in case we want it to be streamed.
struct SoundFile
{
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

    stream: File.FileStream     // Associated stream, when loading
    datas:  Array'u8            // Optional datas
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
        result.stream = File.open(fullname, File.FileMode.Open, File.FileAccess.Read, File.FileShare.Read)
        defer result.stream.close()

        switch ext
        {
        case ".wav": Wav.loadFile(&result, loadDatas, loadMetaDatas)
        }

        return result
    }
}