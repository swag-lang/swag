using Audio

#test
{
    assume Audio.createEngine()
    defer Audio.destroyEngine()

    assume
    {
        var x = SoundFile.load(sndPath("pacman_chomp.wav"), loadDatas = false)
        let voice = Voice.create(&x)
        voice.play()
    }

    while voice.isPlaying() {}
    voice.destroy()
}