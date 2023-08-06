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

        var debug: XAUDIO2_DEBUG_CONFIGURATION
        debug.traceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS
        debug.breakMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS
        xAudio2.vtbl.setDebugConfiguration(xAudio2, &debug, null)

        result = masteringVoice.vtbl.getChannelMask(masteringVoice, &channelMask)
        if result != S_OK
            throw "cannot get xaudio2 channel mask"

        result = X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, x3DInstance)
        if result != S_OK
            throw "cannot initialize X3D audio"

        // Context for xaudio2 threads
        let cxt = @getcontext()
        g_ThreadCxt = dref cxt
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
    var g_ThreadCxt:    Swag.Context
    var g_Cb:           IXAudio2VoiceCallback
    var g_Cbi:          IXAudio2VoiceCallbackItf

    func onVoiceProcessingPassStart(_me: *IXAudio2VoiceCallback, _bytesRequired: UINT32) {}
    func onVoiceProcessingPassEnd(_me: *IXAudio2VoiceCallback) {}
    func onStreamEnd(_me: *IXAudio2VoiceCallback) {}
    func onBufferStart(_me: *IXAudio2VoiceCallback, _pBufferContext: *void) {}
    func onLoopEnd(_me: *IXAudio2VoiceCallback, _pBufferContext: *void) {}
    func onVoiceError(_me: *IXAudio2VoiceCallback, _pBufferContext: *void, _error: HRESULT) {}

    func onBufferEnd(_me: *IXAudio2VoiceCallback, pBufferContext: *void)
    {
        let voice = cast(*Voice) pBufferContext
        if voice.hasState(.PendingDestroy)
            return

        // Called by xaudio2 thread, so we must have a valid Swag context
        if !@getcontext()
            @setcontext(&g_ThreadCxt)

        g_Driver.updateVoice(voice, .UpdateBuffers)
    }
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

impl Driver
{
    mtd setOutputVolume(volume: f32) throw
    {
        if !masteringVoice
            return
        let hr = masteringVoice.vtbl.setVolume(acast masteringVoice, volume, 0)
        if hr != S_OK
            throw "error"
    }

    mtd getOutputVolume()->f32
    {
        if !masteringVoice
            return 0
        var volume: f32
        masteringVoice.vtbl.getVolume(acast masteringVoice, &volume)
        return volume
    }
}

///////////////////////////////////////////////////////////////
// VOICE
///////////////////////////////////////////////////////////////

public typealias VoiceHandle = *IXAudio2SourceVoice

impl Voice
{
    // Creates a new voice based on the given file.
    func createNative(voice: *Voice, createFlags = VoiceCreateFlags.Default) throw
    {
        let file = voice.file

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

        switch (file.encoding)
        {
        case Pcm:       wfx.subFormat = Wav.GUID_PCM
        case FloatIEEE: wfx.subFormat = Wav.GUID_FLOAT_IEEE
        default:        throw "invalid wav format"
        }

        var flags: DWORD
        if createFlags.has(.AcceptFilters)
            flags |= XAUDIO2_VOICE_USEFILTER
        if !createFlags.has(.AcceptPitch)
            flags |= XAUDIO2_VOICE_NOPITCH
        let hr = g_Driver.xAudio2.vtbl.createSourceVoice(g_Driver.xAudio2, &voice.handle, acast &wfx.format, flags, XAUDIO2_DEFAULT_FREQ_RATIO, &g_Cb, null, null)
        if hr != S_OK
            throw "cannot create source voice"
    }

    // Destroy the voice
    mtd destroyNative()
    {
        if !handle
            return
        handle.vtbl.destroyVoice(acast handle)
        handle = null
    }

    // Update the voice
    mtd update() throw
    {
        if !handle
            return

        var voiceState: XAUDIO2_VOICE_STATE
        handle.vtbl.getState(handle, &voiceState, 0)

        while voiceState.buffersQueued < StreamingBufCount - 1
        {
            if !streamingBuf[curStreamBuffer].buffer
                streamingBuf[curStreamBuffer].resize(StreamingBufSize)

            let destBuffer = streamingBuf[curStreamBuffer].buffer
            let readSize   = Decode.decodeData(self, destBuffer, StreamingBufSize)

            let endFile = readSize == 0

            // Loop
            if endFile and playFlags.has(.Loop)
            {
                Decode.rewindData(self)
                if !readSize
                    continue
            }

            if endFile
                break

            var buffer: XAUDIO2_BUFFER
            buffer.pAudioData = destBuffer
            buffer.flags      = endFile and !playFlags.has(.Loop) ? XAUDIO2_END_OF_STREAM : 0
            buffer.audioBytes = acast readSize
            buffer.pContext   = self
            let hr = handle.vtbl.submitSourceBuffer(handle, &buffer, null)
            if hr != S_OK
                throw "error"

            curStreamBuffer = (curStreamBuffer + 1) % StreamingBufCount
            handle.vtbl.getState(handle, &voiceState, 0)
        }

        // End ?
        if voiceState.buffersQueued == 0
        {
            Decode.closeData(self)
            resetState(.Playing)
        }
    }

    mtd playNative(batchID: u32 = 0) throw
    {
        if !handle
            return
        let hr = handle.vtbl.start(handle, 0, batchID)
        if hr != S_OK
            throw "error"
    }

    mtd stopNative(batchID: u32 = 0) throw
    {
        if !handle
            return
        let hr = handle.vtbl.stop(handle, 0, batchID)
        if hr != S_OK
            throw "error"
    }
}

public impl Voice
{
    // Set the playing voice volume between [0..1]
    mtd setVolume(volume: f32, batchID: u32 = 0) throw
    {
        if !handle
            return
        let v = Math.clamp(volume, 0, 1)
        let hr = handle.vtbl.setVolume(acast handle, v, batchID)
        if hr != S_OK
            throw "error"
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

    // Set the playing pitch
    mtd setFrequencyRatio(ratio: f32, batchID: u32 = 0) throw
    {
        if !handle
            return
        let hr = handle.vtbl.setFrequencyRatio(handle, ratio, batchID)
        if hr != S_OK
            throw "error"
    }

    // Root a voice to a given bus
    mtd setRooting(buses: const [..] *Bus) throw
    {
        const MaxBus = 16
        var sfxSend:        [MaxBus] XAUDIO2_SEND_DESCRIPTOR
        var sfxSendList:    XAUDIO2_VOICE_SENDS
        var pSFXSEndList:   *XAUDIO2_VOICE_SENDS

        let count = @countof(buses)
        if count >= MaxBus
            throw "too many buses"

        if count
        {
            visit bus, i: buses
            {
                Debug.assert(bus != null)
                sfxSend[i].pOutputVoice = acast bus.handle
            }

            sfxSendList.sendCount = acast count
            sfxSendList.pSends    = &sfxSend
            pSFXSEndList          = &sfxSendList
        }

        let hr = handle.vtbl.setOutputVoices(acast handle, pSFXSEndList)
        if hr != S_OK
            throw "error"
    }
}

///////////////////////////////////////////////////////////////
// BUS
///////////////////////////////////////////////////////////////

public typealias BusHandle = *IXAudio2SubmixVoice

impl Bus
{
    func createNative(bus: *Bus, numChannels: u32, parent: *Bus = null) throw
    {
        var sfxSend:        XAUDIO2_SEND_DESCRIPTOR
        var sfxSendList:    XAUDIO2_VOICE_SENDS
        var pSFXSEndList:   *XAUDIO2_VOICE_SENDS

        if parent
        {
            if parent.handle == null
                throw "parent bus invalid"

            sfxSend.pOutputVoice  = acast parent.handle
            sfxSendList.sendCount = 1
            sfxSendList.pSends    = &sfxSend
            pSFXSEndList          = &sfxSendList
        }

        let hr = g_Driver.xAudio2.vtbl.createSubmixVoice(g_Driver.xAudio2, &bus.handle, numChannels, 44100, XAUDIO2_VOICE_USEFILTER, 0, pSFXSEndList, null)
        if hr != S_OK
            throw "failed to create mix voice"
    }

    // Destroy bus
    mtd destroyNative()
    {
        if !handle
            return
        handle.vtbl.destroyVoice(acast handle)
        handle = null
    }
}
