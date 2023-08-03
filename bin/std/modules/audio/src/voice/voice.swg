using Core, Win32

public struct Voice
{
    handle:         VoiceHandle
    file:           *SoundFile

    idxInList:      u32
    mustLoop:       bool
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
    func create(file: *SoundFile)->*Voice throw
    {
        let voice = Memory.new'Voice()

        voice.createNative(file)
        voice.setVolume(g_Driver.globalVolume, 0)

        voice.file = file
        g_Driver.registerVoice(voice)

        return voice
    }

    // Play a sound file
    func play(file: *SoundFile, mustLoop = false) throw
    {
        let voice = create(file)
        voice.mustLoop = mustLoop
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
