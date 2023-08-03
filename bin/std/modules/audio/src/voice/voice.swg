using Core, Win32

public struct Voice
{
    handle:         VoiceHandle
    file:           *SoundFile

    idxInList:      u32
    nativeFormat:   bool
    mustLoop:       bool
    mustStream:     bool
    pendingUpdate:  u8
    pendingDestroy: u8
    playing:        u8

    const StreamingBufCount = 3
    const StreamingBufSize  = 65536'u64

    streamingBuf:       [3] Array'u8
    curStreamBuffer:    u32
}

public impl Voice
{
    // Creates a new voice
    func create(file: *SoundFile)->*Voice throw
    {
        let voice = Memory.new'Voice()

        voice.createNative(file)

        voice.file = file
        g_Driver.registerVoice(voice)

        return voice
    }

    // Play a sound file
    func play(file: *SoundFile) throw
    {
        let voice = create(file)
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
