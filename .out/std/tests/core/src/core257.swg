using core

struct Test1
{
    coucous32: s32
    coucouu64: u64
    coucoub:   bool
    coucoue:   EnumFlags = ?
}

enum Enum           { Red; Green; Blue; }
enum Enum2: string  { StrRed = "StrRed"; StrGreen = "StrGreen"; StrBlue = "StrBlue"; }

#[swag.enumflags]
enum EnumFlags      { A; B; C; }

struct Test2
{
    coucou1: f32
    coucou2: f64
    coucou3: String
    coucou4: Enum = ?
    coucou5: rune
    coucou6: Array'rune
    coucou7: Array'String
    coucou8: [4] f32
    coucou9: Enum2 = ?
}

#test
{
    var tw: file.TweakFile
    var t1: Test1
    var t2: Test2
    tw.registerFolder(&t1)
    tw.registerFolder(&t2)

    const BUF = @"
        #comment
        /Test1
            coucous32  -666
            coucouu64  0xFF
            coucoub    True
            coucoub    1
            coucoub    true
            coucoue     A|B

        /Test2
            coucou1     3.14#toto
            coucou2     1e-10 # coucou
            coucou3     "toto la fripouille是"
            coucou4     Blue
            coucou5     是
            coucou6     是 是 a b c d
            coucou7     "this" "is" "an" "array" "of" "strings"
            coucou8     1 2 3 4
            coucou9     StrGreen
    "@

    arr := tokenize.splitLines(BUF)
    //assume tw.parse(arr)
    console.println(t1)
    console.println(t2)
}