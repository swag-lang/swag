using Core, Win32

public struct Voice
{
    handle:         VoiceHandle
    file:           *SoundFile
    nativeFormat:   bool
}
