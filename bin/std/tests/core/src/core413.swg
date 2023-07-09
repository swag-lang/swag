using Core

const CHARSET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789)(*&^%$#@!"
const LEN = 25'u64

#test
{
    var rng = Random.shared()

    var password: String

    password.reserve(LEN)
    loop LEN
    {
        var idx = rng.nextS32(0, @countof(CHARSET))
        password += CHARSET[idx]
    }

    //Console.print(password)
}