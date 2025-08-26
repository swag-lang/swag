#global marked
using Core, Win32

#[Swag.EnumFlags]
public enum VoiceCreateFlags
{
    Zero          = 0
    AcceptPitch
    AcceptFilters
    Default       = AcceptFilters | AcceptPitch
}

#[Swag.EnumFlags]
public enum VoicePlayFlags
{
    Zero          = 0
    Loop                              // Play in loops
    DestroyOnStop                     // Destroy the voice once the sound has been played
    Default       = DestroyOnStop
}

#[Swag.EnumFlags]
public enum VoiceState
{
    Zero           = 0
    PlayedOnce
    Playing
    PendingDestroy
}

// Represents a playing sound
#[Swag.Opaque]
public struct Voice
{
    handle:                     VoiceHandle
    file:                       *SoundFile

    requestedEncoding           = SoundFileEncoding.Unknown
    codec:                      ICodec

    playFlags:                  VoicePlayFlags
    state:                      VoiceState
    idxInList:                  u32
    const StreamBufferSize      = 16 * 1024'u64
    stream:                     File.FileStream
    streamCurSeek:              u64
    streamBuffer:               Array'u8
    const NumDecodedBuffers     = 3
    const DecodedBufferSize     = 65536'u64
    decodedBuffers:             [NumDecodedBuffers] Array'u8
    decodedBufferIdx:           u32
}

internal impl Voice
{
    mtd recycle()
    {
        me.file              = null
        me.playFlags         = .Zero
        me.state             = .Zero
        me.idxInList         = 0
        me.streamCurSeek     = 0
        me.decodedBufferIdx  = 0
        me.requestedEncoding = .Unknown
        me.destroyCodec()
    }

    mtd hasState(st: VoiceState)->bool
    {
        return Atomic.get(cast(*u32) &me.state) & cast(u32) st ? true : false
    }

    mtd setState(st: VoiceState)
    {
        Atomic.logicOr(cast(*u32) &me.state, cast(u32) st)
    }

    mtd resetState(st: VoiceState)
    {
        Atomic.logicAnd(cast(*u32) &me.state, ~cast(u32) st)
    }
}

public impl Voice
{
    mtd opDrop()
    {
        me.destroyCodec()
    }

    // Creates a new voice for a given sound file.
    // A voice is what will be actually played. You can have as many voices as you want for one unique [[SoundFile]].
    func create(file: *SoundFile, createFlags = VoiceCreateFlags.Default)->*Voice throw
    {
        let voice = g_Driver.createVoice()
        voice.file = file

        voice.createNative(createFlags)
        g_Driver.updateVoice(voice, .Register)

        return voice
    }

    // Creates a voice and plays it.
    // By default, the voice will be destroyed when stopped or finished.
    #[Swag.Discardable, Swag.Overload]
    func play(file: *SoundFile, createFlags = VoiceCreateFlags.Default, playFlags = VoicePlayFlags.Default)->*Voice throw
    {
        let voice = create(file, createFlags)
        voice.play(playFlags)
        return voice
    }

    // Plays a voice.
    // By default, you will have to destroy the voice yourself when no more needed, for example if [[Voice.isPlaying]] returns false. But if you want the voice to be destroyed automatically when done, set the 'DestroyOnStop' flag in 'playFlags'.
    // See [[Voice.create]]
    #[Swag.Overload]
    mtd play(playFlags = VoicePlayFlags.Zero) throw
    {
        if me.hasState(.Playing):
            return
        me.playFlags = #me.playFlags
        me.playNative()
        me.setState(.Playing)

        if !me.hasState(.PlayedOnce)
        {
            me.setState(.PlayedOnce)
            g_Driver.updateVoice(me, .UpdateBuffers)
        }
    }

    // Pause the playing voice
    mtd pause() throw
    {
        if !me.hasState(.Playing):
            return
        me.stopNative()
        me.resetState(.Playing)
    }

    // Stop the playing voice
    mtd stop() throw
    {
        if !me.hasState(.Playing):
            return
        me.stopNative()
        me.resetState(.Playing)
        if me.playFlags.has(.DestroyOnStop):
            me.destroy()
    }

    // Destroy the voice
    mtd destroy()
    {
        if me.hasState(.PendingDestroy):
            return
        me.setState(.PendingDestroy)
        g_Driver.updateVoice(me, .Destroy)
    }

    // Returns true if the voice is currently playing
    mtd isPlaying()->bool
    {
        if !me.handle:
            return false
        return me.hasState(.Playing)
    }

    // Set the playing voice volume
    mtd setVolumeDB(volumeDB: f32, batchID: u32 = 0) throw
    {
        me.setVolume(convertDBToPercent(volumeDB), batchID)
    }

    // Returns the actual volume, in DB
    mtd getVolumeDB()->f32 throw
    {
        return convertPercentToDB(me.getVolume())
    }
}
