#global if #os == Swag.TargetOs.Windows
using Core, Win32, XAudio2

#[Swag.Opaque]
public struct DriverNative
{
    xAudio2:          *IXAudio2
    masteringVoice:   *IXAudio2MasteringVoice
    hasInitCOM:       bool
    channelMask:      DWORD
    x3DInstance:      X3DAUDIO_HANDLE
}

impl DriverNative
{
    // Create the xaudio2 engine
    mtd createNative(initCOM: bool = true) throw
    {
        Debug.assert(!xAudio2)

        var result: HRESULT
        defer(err) destroyNative()

        if initCOM
        {
            result = CoInitializeEx(null, 0)
            if result != S_OK
                throw "failed to initialize COM"
            hasInitCOM = initCOM
        }

        result = XAudio2Create(&xAudio2, 0)
        if result != S_OK
            throw "cannot create xaudio2 device"

        result = xAudio2.vtbl.createMasteringVoice(xAudio2, &masteringVoice)
        if result != S_OK
            throw "cannot create xaudio2 mastering voice"

        //var debug: XAUDIO2_DEBUG_CONFIGURATION
        //debug.traceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS
        //debug.breakMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS
        //xAudio2.vtbl.setDebugConfiguration(xAudio2, &debug, null)

        result = masteringVoice.vtbl.getChannelMask(masteringVoice, &channelMask)
        if result != S_OK
            throw "cannot get xaudio2 channel mask"

        result = X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, x3DInstance)
        if result != S_OK
            throw "cannot initialize X3D audio"
    }

    // Destroys the xaudio2 engine
    mtd destroyNative()
    {
        if masteringVoice
        {
            masteringVoice.vtbl.destroyVoice(acast masteringVoice)
            masteringVoice = null
        }

        if xAudio2
        {
            discard xAudio2.vtbl.release(xAudio2)
            xAudio2 = null
        }

        if hasInitCOM
        {
            CoUninitialize()
            hasInitCOM = false
        }
    }
}

// Defines play callbacks
private
{
    var g_Cb:  IXAudio2VoiceCallback
    var g_Cbi: IXAudio2VoiceCallbackItf

    func onVoiceProcessingPassStart(_me: *IXAudio2VoiceCallback, _bytesRequired: UINT32) {}
    func onVoiceProcessingPassEnd(_me: *IXAudio2VoiceCallback) {}
    func onStreamEnd(_me: *IXAudio2VoiceCallback) {}
    func onBufferStart(_me: *IXAudio2VoiceCallback, _pBufferContext: *void) {}
    func onLoopEnd(_me: *IXAudio2VoiceCallback, _pBufferContext: *void) {}
    func onVoiceError(_me: *IXAudio2VoiceCallback, _pBufferContext: *void, _error: HRESULT) {}
    func onBufferEnd(_me: *IXAudio2VoiceCallback, pBufferContext: *void) { g_Driver.updateVoice(cast(*Voice) pBufferContext); }
}

#init
{
    g_Cb.vtbl = &g_Cbi
    g_Cbi.onVoiceProcessingPassStart = @mkcallback(&onVoiceProcessingPassStart)
    g_Cbi.onVoiceProcessingPassEnd   = @mkcallback(&onVoiceProcessingPassEnd)
    g_Cbi.onStreamEnd                = @mkcallback(&onStreamEnd)
    g_Cbi.onBufferStart              = @mkcallback(&onBufferStart)
    g_Cbi.onBufferEnd                = @mkcallback(&onBufferEnd)
    g_Cbi.onLoopEnd                  = @mkcallback(&onLoopEnd)
    g_Cbi.onVoiceError               = @mkcallback(&onVoiceError)
}

impl Voice
{
    mtd stream() throw
    {
        if !handle
            return

        var voiceState: XAUDIO2_VOICE_STATE
        handle.vtbl.getState(handle, &voiceState, 0)

        while voiceState.buffersQueued < StreamingBufCount - 1
        {
            if pendingDestroy
                break

            if !streamingBuf[curStreamBuffer].buffer
                streamingBuf[curStreamBuffer].resize(StreamingBufSize)

            let destBuffer = streamingBuf[curStreamBuffer].buffer
            let readSize   = file.loadData(destBuffer, StreamingBufSize)
            let endFile    = readSize == 0

            // Loop
            if endFile and mustLoop
            {
                file.rewindData()
                if !readSize
                    continue
            }

            if endFile
                break

            var buffer: XAUDIO2_BUFFER
            buffer.pAudioData = destBuffer
            buffer.flags      = endFile and !mustLoop ? XAUDIO2_END_OF_STREAM : 0
            buffer.audioBytes = acast readSize
            buffer.pContext   = self
            let hr = handle.vtbl.submitSourceBuffer(handle, &buffer, null)
            if hr != S_OK
                throw "error"

            curStreamBuffer = (curStreamBuffer + 1) % StreamingBufCount
            handle.vtbl.getState(handle, &voiceState, 0)
        }
    }
}

public typealias VoiceHandle = *IXAudio2SourceVoice

public impl Voice
{
    // Creates a new voice based on the given file.
    func create(file: *SoundFile)->*Voice throw
    {
        var wfx: WAVEFORMATEXTENSIBLE
        wfx.format.nChannels            = cast(u16) file.channelCount
        wfx.format.nSamplesPerSec       = cast(u16) file.frequency
        wfx.format.wBitsPerSample       = cast(u16) file.bitsPerSample
        wfx.format.nBlockAlign          = cast(u16) (wfx.format.nChannels * (wfx.format.wBitsPerSample / 8))
        wfx.format.nAvgBytesPerSec      = wfx.format.nBlockAlign * wfx.format.nSamplesPerSec
        wfx.format.wFormatTag           = FORMAT_EXTENSIBLE
        wfx.format.cbSize               = @sizeof(WAVEFORMATEXTENSIBLE) - @sizeof(WAVEFORMATEX)
        wfx.samples.wValidBitsPerSample = wfx.format.wBitsPerSample
        wfx.dwChannelMask               = file.channelMask

        var voice = Memory.new'Voice()
        switch (file.format)
        {
        case Pcm:
            voice.nativeFormat = true
            wfx.subFormat = Wav.GUID_PCM
        case FloatIEEE:
            voice.nativeFormat = true
            wfx.subFormat = Wav.GUID_FLOAT_IEEE
        default:
            throw "invalid wav format"
        }

        var cb: *IXAudio2VoiceCallback
        if !voice.nativeFormat or file.datas.count != file.dataSize
        {
            voice.mustStream = true
            cb = &g_Cb
        }

        let hr = g_Driver.xAudio2.vtbl.createSourceVoice(g_Driver.xAudio2, &voice.handle, acast &wfx.format, XAUDIO2_VOICE_USEFILTER, XAUDIO2_DEFAULT_FREQ_RATIO, cb, null, null)
        if hr != S_OK
            throw "cannot create source voice"

        voice.file = file
        g_Driver.registerVoice(voice)

        return voice
    }

    // Destroy the voice
    mtd destroy()
    {
        if !handle
            return
        handle.vtbl.destroyVoice(acast handle)
        handle = null
    }

    // Play the sound
    mtd play(mustLoop = false, firstSample: u32 = 0, batchID: u32 = 0) throw
    {
        if !handle
            return

        self.mustLoop = #self.mustLoop

        // Submit the initial buffer, if available
        let dataSize = file.datas.count
        if dataSize and nativeFormat
        {
            var buffer: XAUDIO2_BUFFER
            buffer.pAudioData     = file.datas.buffer
            buffer.flags          = dataSize == file.dataSize ? XAUDIO2_END_OF_STREAM : 0
            buffer.audioBytes     = acast dataSize
            buffer.playBegin      = firstSample
            buffer.pContext       = self
            if self.mustLoop and dataSize == file.dataSize
                buffer.loopCount = XAUDIO2_LOOP_INFINITE

            let hr = handle.vtbl.submitSourceBuffer(handle, &buffer, null)
            if hr != S_OK
                throw "cannot play"
        }

        resume(batchID)

        if mustStream
            g_Driver.updateVoice(self)
    }

    // Resume play
    mtd resume(batchID: u32 = 0) throw
    {
        if !handle
            return
        if playing
            return
        playing = true
        let hr = handle.vtbl.start(handle, 0, batchID)
        if hr != S_OK
            throw "error"
    }

    // Stop the playing voice
    mtd stop(batchID: u32 = 0) throw
    {
        if !handle
            return
        if !playing
            return
        playing = false
        let hr = handle.vtbl.stop(handle, 0, batchID)
        if hr != S_OK
            throw "error"
    }

    // Set the playing voice volume between [0..1]
    mtd setVolume(volume: f32, batchID: u32 = 0) throw
    {
        if !handle
            return
        let hr = handle.vtbl.setVolume(acast handle, Math.clamp(volume, 0, 1), batchID)
        if hr != S_OK
            throw "error"
    }

    // Set the playing voice volume
    mtd setVolumeDB(volumeDB: f32, batchID: u32 = 0) throw
    {
        setVolume(convertDBToPercent(volumeDB), batchID)
    }

    // Returns the actual volume
    mtd getVolume()->f32 throw
    {
        if !handle
            return 0
        var volume: f32
        handle.vtbl.getVolume(acast handle, &volume)
        return volume
    }

    // Returns the actual volume, in DB
    mtd getVolumeDB()->f32 throw
    {
        return convertPercentToDB(getVolume())
    }

    // Set the playing pitch
    mtd setFrequencyRatio(ratio: f32, batchID: u32 = 0) throw
    {
        if !handle
            return
        let hr = handle.vtbl.setFrequencyRatio(handle, ratio, batchID)
        if hr != S_OK
            throw "error"
    }

    // Returns true if the voice is currently playing
    mtd isPlaying()->bool
    {
        if !handle
            return false
        if pendingStream
            return true
        if !playing
            return false
        var voiceState: XAUDIO2_VOICE_STATE
        handle.vtbl.getState(handle, &voiceState, 0)
        playing = voiceState.buffersQueued != 0
        return playing
    }

    // Stop the playing loop
    mtd breakLoop(batchID: u32 = 0) throw
    {
        if !handle
            return
        let hr = handle.vtbl.exitLoop(handle, batchID)
        if hr != S_OK
            throw "error"
    }
}