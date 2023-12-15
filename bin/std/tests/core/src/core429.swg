using Core

const File = """
    cfg_a : 10
    cfg_b : 20.1
"""

#ast
{
    let lines = Tokenize.split(File, `\n`'u8)
    var sb: StrConv.StringBuilder

    visit l: lines
    {
        let parts = Tokenize.split(l, `:`'u8)
        if parts.count != 2 continue
        let name  = parts[0]
        let value = parts[1]
        sb.appendFormat("const % = %\n", name, value)
    }

    return sb.toString()
}

#if cfg_a != 10
    #error "toto"
#else
    #assert cfg_a == 10
