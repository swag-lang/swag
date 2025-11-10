enum SoundFileEncoding { Pcm8, B }

#test
{
    func(TS, EN: SoundFileEncoding) doIt() {}

    doIt'(s8, SoundFileEncoding.Pcm8)()
}
