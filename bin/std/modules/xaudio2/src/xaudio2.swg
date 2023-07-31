#global export
#global if #os == Swag.TargetOs.Windows
#foreignlib "xaudio2"
using Swag, Win32

alias UINT32  = u32
alias UINT64  = u64
alias FLOAT32 = f32

const X3DAUDIO_HANDLE_BYTESIZE = 20
alias X3DAUDIO_HANDLE = [X3DAUDIO_HANDLE_BYTESIZE] BYTE

alias XAUDIO2_PROCESSOR = UINT32
const Processor1  = 0x00000001
const Processor2  = 0x00000002
const Processor3  = 0x00000004
const Processor4  = 0x00000008
const Processor5  = 0x00000010
const Processor6  = 0x00000020
const Processor7  = 0x00000040
const Processor8  = 0x00000080
const Processor9  = 0x00000100
const Processor10 = 0x00000200
const Processor11 = 0x00000400
const Processor12 = 0x00000800
const Processor13 = 0x00001000
const Processor14 = 0x00002000
const Processor15 = 0x00004000
const Processor16 = 0x00008000
const Processor17 = 0x00010000
const Processor18 = 0x00020000
const Processor19 = 0x00040000
const Processor20 = 0x00080000
const Processor21 = 0x00100000
const Processor22 = 0x00200000
const Processor23 = 0x00400000
const Processor24 = 0x00800000
const Processor25 = 0x01000000
const Processor26 = 0x02000000
const Processor27 = 0x04000000
const Processor28 = 0x08000000
const Processor29 = 0x10000000
const Processor30 = 0x20000000
const Processor31 = 0x40000000
const Processor32 = 0x80000000
const XAUDIO2_ANY_PROCESSOR = 0xffffffff

const X3DAUDIO_SPEED_OF_SOUND = 343.5

const XAUDIO2_DEFAULT_CHANNELS = 0
const XAUDIO2_DEFAULT_SAMPLERATE = 0

struct XAUDIO2_EFFECT_CHAIN
{
}

struct XAUDIO2_PERFORMANCE_DATA
{
    AudioCyclesSinceLastQuery:    UINT64
    TotalCyclesSinceLastQuery:    UINT64
    MinimumCyclesPerQuantum:      UINT32
    MaximumCyclesPerQuantum:      UINT32
    MemoryUsageInBytes:           UINT32
    CurrentLatencyInSamples:      UINT32
    GlitchesSinceEngineStarted:   UINT32
    ActiveSourceVoiceCount:       UINT32
    TotalSourceVoiceCount:        UINT32
    ActiveSubmixVoiceCount:       UINT32
    ActiveResamplerCount:         UINT32
    ActiveMatrixMixCount:         UINT32
    ActiveXmaSourceVoices:        UINT32
    ActiveXmaStreams:             UINT32
}

struct XAUDIO2_DEBUG_CONFIGURATION
{
    TraceMask:        UINT32
    BreakMask:        UINT32
    LogThreadID:      BOOL
    LogFileline:      BOOL
    LogFunctionName:  BOOL
    LogTiming:        BOOL
}

enum AUDIO_STREAM_CATEGORY
{
    AudioCategory_Other = 0,
    AudioCategory_ForegroundOnlyMedia = 1,
    AudioCategory_BackgroundCapableMedia = 2,
    AudioCategory_Communications = 3,
    AudioCategory_Alerts = 4,
    AudioCategory_SoundEffects = 5,
    AudioCategory_GameEffects = 6,
    AudioCategory_GameMedia = 7,
    AudioCategory_GameChat = 8,
    AudioCategory_Speech = 9,
    AudioCategory_Movie = 10,
    AudioCategory_Media = 11,
}

struct XAUDIO2_VOICE_DETAILS
{
    creationFlags:      UINT32
    activeFlags:        UINT32
    inputChannels:      UINT32
    inputSampleRate:    UINT32
}

struct XAUDIO2_SEND_DESCRIPTOR
{
    flags:          UINT32
    pOutputVoice:   *IXAudio2Voice;
}

struct XAUDIO2_VOICE_SENDS
{
    sendCount:  UINT32
    pSends:     *XAUDIO2_SEND_DESCRIPTOR
}

struct XAUDIO2_BUFFER
{
    flags:      UINT32          // Either 0 or XAUDIO2_END_OF_STREAM.
    audioBytes: UINT32          // Size of the audio data buffer in bytes.
    pAudioData: const *BYTE     // Pointer to the audio data buffer.
    playBegin:  UINT32          // First sample in this buffer to be played.
    playLength: UINT32          // Length of the region to be played in samples, or 0 to play the whole buffer.
    loopBegin:  UINT32          // First sample of the region to be looped.
    loopLength: UINT32          // Length of the desired loop region in samples, or 0 to loop the entire buffer.
    loopCount:  UINT32          // Number of times to repeat the loop region, or XAUDIO2_LOOP_INFINITE to loop forever.
    pContext:   *void           // Context value to be passed back in callbacks.
}

struct XAUDIO2_BUFFER_WMA
{
    pDecodedPacketCumulativeBytes:  const *UINT32;
    PacketCount:                    UINT32
}

struct XAUDIO2_VOICE_STATE
{
    pCurrentBufferContext:  *void
    buffersQueued:          UINT32
    samplesPlayed:          UINT64
}

// Used in XAUDIO2_FILTER_PARAMETERS below
enum XAUDIO2_FILTER_TYPE
{
    LowPassFilter,                      // Attenuates frequencies above the cutoff frequency (state-variable filter).
    BandPassFilter,                     // Attenuates frequencies outside a given range      (state-variable filter).
    HighPassFilter,                     // Attenuates frequencies below the cutoff frequency (state-variable filter).
    NotchFilter,                        // Attenuates frequencies inside a given range       (state-variable filter).
    LowPassOnePoleFilter,               // Attenuates frequencies above the cutoff frequency (one-pole filter, XAUDIO2_FILTER_PARAMETERS.OneOverQ has no effect)
    HighPassOnePoleFilter               // Attenuates frequencies below the cutoff frequency (one-pole filter, XAUDIO2_FILTER_PARAMETERS.OneOverQ has no effect)
}

struct XAUDIO2_FILTER_PARAMETERS
{
    type:       XAUDIO2_FILTER_TYPE
    frequency:  f32
    oneOverQ:   f32
}

struct IXAudio2                 { vtbl: *IXAudio2Itf; }
struct IXAudio2Voice            { vtbl: *IXAudio2VoiceItf; }
struct IXAudio2MasteringVoice   { vtbl: *IXAudio2MasteringVoiceItf; }
struct IXAudio2SourceVoice      { vtbl: *IXAudio2SourceVoiceItf; }
struct IXAudio2EngineCallback   { vtbl: *IXAudio2EngineCallbackItf; }
struct IXAudio2VoiceCallback    { vtbl: *IXAudio2VoiceCallbackItf; }
struct IXAudio2SubmixVoice      { vtbl: *IXAudio2SubmixVoiceItf; }

struct IXAudio2EngineCallbackItf
{
    onProcessingPassStart:  func(me: *IXAudio2EngineCallback)
    onProcessingPassEnd:    func(me: *IXAudio2EngineCallback)
    onCriticalError:        func(me: *IXAudio2EngineCallback, error: HRESULT)
}

struct IXAudio2VoiceCallbackItf
{
    onVoiceProcessingPassStart:     func(me: *IXAudio2VoiceCallback, bytesRequired: UINT32)
    onVoiceProcessingPassEnd:       func(me: *IXAudio2VoiceCallback)
    onStreamEnd:                    func(me: *IXAudio2VoiceCallback)
    onBufferStart:                  func(me: *IXAudio2VoiceCallback, pBufferContext: *void)
    onBufferEnd:                    func(me: *IXAudio2VoiceCallback, pBufferContext: *void)
    onLoopEnd:                      func(me: *IXAudio2VoiceCallback, pBufferContext: *void)
    onVoiceError:                   func(me: *IXAudio2VoiceCallback, pBufferContext: *void, error: HRESULT)
}

struct IXAudio2Itf
{
    queryInterface:         *void
    addRef:                 func(me: *IXAudio2)->ULONG
    release:                func(me: *IXAudio2)->ULONG
    registerForCallbacks:   func(me: *IXAudio2, pCallback: *IXAudio2EngineCallback)
    unregisterForCallbacks: func(me: *IXAudio2, pCallback: *IXAudio2EngineCallback)
    createSourceVoice:      func(me: *IXAudio2, ppSourceVoice: **IXAudio2SourceVoice, pSourceFormat: *const WAVEFORMATEX, flags: UINT32, maxFrequencyRatio: f32, pCallback: *IXAudio2VoiceCallback, pSendList: const *XAUDIO2_VOICE_SENDS, pEffectChain: const *XAUDIO2_EFFECT_CHAIN)->HRESULT
    createSubmixVoice:      func(me: *IXAudio2, ppSubmixVoice: **IXAudio2SubmixVoice, inputChannels: UINT32, inputSampleRate: UINT32, flags: UINT32, processingStage: UINT32, pSendList: const *XAUDIO2_VOICE_SENDS, pEffectChain: const *XAUDIO2_EFFECT_CHAIN)->HRESULT
    createMasteringVoice:   func(me: *IXAudio2, voice: **IXAudio2MasteringVoice, inputChannels: UINT32 = XAUDIO2_DEFAULT_CHANNELS, inputSampleRate: UINT32 = XAUDIO2_DEFAULT_SAMPLERATE, flags: UINT32 = 0, szDeviceId: LPCWSTR = null, effectChain: *XAUDIO2_EFFECT_CHAIN = null, streamCategory = AUDIO_STREAM_CATEGORY.AudioCategory_GameEffects)->HRESULT
    startEngine:            func(me: *IXAudio2)->HRESULT
    stopEngine:             func(me: *IXAudio2)
    commitChanges:          func(me: *IXAudio2, operationSet: UINT32)->HRESULT
    getPerformanceData:     func(me: *IXAudio2, pPerfData: *XAUDIO2_PERFORMANCE_DATA)
    setDebugConfiguration:  func(me: *IXAudio2, pDebugConfiguration: const *XAUDIO2_DEBUG_CONFIGURATION, pReserved: *void)
}

struct IXAudio2VoiceItf
{
    getVoiceDetails:            func(me: *IXAudio2Voice, pVoiceDetails: *XAUDIO2_VOICE_DETAILS)
    setOutputVoices:            func(me: *IXAudio2Voice, pSendList: const *XAUDIO2_VOICE_SENDS)
    setEffectChain:             func(me: *IXAudio2Voice, pEffectChain: const *XAUDIO2_EFFECT_CHAIN)
    enableEffect:               func(me: *IXAudio2Voice, effectIndex: UINT32, operationSet: UINT32)->HRESULT
    disableEffect:              func(me: *IXAudio2Voice, effectIndex: UINT32, operationSet: UINT32)->HRESULT
    getEffectState:             func(me: *IXAudio2Voice, effectIndex: UINT32, pEnabled: *BOOL)
    setEffectParameters:        func(me: *IXAudio2Voice, effectIndex: UINT32, pParameters: const *void, parametersByteSize: UINT32, operationSet: UINT32)->HRESULT
    getEffectParameters:        func(me: *IXAudio2Voice, effectIndex: UINT32, pParameters: *void, parametersByteSize: UINT32)->HRESULT
    setFilterParameters:        func(me: *IXAudio2Voice, pParameters: const *XAUDIO2_FILTER_PARAMETERS, operationSet: UINT32)->HRESULT
    getFilterParameters:        func(me: *IXAudio2Voice, pParameters: *XAUDIO2_FILTER_PARAMETERS)
    setOutputFilterParameters:  func(me: *IXAudio2Voice, pDestinationVoice: *IXAudio2Voice, pParameters: const *XAUDIO2_FILTER_PARAMETERS, operationSet: UINT32)->HRESULT
    getOutputFilterParameters:  func(me: *IXAudio2Voice, pDestinationVoice: *IXAudio2Voice, pParameters: *XAUDIO2_FILTER_PARAMETERS)
    setVolume:                  func(me: *IXAudio2Voice, volume: f32, operationSet: UINT32)->HRESULT
    getVolume:                  func(me: *IXAudio2Voice, pVolume: *f32)
    setChannelVolumes:          func(me: *IXAudio2Voice, channels: UINT32, pVolumes: const *f32, operationSet: UINT32)->HRESULT
    getChannelVolumes:          func(me: *IXAudio2Voice, channels: UINT32, pVolumes: *f32)
    setOutputMatrix:            func(me: *IXAudio2Voice, pDestinationVoice: *IXAudio2Voice, sourceChannels: UINT32, destinationChannels: UINT32, pLevelMatrix: const *f32, operationSet: UINT32)->HRESULT
    getOutputMatrix:            func(me: *IXAudio2Voice, pDestinationVoice: *IXAudio2Voice, sourceChannels: UINT32, destinationChannels: UINT32, pLevelMatrix: *f32)
    destroyVoice:               func(me: *IXAudio2Voice)
}

struct IXAudio2MasteringVoiceItf
{
    using voice: IXAudio2VoiceItf
    getChannelMask: func(me: *IXAudio2MasteringVoice, pChannelmask: *DWORD)->HRESULT;
}

struct IXAudio2SubmixVoiceItf
{
    using voice: IXAudio2VoiceItf
}

struct IXAudio2SourceVoiceItf
{
    using voice: IXAudio2VoiceItf
    start:               func(me: *IXAudio2SourceVoice, flags: UINT32, operationSet: UINT32)->HRESULT
    stop:                func(me: *IXAudio2SourceVoice, flags: UINT32, operationSet: UINT32)->HRESULT
    submitSourceBuffer:  func(me: *IXAudio2SourceVoice, pBuffer: const *XAUDIO2_BUFFER, pBufferWMA: const *XAUDIO2_BUFFER_WMA)->HRESULT
    glushSourceBuffers:  func(me: *IXAudio2SourceVoice)->HRESULT
    discontinuity:       func(me: *IXAudio2SourceVoice)->HRESULT
    exitLoop:            func(me: *IXAudio2SourceVoice, OperationSet: UINT32)->HRESULT
    getState:            func(me: *IXAudio2SourceVoice, pVoiceState: *XAUDIO2_VOICE_STATE, flags: UINT32)
    setFrequencyRatio:   func(me: *IXAudio2SourceVoice, ratio: f32, operationSet: UINT32)
    getFrequencyRatio:   func(me: *IXAudio2SourceVoice, pRatio: *f32)
    setSourceSampleRate: func(me: *IXAudio2SourceVoice, newSourceSampleRate: UINT32)->HRESULT
}

struct WAVEFORMATEX
{
    wFormatTag:         WORD    /* format type */
    nChannels:          WORD    /* number of channels (i.e. mono, stereo...) */
    nSamplesPerSec:     DWORD   /* sample rate */
    nAvgBytesPerSec:    DWORD   /* for buffer estimation */
    nBlockAlign:        WORD    /* block size of data */
    wBitsPerSample:     WORD    /* number of bits per sample of mono data */
    cbSize:             WORD    /* the count in bytes of the size of */
}

var g_XAudio2:          *IXAudio2
var g_MasteringVoice:   *IXAudio2MasteringVoice
var g_InitCOM:          bool
var g_ChannelMask:      DWORD
var g_X3DInstance:      X3DAUDIO_HANDLE

#[Swag.Foreign("xaudio2_9")]
{
    func XAudio2Create(ppXAudio2: **IXAudio2, flags: UINT32, processor: XAUDIO2_PROCESSOR = XAUDIO2_ANY_PROCESSOR)->HRESULT;
    func X3DAudioInitialize(speakerChannelMask: UINT32, speedOfSound: FLOAT32, instance: X3DAUDIO_HANDLE)->HRESULT;
}

// Create the xaudio2 engine
func create(initCOM: bool = true) throw
{
    //Debug.assert(!g_XAudio2)

    var result: HRESULT
    defer(err) destroy()

    if initCOM
    {
        result = CoInitializeEx(null, 0)
        if result != S_OK
            throw "failed to initialize COM"
        g_InitCOM = initCOM
    }

    result = XAudio2Create(&g_XAudio2, 0)
    if result != S_OK
        throw "cannot create xaudio2 device"

    result = g_XAudio2.vtbl.createMasteringVoice(g_XAudio2, &g_MasteringVoice)
    if result != S_OK
        throw "cannot create xaudio2 mastering voice"

    result = g_MasteringVoice.vtbl.getChannelMask(g_MasteringVoice, &g_ChannelMask)
    if result != S_OK
        throw "cannot get xaudio2 channel mask"

    result = X3DAudioInitialize(g_ChannelMask, X3DAUDIO_SPEED_OF_SOUND, g_X3DInstance)
    if result != S_OK
        throw "cannot initialize X3D audio"
}

// Destroys the xaudio2 engine
func destroy()
{
    if g_MasteringVoice
    {
        g_MasteringVoice.vtbl.destroyVoice(acast g_MasteringVoice)
        g_MasteringVoice = null
    }

    if g_XAudio2
    {
        discard g_XAudio2.vtbl.release(g_XAudio2)
        g_XAudio2 = null
    }

    if g_InitCOM
    {
        CoUninitialize()
        g_InitCOM = false
    }
}
