using Core, Win32

public struct Voice
{
    handle:         VoiceHandle
    file:           *SoundFile

    pendingStream:  bool
    playing:        bool
    nativeFormat:   bool
    mustLoop:       bool
    pendingDestroy: bool
    mustStream:     bool

    const StreamingBufCount = 3
    const StreamingBufSize  = 65536'u64

    streamingBuf:       [3] Array'u8
    curStreamBuffer:    u32
}
