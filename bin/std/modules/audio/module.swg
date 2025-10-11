/*
'Std.Audio' is a module to decode and play sound files.
Under windows, it is based on the 'xaudio2' library.
# How to play a sound
First, you have to initialize the audio engine by calling [[Audio.createEngine]].
```swag
Audio.createEngine()
defer Audio.destroyEngine() // Don't forget to destroy the engine when you are done
```
You then have to load a sound file.
```swag
let soundFile = Audio.SoundFile.Load("mySound.wav")
```
Note that by default, the sound file will load all of its datas in memory. If you want the sound to be loaded only when played, set 'loadDatas' to false.
```swag
let soundFile = Audio.SoundFile.Load("mySound.wav", loadDatas = false)
```
Once you have a sound file, the simplest way to play it is by calling [[Voice.play]]. The sound will be played once, until the end, and will be destroyed.
```swag
Voice.play(soundFile)
```
To have more control, you could also use [[Voice.create]] then [[Voice.Play]] on the created sound. That way you will recieve a [[Voice]] object you can play with.
```swag
let voice = Voice.create(&soundFile)
voice.setVolume(0.5)
voice.play(Loop)
```
*/

#dependencies
{
    #import("core")
    #if #os == Swag.TargetOs.Windows do
        #import("xaudio2")

    #run
    {
        let itf = @compiler
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion   = #swagversion
        cfg.moduleRevision  = #swagrevision
        cfg.moduleBuildNum  = #swagbuildnum
        cfg.moduleNamespace = "Audio"
        cfg.backendKind     = .Library
        cfg.repoPath        = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio"

        cfg.genDoc.kind            = .Api
        cfg.genDoc.outputExtension = ".php"
        cfg.genDoc.css             = "css/style.css"
        cfg.genDoc.startHead       = "<?php include('common/start-head.php'); ?>"
        cfg.genDoc.endHead         = "<?php include('common/end-head.php'); ?>"
        cfg.genDoc.startBody       = "<?php include('common/start-body.php'); ?>"
    }
}
