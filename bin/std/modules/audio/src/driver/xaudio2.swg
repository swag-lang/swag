#global #if #os == Swag.TargetOs.Windows
using Swag, Core, Core.Errors, Win32, XAudio2

struct DriverNative
{
    xAudio2:            *IXAudio2
    masteringVoice:     *IXAudio2MasteringVoice
    hasInitCOM:         bool
    channelMask:        DWORD
    x3DInstance:        X3DAUDIO_HANDLE
}

impl DriverNative
{
    // Create the xaudio2 engine
    mtd createNative(initCOM: bool = true) throw
    {
        Debug.assert(!me.xAudio2)

        var result: HRESULT
        defer #err me.destroyNative()

        if initCOM
        {
            result = CoInitializeEx(null, 0)
            if result != S_OK:
                throw SystemError{"failed to initialize COM", errorID: cast(u64) result}
            me.hasInitCOM = initCOM
        }

        result = XAudio2Create(&me.xAudio2, 0)
        if result != S_OK:
            throw SystemError{"cannot create xaudio2 device", errorID: cast(u64) result}

        result = me.xAudio2.vtbl.createMasteringVoice(me.xAudio2, &me.masteringVoice)
        if result != S_OK:
            throw SystemError{"cannot create xaudio2 mastering voice", errorID: cast(u64) result}

        var debug: XAUDIO2_DEBUG_CONFIGURATION
        debug.traceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS
        //debug.breakMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS
        me.xAudio2.vtbl.setDebugConfiguration(me.xAudio2, &debug, null)

        result = me.masteringVoice.vtbl.getChannelMask(me.masteringVoice, &me.channelMask)
        if result != S_OK:
            throw SystemError{"cannot get xaudio2 channel mask", errorID: cast(u64) result}

        result = X3DAudioInitialize(me.channelMask, X3DAUDIO_SPEED_OF_SOUND, me.x3DInstance)
        if result != S_OK:
            throw SystemError{"cannot initialize X3D audio", errorID: cast(u64) result}

        // Context for xaudio2 threads
        let cxt = @getcontext()
        g_ThreadCxt = dref cxt
    }

    // Destroys the xaudio2 engine
    mtd destroyNative()
    {
        if me.masteringVoice
        {
            me.masteringVoice.vtbl.destroyVoice(cast() me.masteringVoice)
            me.masteringVoice = null
        }

        if me.xAudio2
        {
            discard me.xAudio2.vtbl.release(me.xAudio2)
            me.xAudio2 = null
        }

        if me.hasInitCOM
        {
            CoUninitialize()
            me.hasInitCOM = false
        }
    }
}

impl Driver
{
    mtd setOutputVolume(volume: f32) throw
    {
        if !me.masteringVoice:
            return
        let hr = me.masteringVoice.vtbl.setVolume(cast() me.masteringVoice, volume, 0)
        if hr != S_OK:
            throw SystemError{"setOutputVolume", errorID: cast(u64) hr}
    }

    mtd getOutputVolume()->f32
    {
        if !me.masteringVoice:
            return 0
        var volume: f32
        me.masteringVoice.vtbl.getVolume(cast() me.masteringVoice, &volume)
        return volume
    }
}

///////////////////////////////////////////////////////////////
// XAUDIO2 CALLBACKS
///////////////////////////////////////////////////////////////

private
{
    var g_ThreadCxt: Swag.Context
    var g_Cb:        IXAudio2VoiceCallback
    var g_Cbi:       IXAudio2VoiceCallbackItf

    func onVoiceProcessingPassStart(_me: *IXAudio2VoiceCallback, _bytesRequired: UINT32) {}
    func onVoiceProcessingPassEnd(_me: *IXAudio2VoiceCallback) {}
    func onStreamEnd(_me: *IXAudio2VoiceCallback) {}
    func onBufferStart(_me: *IXAudio2VoiceCallback, _pBufferContext: *void) {}
    func onLoopEnd(_me: *IXAudio2VoiceCallback, _pBufferContext: *void) {}
    func onVoiceError(_me: *IXAudio2VoiceCallback, _pBufferContext: *void, _error: HRESULT) {}

    func onBufferEnd(_me: *IXAudio2VoiceCallback, pBufferContext: *void)
    {
        let voice = cast(*Voice) pBufferContext
        if voice.hasState(.PendingDestroy):
            return

        // Called by xaudio2 thread, so we must have a valid Swag context
        if !@getcontext():
            @setcontext(&g_ThreadCxt)

        g_Driver.updateVoice(voice, .UpdateBuffers)
    }
}

#init
{
    g_Cb.vtbl                        = &g_Cbi
    g_Cbi.onVoiceProcessingPassStart = @mkcallback(&onVoiceProcessingPassStart)
    g_Cbi.onVoiceProcessingPassEnd   = @mkcallback(&onVoiceProcessingPassEnd)
    g_Cbi.onStreamEnd                = @mkcallback(&onStreamEnd)
    g_Cbi.onBufferStart              = @mkcallback(&onBufferStart)
    g_Cbi.onBufferEnd                = @mkcallback(&onBufferEnd)
    g_Cbi.onLoopEnd                  = @mkcallback(&onLoopEnd)
    g_Cbi.onVoiceError               = @mkcallback(&onVoiceError)
}

///////////////////////////////////////////////////////////////
// VOICE
///////////////////////////////////////////////////////////////

public alias VoiceHandle = *IXAudio2SourceVoice

impl Voice
{
    func createNative(voice: *Voice, createFlags = VoiceCreateFlags.Default) throw
    {
        Debug.assert(g_Driver.xAudio2 != null, "driver should be created first")

        let file = voice.file

        var wfx: WAVEFORMATEXTENSIBLE
        wfx.format.nChannels      = cast(u16) file.channelCount
        wfx.format.nSamplesPerSec = cast(u16) file.frequency
        wfx.format.wBitsPerSample = cast(u16) file.bitsPerSample
        wfx.format.wFormatTag     = FORMAT_EXTENSIBLE
        wfx.format.cbSize         = #sizeof(WAVEFORMATEXTENSIBLE) - #sizeof(WAVEFORMATEX)
        wfx.dwChannelMask         = file.channelMask

        switch file.encoding
        {
        case Pcm8, Pcm16, Pcm24, Pcm32:
            wfx.subFormat           = Wav.GUID_PCM
            voice.requestedEncoding = file.encoding
        case Float32:
            wfx.subFormat           = Wav.GUID_IEEE_FLOAT
            voice.requestedEncoding = file.encoding
        default:
            wfx.subFormat             = Wav.GUID_PCM
            wfx.format.wBitsPerSample = 16
            voice.requestedEncoding   = .Pcm16
        }

        wfx.format.nBlockAlign          = cast(u16) (wfx.format.nChannels * (wfx.format.wBitsPerSample / 8))
        wfx.format.nAvgBytesPerSec      = wfx.format.nBlockAlign * wfx.format.nSamplesPerSec
        wfx.samples.wValidBitsPerSample = wfx.format.wBitsPerSample

        var flags: DWORD
        if createFlags.has(.AcceptFilters):
            flags |= XAUDIO2_VOICE_USEFILTER
        if !createFlags.has(.AcceptPitch):
            flags |= XAUDIO2_VOICE_NOPITCH
        let hr = g_Driver.xAudio2.vtbl.createSourceVoice(g_Driver.xAudio2, &voice.handle, cast() &wfx.format, flags, XAUDIO2_DEFAULT_FREQ_RATIO, &g_Cb, null, null)
        if hr != S_OK:
            throw SystemError{"createSourceVoice", errorID: cast(u64) hr}
    }

    mtd destroyNative()
    {
        if !me.handle:
            return
        me.handle.vtbl.destroyVoice(cast() me.handle)
        me.handle = null
    }

    mtd updateNative()->bool throw
    {
        if !me.handle:
            return false

        var voiceState: XAUDIO2_VOICE_STATE
        me.handle.vtbl.getState(me.handle, &voiceState, 0)
        while voiceState.buffersQueued < NumDecodedBuffers - 1
        {
            let (ptrBuffer, sizeBuffer) = me.readBuffer()
            if !sizeBuffer:
                break

            var buffer: XAUDIO2_BUFFER
            buffer.pAudioData = ptrBuffer
            buffer.audioBytes = cast() sizeBuffer
            buffer.pContext   = me
            let hr = me.handle.vtbl.submitSourceBuffer(me.handle, &buffer, null)
            if hr != S_OK:
                throw SystemError{"submitSourceBuffer", errorID: cast(u64) hr}
            me.handle.vtbl.getState(me.handle, &voiceState, 0)
        }

        return voiceState.buffersQueued != 0
    }

    mtd playNative(batchID: u32 = 0) throw
    {
        if !me.handle:
            return
        let hr = me.handle.vtbl.start(me.handle, 0, batchID)
        if hr != S_OK:
            throw SystemError{"start", errorID: cast(u64) hr}
    }

    mtd stopNative(batchID: u32 = 0) throw
    {
        if !me.handle:
            return
        let hr = me.handle.vtbl.stop(me.handle, 0, batchID)
        if hr != S_OK:
            throw SystemError{"stop", errorID: cast(u64) hr}
    }
}

public impl Voice
{
    // Set the playing voice volume between [0..1].
    mtd setVolume(volume: f32, batchID: u32 = 0) throw
    {
        if !me.handle:
            return
        let v  = Math.clamp(volume, 0, 1)
        let hr = me.handle.vtbl.setVolume(cast() me.handle, v, batchID)
        if hr != S_OK:
            throw SystemError{"setVolume", errorID: cast(u64) hr}
    }

    // Returns the actual volume.
    mtd getVolume()->f32 throw
    {
        if !me.handle:
            return 0
        var volume: f32
        me.handle.vtbl.getVolume(cast() me.handle, &volume)
        return volume
    }

    // Set the playing pitch.
    // The voice should have been created with 'VoiceCreateFlags.AcceptPitch'.
    // See [[Voice.create]]
    mtd setFrequencyRatio(ratio: f32, batchID: u32 = 0) throw
    {
        if !me.handle:
            return
        let hr = me.handle.vtbl.setFrequencyRatio(me.handle, ratio, batchID)
        if hr != S_OK:
            throw SystemError{"setFrequencyRatio", errorID: cast(u64) hr}
    }

    // Root a voice to a given list of buses.
    // You can also set 'buses' to null if you want to root the voice only to the
    // main bus (which is the default).
    mtd setRooting(buses: const [..] *Bus) throw
    {
        const MaxBus      = 16
        var sfxSend:      [MaxBus] XAUDIO2_SEND_DESCRIPTOR
        var sfxSendList:  XAUDIO2_VOICE_SENDS
        var pSFXSEndList: *XAUDIO2_VOICE_SENDS

        let count = @countof(buses)
        if count >= MaxBus:
            throw SystemError{"too many buses", errorID: count}

        if count
        {
            foreach bus, i in buses
            {
                Debug.assert(bus != null)
                sfxSend[i].pOutputVoice = cast() bus.handle
            }

            sfxSendList.sendCount = cast() count
            sfxSendList.pSends    = &sfxSend
            pSFXSEndList          = &sfxSendList
        }

        let hr = me.handle.vtbl.setOutputVoices(cast() me.handle, pSFXSEndList)
        if hr != S_OK:
            throw SystemError{"setOutputVoices", errorID: cast(u64) hr}
    }
}

///////////////////////////////////////////////////////////////
// BUS
///////////////////////////////////////////////////////////////

public alias BusHandle = *IXAudio2SubmixVoice

impl Bus
{
    func createNative(bus: *Bus, numChannels: u32, parent: *Bus = null) throw
    {
        var sfxSend:      XAUDIO2_SEND_DESCRIPTOR
        var sfxSendList:  XAUDIO2_VOICE_SENDS
        var pSFXSEndList: *XAUDIO2_VOICE_SENDS

        if parent
        {
            if parent.handle == null:
                throw BadParameterError{"parent bus invalid"}

            sfxSend.pOutputVoice  = cast() parent.handle
            sfxSendList.sendCount = 1
            sfxSendList.pSends    = &sfxSend
            pSFXSEndList          = &sfxSendList
        }

        let hr = g_Driver.xAudio2.vtbl.createSubmixVoice(g_Driver.xAudio2, &bus.handle, numChannels, 44100, XAUDIO2_VOICE_USEFILTER, 0, pSFXSEndList, null)
        if hr != S_OK:
            throw SystemError{"createSubmixVoice", errorID: cast(u64) hr}
    }

    mtd destroyNative()
    {
        if !me.handle:
            return
        me.handle.vtbl.destroyVoice(cast() me.handle)
        me.handle = null
    }
}

public impl Bus
{
    // Set the playing bus volume between [0..1]
    mtd setVolume(volume: f32, batchID: u32 = 0) throw
    {
        if !me.handle:
            return
        let v  = Math.clamp(volume, 0, 1)
        let hr = me.handle.vtbl.setVolume(cast() me.handle, v, batchID)
        if hr != S_OK:
            throw SystemError{"setVolume", errorID: cast(u64) hr}
    }

    // Returns the actual volume
    mtd getVolume()->f32 throw
    {
        if !me.handle:
            return 0
        var volume: f32
        me.handle.vtbl.getVolume(cast() me.handle, &volume)
        return volume
    }
}
