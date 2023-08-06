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
public struct Voice
{
    handle:     VoiceHandle
    file:       *SoundFile

    playFlags:  VoicePlayFlags
    state:      VoiceState
    idxInList:  u32

    const StreamingBufCount = 3
    const StreamingBufSize  = 65536'u64

    stream:             File.FileStream
    streamingSeek:      u64
    streamingBuf:       [3] Array'u8
    curStreamBuffer:    u32
}

internal impl Voice
{
    mtd recycle()
    {
        file = null
        playFlags = .Zero
        state = .Zero
        idxInList = 0
        streamingSeek = 0
        curStreamBuffer = 0
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

impl Voice
{
    mtd readBuffer()->{buffer: ^void, size: u64} throw
    {
        // If we are at the end of the file, then nothing to do, except if we need
        // to loop. In that case, we rewind datas to the start
        if streamingSeek >= file.dataSize
        {
             if !playFlags.has(.Loop)
                return {null, 0}
            Decode.rewindData(self)
        }

        // If the file is prefetched and in a driver supported format, just submit the
        // prefetched datas
        if streamingSeek < file.datas.count and Driver.supportsEncoding(file.encoding)
        {
            let readSize = (file.datas.count - streamingSeek)
            streamingSeek = file.datas.count
            return {file.datas.buffer, readSize}
        }

        // Otherwise we need to decode datas in a temporary buffer
        if !streamingBuf[curStreamBuffer].buffer
            streamingBuf[curStreamBuffer].resize(StreamingBufSize)

        let destBuffer = streamingBuf[curStreamBuffer].buffer
        let readSize   = Decode.decodeData(self, destBuffer, StreamingBufSize)

        curStreamBuffer = (curStreamBuffer + 1) % StreamingBufCount
        return {destBuffer, readSize}
    }

    mtd closeBuffer()
    {
        Decode.closeData(self)
    }
}