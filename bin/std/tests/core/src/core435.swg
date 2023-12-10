using Core

struct Test
{
    exp, val: string
    result:   bool
}

const TestCases: [] Test = [
    {"abc", "abc", true},
    {"123", "123", true},
    {"abc123", "abc123", true},    

    {"[0-9]", "5", true},
    {"[a-z]", "a", true},
    {"[A-Z]", "X", true},
    {"[0-9a-zA-Z]*", "a9Z", true},   

    {"a*", "aaa", true},
    {"a+", "aa", true},
    {"a?", "", true},
    {"a{2,4}", "aaa", true},     
    {"a{2,4}", "aaaaa", false},   

    //{"(abc)+", "abcabc", true},
    //{"(abc|123)", "abc", true}, 

    //{$"[a-z]+\d{2,4}"$, "word123", true},
    //{$"\bword\b"$, "word", true},         
]

#test
{
    visit t: TestCases
    {
        var parser: Parser.RegExp
        assume parser.compile(t.exp)
        let res = parser.match(t.val)
        @assert(res == t.result)
    }
}