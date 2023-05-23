/**
The Swag compiler can generate documentation for all the modules of a given workspace.
```
swag doc -w:myWorkspaceFolder
```
*/

/**
The documentation comment needs to be placed just before a function, struct or enum.
*/
#test
{
    // Everything between empty lines is considered to be a simple paragraph.
    // Which means that if you put several comments on several lines like
    // this, they all will be part of the same paragraph.
    //
    // This is another paragraph because there's an empty line before.
    //
    // ---
    // A paragraph that starts with '---' is a paragraph where every blanks and end of lines
    // are respected. Even...
    //
    // ...empty lines are preserved.
    // You end that kind of paragraph with another '---' alone on its line.
    // ---
    func test()
    {

    }

    // Some other markdown markers are also supported:
    //
    // `this is code` for 'inline' code.
    // 'single_word'  for 'inline' code.
    // **bold**
    struct RGB
    {
        r, g, b: s32
    }
}