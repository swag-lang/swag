using Core, Win32

#[Swag.EnumFlags]
public enum VoiceCreateFlags
{
    Zero = 0
    AcceptPitch
    AcceptFilters
    Default = AcceptFilters | AcceptPitch
}

#[Swag.EnumFlags]
public enum VoicePlayFlags
{
    Zero = 0
    Loop                // Play in loops
    DestroyOnStop       // Destroy the voice once the sound has been played
}

// Represents a playing sound
public struct Voice
{
    handle:         VoiceHandle
    file:           *SoundFile

    playFlags:      VoicePlayFlags
    idxInList:      u32
    pendingUpdate:  u8
    pendingDestroy: u8
    playing:        u8

    const StreamingBufCount = 3
    const StreamingBufSize  = 65536'u64

    streamingSeek:      u64
    streamingBuf:       [3] Array'u8
    curStreamBuffer:    u32
}

internal impl Voice
{
    mtd streamingDecodeData(destBuffer: ^u8, length: u64)->u64 throw
    {
        switch file.format
        {
        case Pcm, FloatIEEE:
            return Wav.streamingDecodeData(self, destBuffer, length)
        }

        return 0
    }

    mtd streamingRewindData() throw
    {
        switch file.format
        {
        case Pcm, FloatIEEE:
            return Wav.streamingRewindData(self)
        }
    }

    mtd streamingCloseData()
    {
        switch file.format
        {
        case Pcm, FloatIEEE:
            return Wav.streamingCloseData(self)
        }
    }
}


public impl Voice
{
    // Creates a new voice
    func create(file: *SoundFile, createFlags = VoiceCreateFlags.Default)->*Voice throw
    {
        let voice = Memory.new'Voice()
        voice.file = file

        voice.createNative(createFlags)

        g_Driver.registerVoice(voice)
        return voice
    }

    // Play a sound file
    func play(file: *SoundFile, createFlags = VoiceCreateFlags.Default, playFlags = VoicePlayFlags.Zero) throw
    {
        let voice = create(file, createFlags)
        voice.playFlags = playFlags
        voice.play()
    }

    // Destroy the voice
    mtd destroy()
    {
        if Atomic.get(&pendingDestroy)
            return
        Atomic.or(&pendingDestroy, 1)
        g_Driver.updateVoice(self)
    }
}
