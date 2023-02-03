using Core

const CHARSET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789)(*&^%$#@!"
const LEN = 25'uint

#test
{
    rng := Random.shared()

    var password: String

    password.reserve(LEN)
    loop LEN
    {
        idx := rng.nextS32(0, @countof(CHARSET))
        password += CHARSET[idx]
    }

    //Console.print(password)
}