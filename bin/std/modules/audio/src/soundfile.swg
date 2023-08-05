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
    sampleCount:        u64
    validity:           SoundFileValidityMask
    encoding            = SoundFileEncoding.Unknown
    frequency:          u32
    channelCount:       u32
    channelMask:        u32
    bitsPerSample:      u32
    duration:           f32
    validBitsPerSample: u16
    dataSize:           u64
    dataSeek:           u64

    stream: File.FileStream
    datas:  Array'u8
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