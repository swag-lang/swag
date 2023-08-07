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

#[Swag.EnumFlags]
public enum VoiceState
{
    Zero = 0
    PlayedOnce
    Playing
    PendingDestroy
}

// Represents a playing sound
#[Swag.Opaque]
public struct Voice
{
    handle:     VoiceHandle
    file:       *SoundFile

    requestedEncoding = SoundFileEncoding.Unknown
    codec: ICodec

    playFlags:  VoicePlayFlags
    state:      VoiceState
    idxInList:  u32

    const StreamBufferSize = 16*1024'u64
    stream:             File.FileStream
    streamCurSeek:      u64
    streamBuffer:       Array'u8

    const NumDecodedBuffers = 3
    const DecodedBufferSize = 65536'u64
    decodedBuffers:     [NumDecodedBuffers] Array'u8
    decodedBufferIdx:   u32
}

internal impl Voice
{
    mtd recycle()
    {
        file              = null
        playFlags         = .Zero
        state             = .Zero
        idxInList         = 0
        streamCurSeek     = 0
        decodedBufferIdx  = 0
        requestedEncoding = .Unknown
        destroyCodec()
    }

    mtd hasState(st: VoiceState)->bool
    {
        return Atomic.get(cast(*u32) &state) & cast(u32) st ? true : false
    }

    mtd setState(st: VoiceState)
    {
        Atomic.or(cast(*u32) &state, cast(u32) st)
    }

    mtd resetState(st: VoiceState)
    {
        Atomic.and(cast(*u32) &state, ~cast(u32) st)
    }
}

public impl Voice
{
    mtd opDrop()
    {
        destroyCodec()
    }

    // Creates a new voice with a given sound file
    func create(file: *SoundFile, createFlags = VoiceCreateFlags.Default)->*Voice throw
    {
        let voice = g_Driver.createVoice()
        voice.file = file

        voice.createNative(createFlags)
        g_Driver.updateVoice(voice, .Register)

        return voice
    }

    // Simple function that will create a voice and play it
    // See [Create] and [Play]
    #[Swag.Discardable]
    func play(file: *SoundFile, createFlags = VoiceCreateFlags.Default, playFlags = VoicePlayFlags.Zero)->*Voice throw
    {
        let voice = create(file, createFlags)
        voice.play(playFlags)
        return voice
    }

    // Play a voice
    mtd play(playFlags = VoicePlayFlags.Zero) throw
    {
        if hasState(.Playing)
            return
        self.playFlags = #self.playFlags
        playNative()
        setState(.Playing)

        if !hasState(.PlayedOnce)
        {
            setState(.PlayedOnce)
            g_Driver.updateVoice(self, .UpdateBuffers)
        }
    }

    // Pause the playing voice
    mtd pause() throw
    {
        if !hasState(.Playing)
            return
        stopNative()
        resetState(.Playing)
    }

    // Stop the playing voice
    mtd stop() throw
    {
        if !hasState(.Playing)
            return
        stopNative()
        resetState(.Playing)
        if playFlags.has(.DestroyOnStop)
            destroy()
    }

    // Destroy the voice
    mtd destroy()
    {
        if hasState(.PendingDestroy)
            return
        setState(.PendingDestroy)
        g_Driver.updateVoice(self, .Destroy)
    }

    // Returns true if the voice is currently playing
    mtd isPlaying()->bool
    {
        if !handle
            return false
        return hasState(.Playing)
    }

    // Set the playing voice volume
    mtd setVolumeDB(volumeDB: f32, batchID: u32 = 0) throw
    {
        setVolume(convertDBToPercent(volumeDB), batchID)
    }

    // Returns the actual volume, in DB
    mtd getVolumeDB()->f32 throw
    {
        return convertPercentToDB(getVolume())
    }
}
