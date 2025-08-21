#global export
#global #if #os == Swag.TargetOs.Windows
#foreignlib("xaudio2")
using Swag, Win32

alias UINT32  = u32
alias UINT64  = u64
alias FLOAT32 = f32

const X3DAUDIO_HANDLE_BYTESIZE = 20
alias X3DAUDIO_HANDLE = [X3DAUDIO_HANDLE_BYTESIZE] BYTE

alias XAUDIO2_PROCESSOR = UINT32
const Processor1            = 0x00000001
const Processor2            = 0x00000002
const Processor3            = 0x00000004
const Processor4            = 0x00000008
const Processor5            = 0x00000010
const Processor6            = 0x00000020
const Processor7            = 0x00000040
const Processor8            = 0x00000080
const Processor9            = 0x00000100
const Processor10           = 0x00000200
const Processor11           = 0x00000400
const Processor12           = 0x00000800
const Processor13           = 0x00001000
const Processor14           = 0x00002000
const Processor15           = 0x00004000
const Processor16           = 0x00008000
const Processor17           = 0x00010000
const Processor18           = 0x00020000
const Processor19           = 0x00040000
const Processor20           = 0x00080000
const Processor21           = 0x00100000
const Processor22           = 0x00200000
const Processor23           = 0x00400000
const Processor24           = 0x00800000
const Processor25           = 0x01000000
const Processor26           = 0x02000000
const Processor27           = 0x04000000
const Processor28           = 0x08000000
const Processor29           = 0x10000000
const Processor30           = 0x20000000
const Processor31           = 0x40000000
const Processor32           = 0x80000000
const XAUDIO2_ANY_PROCESSOR = 0xffffffff

const X3DAUDIO_SPEED_OF_SOUND = 343.5

const XAUDIO2_DEBUG_ENGINE            = 0x0001      // Used in XAudio2Create
const XAUDIO2_VOICE_NOPITCH           = 0x0002      // Used in IXAudio2::CreateSourceVoice
const XAUDIO2_VOICE_NOSRC             = 0x0004      // Used in IXAudio2::CreateSourceVoice
const XAUDIO2_VOICE_USEFILTER         = 0x0008      // Used in IXAudio2::CreateSource/SubmixVoice
const XAUDIO2_PLAY_TAILS              = 0x0020      // Used in IXAudio2SourceVoice::Stop
const XAUDIO2_END_OF_STREAM           = 0x0040      // Used in XAUDIO2_BUFFER.Flags
const XAUDIO2_SEND_USEFILTER          = 0x0080      // Used in XAUDIO2_SEND_DESCRIPTOR.Flags
const XAUDIO2_VOICE_NOSAMPLESPLAYED   = 0x0100      // Used in IXAudio2SourceVoice::GetState
const XAUDIO2_STOP_ENGINE_WHEN_IDLE   = 0x2000      // Used in XAudio2Create to force the engine to Stop when no source voices are Started, and Start when a voice is Started
const XAUDIO2_1024_QUANTUM            = 0x8000      // Used in XAudio2Create to specify nondefault processing quantum of 21.33 ms (1024 samples at 48KHz)
const XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT = 0x10000     // Used in CreateMasteringVoice to create a virtual audio client

const XAUDIO2_MAX_BUFFER_BYTES     = 0x80000000       // Maximum bytes allowed in a source buffer
const XAUDIO2_MAX_QUEUED_BUFFERS   = 64               // Maximum buffers allowed in a voice queue
const XAUDIO2_MAX_BUFFERS_SYSTEM   = 2                // Maximum buffers allowed for system threads (Xbox 360 only)
const XAUDIO2_MAX_AUDIO_CHANNELS   = 64               // Maximum channels in an audio stream
const XAUDIO2_MIN_SAMPLE_RATE      = 1000             // Minimum audio sample rate supported
const XAUDIO2_MAX_SAMPLE_RATE      = 200000           // Maximum audio sample rate supported
const XAUDIO2_MAX_VOLUME_LEVEL     = 16777216.0       // Maximum acceptable volume level (2^24)
const XAUDIO2_MIN_FREQ_RATIO       = (1 / 1024.0)     // Minimum SetFrequencyRatio argument
const XAUDIO2_MAX_FREQ_RATIO       = 1024.0           // Maximum MaxFrequencyRatio argument
const XAUDIO2_DEFAULT_FREQ_RATIO   = 2.0              // Default MaxFrequencyRatio argument
const XAUDIO2_MAX_FILTER_ONEOVERQ  = 1.5              // Maximum XAUDIO2_FILTER_PARAMETERS.OneOverQ
const XAUDIO2_MAX_FILTER_FREQUENCY = 1.0              // Maximum XAUDIO2_FILTER_PARAMETERS.Frequency
const XAUDIO2_MAX_LOOP_COUNT       = 254              // Maximum non-infinite XAUDIO2_BUFFER.LoopCount
const XAUDIO2_MAX_INSTANCES        = 8                // Maximum simultaneous XAudio2 objects on Xbox 360

// Numeric values with special meanings
const XAUDIO2_COMMIT_NOW         = 0              // Used as an OperationSet argument
const XAUDIO2_COMMIT_ALL         = 0              // Used in IXAudio2::CommitChanges
const XAUDIO2_INVALID_OPSET      = 0xFFFFFFFF     // Not allowed for OperationSet arguments
const XAUDIO2_NO_LOOP_REGION     = 0              // Used in XAUDIO2_BUFFER.LoopCount
const XAUDIO2_LOOP_INFINITE      = 255            // Used in XAUDIO2_BUFFER.LoopCount
const XAUDIO2_DEFAULT_CHANNELS   = 0              // Used in CreateMasteringVoice
const XAUDIO2_DEFAULT_SAMPLERATE = 0              // Used in CreateMasteringVoice

// Values for the TraceMask and BreakMask bitmaps.  Only ERRORS and WARNINGS
// are valid in BreakMask.  WARNINGS implies ERRORS, DETAIL implies INFO, and
// FUNC_CALLS implies API_CALLS.  By default, TraceMask is ERRORS and WARNINGS
// and all the other settings are zero.
const XAUDIO2_LOG_ERRORS     = 0x0001     // For handled errors with serious effects.
const XAUDIO2_LOG_WARNINGS   = 0x0002     // For handled errors that may be recoverable.
const XAUDIO2_LOG_INFO       = 0x0004     // Informational chit-chat (e.g. state changes).
const XAUDIO2_LOG_DETAIL     = 0x0008     // More detailed chit-chat.
const XAUDIO2_LOG_API_CALLS  = 0x0010     // Public API function entries and exits.
const XAUDIO2_LOG_FUNC_CALLS = 0x0020     // Internal function entries and exits.
const XAUDIO2_LOG_TIMING     = 0x0040     // Delays detected and other timing data.
const XAUDIO2_LOG_LOCKS      = 0x0080     // Usage of critical sections and mutexes.
const XAUDIO2_LOG_MEMORY     = 0x0100     // Memory heap usage information.
const XAUDIO2_LOG_STREAMING  = 0x1000     // Audio streaming information.

struct XAUDIO2_EFFECT_CHAIN {}

struct XAUDIO2_PERFORMANCE_DATA
{
    AudioCyclesSinceLastQuery:      UINT64
    TotalCyclesSinceLastQuery:      UINT64
    MinimumCyclesPerQuantum:        UINT32
    MaximumCyclesPerQuantum:        UINT32
    MemoryUsageInBytes:             UINT32
    CurrentLatencyInSamples:        UINT32
    GlitchesSinceEngineStarted:     UINT32
    ActiveSourceVoiceCount:         UINT32
    TotalSourceVoiceCount:          UINT32
    ActiveSubmixVoiceCount:         UINT32
    ActiveResamplerCount:           UINT32
    ActiveMatrixMixCount:           UINT32
    ActiveXmaSourceVoices:          UINT32
    ActiveXmaStreams:               UINT32
}

struct XAUDIO2_DEBUG_CONFIGURATION
{
    traceMask:           UINT32
    breakMask:           UINT32
    logThreadID:         BOOL
    logFileline:         BOOL
    logFunctionName:     BOOL
    logTiming:           BOOL
}

enum AUDIO_STREAM_CATEGORY
{
    AudioCategory_Other                  = 0
    AudioCategory_ForegroundOnlyMedia    = 1
    AudioCategory_BackgroundCapableMedia = 2
    AudioCategory_Communications         = 3
    AudioCategory_Alerts                 = 4
    AudioCategory_SoundEffects           = 5
    AudioCategory_GameEffects            = 6
    AudioCategory_GameMedia              = 7
    AudioCategory_GameChat               = 8
    AudioCategory_Speech                 = 9
    AudioCategory_Movie                  = 10
    AudioCategory_Media                  = 11
}

struct XAUDIO2_VOICE_DETAILS
{
    creationFlags:       UINT32
    activeFlags:         UINT32
    inputChannels:       UINT32
    inputSampleRate:     UINT32
}

#[Swag.Pack(1)]
struct XAUDIO2_SEND_DESCRIPTOR
{
    flags:            UINT32
    pOutputVoice:     *IXAudio2Voice
}

#[Swag.Pack(1)]
struct XAUDIO2_VOICE_SENDS
{
    sendCount:     UINT32
    pSends:        *XAUDIO2_SEND_DESCRIPTOR
}

#[Swag.Pack(1)]
struct XAUDIO2_BUFFER
{
    flags:          UINT32          // Either 0 or XAUDIO2_END_OF_STREAM.
    audioBytes:     UINT32          // Size of the audio data buffer in bytes.
    pAudioData:     const *BYTE     // Pointer to the audio data buffer.
    playBegin:      UINT32          // First sample in this buffer to be played.
    playLength:     UINT32          // Length of the region to be played in samples, or 0 to play the whole buffer.
    loopBegin:      UINT32          // First sample of the region to be looped.
    loopLength:     UINT32          // Length of the desired for region in samples, or 0 to for the entire buffer.
    loopCount:      UINT32          // Number of times to repeat the for region, or XAUDIO2_LOOP_INFINITE to for forever.
    pContext:       *void           // Context value to be passed back in callbacks.
}

struct XAUDIO2_BUFFER_WMA
{
    pDecodedPacketCumulativeBytes:     const *UINT32
    PacketCount:                       UINT32
}

struct XAUDIO2_VOICE_STATE
{
    pCurrentBufferContext:     *void
    buffersQueued:             UINT32
    samplesPlayed:             UINT64
}

// Used in XAUDIO2_FILTER_PARAMETERS below
enum XAUDIO2_FILTER_TYPE
{
    LowPassFilter             // Attenuates frequencies above the cutoff frequency (state-variable filter).
    BandPassFilter            // Attenuates frequencies outside a given range      (state-variable filter).
    HighPassFilter            // Attenuates frequencies below the cutoff frequency (state-variable filter).
    NotchFilter               // Attenuates frequencies inside a given range       (state-variable filter).
    LowPassOnePoleFilter      // Attenuates frequencies above the cutoff frequency (one-pole filter, XAUDIO2_FILTER_PARAMETERS.OneOverQ has no effect)
    HighPassOnePoleFilter     // Attenuates frequencies below the cutoff frequency (one-pole filter, XAUDIO2_FILTER_PARAMETERS.OneOverQ has no effect)
}

struct XAUDIO2_FILTER_PARAMETERS
{
    type:          XAUDIO2_FILTER_TYPE
    frequency:     f32
    oneOverQ:      f32
}

struct IXAudio2
{
    vtbl: *IXAudio2Itf
}
struct IXAudio2Voice
{
    vtbl: *IXAudio2VoiceItf
}
struct IXAudio2MasteringVoice
{
    vtbl: *IXAudio2MasteringVoiceItf
}
struct IXAudio2SourceVoice
{
    vtbl: *IXAudio2SourceVoiceItf
}
struct IXAudio2EngineCallback
{
    vtbl: *IXAudio2EngineCallbackItf
}
struct IXAudio2VoiceCallback
{
    vtbl: *IXAudio2VoiceCallbackItf
}
struct IXAudio2SubmixVoice
{
    vtbl: *IXAudio2SubmixVoiceItf
}

struct IXAudio2EngineCallbackItf
{
    onProcessingPassStart:     func(self: *IXAudio2EngineCallback)
    onProcessingPassEnd:       func(self: *IXAudio2EngineCallback)
    onCriticalError:           func(self: *IXAudio2EngineCallback, error: HRESULT)
}

struct IXAudio2VoiceCallbackItf
{
    onVoiceProcessingPassStart:     func(self: *IXAudio2VoiceCallback, bytesRequired: UINT32)
    onVoiceProcessingPassEnd:       func(self: *IXAudio2VoiceCallback)
    onStreamEnd:                    func(self: *IXAudio2VoiceCallback)
    onBufferStart:                  func(self: *IXAudio2VoiceCallback, pBufferContext: *void)
    onBufferEnd:                    func(self: *IXAudio2VoiceCallback, pBufferContext: *void)
    onLoopEnd:                      func(self: *IXAudio2VoiceCallback, pBufferContext: *void)
    onVoiceError:                   func(self: *IXAudio2VoiceCallback, pBufferContext: *void, error: HRESULT)
}

struct IXAudio2Itf
{
    queryInterface:             *void
    addRef:                     func(self: *IXAudio2)->ULONG
    release:                    func(self: *IXAudio2)->ULONG
    registerForCallbacks:       func(self: *IXAudio2, pCallback: *IXAudio2EngineCallback)
    unregisterForCallbacks:     func(self: *IXAudio2, pCallback: *IXAudio2EngineCallback)
    createSourceVoice:          func(self: *IXAudio2, ppSourceVoice: **IXAudio2SourceVoice, pSourceFormat: const *WAVEFORMATEX, flags: UINT32, maxFrequencyRatio: f32, pCallback: *IXAudio2VoiceCallback, pSendList: #null const *XAUDIO2_VOICE_SENDS, pEffectChain: #null const *XAUDIO2_EFFECT_CHAIN)->HRESULT
    createSubmixVoice:          func(self: *IXAudio2, ppSubmixVoice: **IXAudio2SubmixVoice, inputChannels: UINT32, inputSampleRate: UINT32, flags: UINT32, processingStage: UINT32, pSendList: #null const *XAUDIO2_VOICE_SENDS, pEffectChain: #null const *XAUDIO2_EFFECT_CHAIN)->HRESULT
    createMasteringVoice:       func(self: *IXAudio2, voice: **IXAudio2MasteringVoice, inputChannels: UINT32 = XAUDIO2_DEFAULT_CHANNELS, inputSampleRate: UINT32 = XAUDIO2_DEFAULT_SAMPLERATE, flags: UINT32 = 0, szDeviceId: LPCWSTR = null, effectChain: *XAUDIO2_EFFECT_CHAIN = null, streamCategory = AUDIO_STREAM_CATEGORY.AudioCategory_GameEffects)->HRESULT
    startEngine:                func(self: *IXAudio2)->HRESULT
    stopEngine:                 func(self: *IXAudio2)
    commitChanges:              func(self: *IXAudio2, operationSet: UINT32)->HRESULT
    getPerformanceData:         func(self: *IXAudio2, pPerfData: *XAUDIO2_PERFORMANCE_DATA)
    setDebugConfiguration:      func(self: *IXAudio2, pDebugConfiguration: const *XAUDIO2_DEBUG_CONFIGURATION, pReserved: #null *void)
}

struct IXAudio2VoiceItf
{
    getVoiceDetails:               func(self: *IXAudio2Voice, pVoiceDetails: *XAUDIO2_VOICE_DETAILS)
    setOutputVoices:               func(self: *IXAudio2Voice, pSendList: #null const *XAUDIO2_VOICE_SENDS)->HRESULT
    setEffectChain:                func(self: *IXAudio2Voice, pEffectChain: const *XAUDIO2_EFFECT_CHAIN)
    enableEffect:                  func(self: *IXAudio2Voice, effectIndex: UINT32, operationSet: UINT32)->HRESULT
    disableEffect:                 func(self: *IXAudio2Voice, effectIndex: UINT32, operationSet: UINT32)->HRESULT
    getEffectState:                func(self: *IXAudio2Voice, effectIndex: UINT32, pEnabled: *BOOL)
    setEffectParameters:           func(self: *IXAudio2Voice, effectIndex: UINT32, pParameters: const *void, parametersByteSize: UINT32, operationSet: UINT32)->HRESULT
    getEffectParameters:           func(self: *IXAudio2Voice, effectIndex: UINT32, pParameters: *void, parametersByteSize: UINT32)->HRESULT
    setFilterParameters:           func(self: *IXAudio2Voice, pParameters: const *XAUDIO2_FILTER_PARAMETERS, operationSet: UINT32)->HRESULT
    getFilterParameters:           func(self: *IXAudio2Voice, pParameters: *XAUDIO2_FILTER_PARAMETERS)
    setOutputFilterParameters:     func(self: *IXAudio2Voice, pDestinationVoice: *IXAudio2Voice, pParameters: const *XAUDIO2_FILTER_PARAMETERS, operationSet: UINT32)->HRESULT
    getOutputFilterParameters:     func(self: *IXAudio2Voice, pDestinationVoice: *IXAudio2Voice, pParameters: *XAUDIO2_FILTER_PARAMETERS)
    setVolume:                     func(self: *IXAudio2Voice, volume: f32, operationSet: UINT32)->HRESULT
    getVolume:                     func(self: *IXAudio2Voice, pVolume: *f32)
    setChannelVolumes:             func(self: *IXAudio2Voice, channels: UINT32, pVolumes: const *f32, operationSet: UINT32)->HRESULT
    getChannelVolumes:             func(self: *IXAudio2Voice, channels: UINT32, pVolumes: *f32)
    setOutputMatrix:               func(self: *IXAudio2Voice, pDestinationVoice: *IXAudio2Voice, sourceChannels: UINT32, destinationChannels: UINT32, pLevelMatrix: const *f32, operationSet: UINT32)->HRESULT
    getOutputMatrix:               func(self: *IXAudio2Voice, pDestinationVoice: *IXAudio2Voice, sourceChannels: UINT32, destinationChannels: UINT32, pLevelMatrix: *f32)
    destroyVoice:                  func(self: *IXAudio2Voice)
}

struct IXAudio2MasteringVoiceItf
{
    using voice:        IXAudio2VoiceItf
    getChannelMask:     func(self: *IXAudio2MasteringVoice, pChannelmask: *DWORD)->HRESULT
}

struct IXAudio2SubmixVoiceItf
{
    using voice: IXAudio2VoiceItf
}

struct IXAudio2SourceVoiceItf
{
    using voice:             IXAudio2VoiceItf
    start:                   func(self: *IXAudio2SourceVoice, flags: UINT32, operationSet: UINT32)->HRESULT
    stop:                    func(self: *IXAudio2SourceVoice, flags: UINT32, operationSet: UINT32)->HRESULT
    submitSourceBuffer:      func(self: *IXAudio2SourceVoice, pBuffer: const *XAUDIO2_BUFFER, pBufferWMA: #null const *XAUDIO2_BUFFER_WMA)->HRESULT
    glushSourceBuffers:      func(self: *IXAudio2SourceVoice)->HRESULT
    discontinuity:           func(self: *IXAudio2SourceVoice)->HRESULT
    exitLoop:                func(self: *IXAudio2SourceVoice, OperationSet: UINT32)->HRESULT
    getState:                func(self: *IXAudio2SourceVoice, pVoiceState: *XAUDIO2_VOICE_STATE, flags: UINT32)
    setFrequencyRatio:       func(self: *IXAudio2SourceVoice, ratio: f32, operationSet: UINT32)->HRESULT
    getFrequencyRatio:       func(self: *IXAudio2SourceVoice, pRatio: *f32)
    setSourceSampleRate:     func(self: *IXAudio2SourceVoice, newSourceSampleRate: UINT32)->HRESULT
}

const FORMAT_EXTENSIBLE = 0xFFFE'u16

#[Swag.Pack(1)]
struct WAVEFORMATEX
{
    wFormatTag:          WORD
    nChannels:           WORD
    nSamplesPerSec:      DWORD
    nAvgBytesPerSec:     DWORD
    nBlockAlign:         WORD
    wBitsPerSample:      WORD
    cbSize:              WORD
}

struct WAVEFORMATEXTENSIBLE
{
    format: WAVEFORMATEX

    samples: union
    {
        wValidBitsPerSample:     WORD
        wSamplesPerBlock:        WORD
        wReserved:               WORD
    }

    dwChannelMask:     DWORD
    subFormat:         GUID
}

#[Swag.Foreign("xaudio2_9")]
{
    func XAudio2Create(ppXAudio2: **IXAudio2, flags: UINT32, processor: XAUDIO2_PROCESSOR = XAUDIO2_ANY_PROCESSOR)->HRESULT;
    func X3DAudioInitialize(speakerChannelMask: UINT32, speedOfSound: FLOAT32, instance: X3DAUDIO_HANDLE)->HRESULT;
}
