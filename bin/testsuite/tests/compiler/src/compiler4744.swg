enum SoundFileEncoding { Pcm8, B }

#test
{
    func(TS, EN: SoundFileEncoding) do()
    {
    }

    do'(s8, SoundFileEncoding.Pcm8)()
}