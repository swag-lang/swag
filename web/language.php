<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><style>

        .container {
            display:        flex;
            flex-wrap:      nowrap;
            flex-direction: row;
            margin:         0px auto;
            padding:        0px;
        }
        .left {
            display:    block;
            overflow-y: scroll;
            width:      500px;
        }
        .leftpage {
            margin:     10px;
        }
        .right {
            display:    block;
            width:      100%;
        }
        .rightpage {
            max-width:  1024px;
            margin:     10px auto;
        }
        @media(min-width: 640px) {
            .container {
                max-width: 640px;
            }
        }
        @media(min-width: 768px) {
            .container {
                max-width: 768px;
            }
        }
        @media(min-width: 1024px) {
            .container {
                max-width: 1024px;
            }
        }
        @media(min-width: 1280px) {
            .container {
                max-width: 1280px;
            }
        }
        @media(min-width: 1536px) {
            .container {
                max-width: 1536px;
            }
        }
        @media screen and (max-width: 600px) {
            .left {
                display: none;
            }
            .rightpage {
                margin:  10px;
            }
        }
            .container {
                height:     100vh;
            }
            .right {
                overflow-y: scroll;
            }
        body {
            margin:         0px;
            line-height:    1.3em;
            font-family:    Segoe UI;
        }
        .container blockquote {
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            margin-right:       50px;
            padding:            10px;
        }
        .container a {
            text-decoration: none;
            color:           DoggerBlue;
        }
        .container a:hover {
            text-decoration: underline;
        }
        .container a.src {
            font-size:          90%;
            color:              LightGrey;
        }
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        .item td:first-child {
            width:              33%;
            white-space:        nowrap;
        }
        .enumeration td {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            min-width:          100px;
        }
        .enumeration td:first-child {
            background-color:   #f8f8f8;
            white-space:        nowrap;
        }
        .container td:last-child {
            width:              100%;
        }
        .left ul {
            list-style-type:    none;
            margin-left:        -20px;
        }
        .titletype {
            font-weight:        normal;
            font-size:          80%;
        }
        .titlelight {
            font-weight:        normal;
        }
        .titlestrong {
            font-weight:        bold;
            font-size:          100%;
        }
        .left h2 {
            background-color:   Black;
            color:              White;
            padding:            6px;
        }
        .right h1 {
            margin-top:         50px;
            margin-bottom:      50px;
        }
        .right h2 {
            margin-top:         35px;
        }
        table.item {
            background-color:   Black;
            color:              White;
            width:              100%;
            margin-top:         70px;
            margin-right:       0px;
            padding:            4px;
            font-size:          110%;
        }
        .srcref {
            text-align:         right;
        }
        .incode {
            background-color:   #eeeeee;
            padding:            2px;
            border: 1px dotted  #cccccc;
        }
        .tdname .incode {
            background-color:   revert;
            padding:            2px;
            border:             revert;
        }
        .addinfos {
            font-size:          90%;
            white-space:        break-spaces;
            overflow-wrap:      break-word;
        }
        .precode {
            background-color:   #eeeeee;
            border:             1px solid LightGrey;
            padding:            10px;
            margin:             20px;
            white-space:        pre;
            overflow-x:         auto;
        }    .SyntaxCode      { color: #7f7f7f; }
    .SyntaxComment   { color: #71a35b; }
    .SyntaxCompiler  { color: #7f7f7f; }
    .SyntaxFunction  { color: #ff6a00; }
    .SyntaxConstant  { color: #3173cd; }
    .SyntaxIntrinsic { color: #b4b44a; }
    .SyntaxType      { color: #3bc3a7; }
    .SyntaxKeyword   { color: #3186cd; }
    .SyntaxLogic     { color: #b040be; }
    .SyntaxNumber    { color: #74a35b; }
    .SyntaxString    { color: #bb6643; }
    .SyntaxAttribute { color: #7f7f7f; }
</style>
<?php include('common/end-head.php'); ?></head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="left">
<div class="leftpage">
<h1>Content</h1>
<ul>
<li><a href="#000_introduction">Introduction</a></li>
<li><a href="#001_hello_mad_world">Hello mad world</a></li>
<li><a href="#002_source_code_organization">Source code organization</a></li>
<li><a href="#003_comments">Comments</a></li>
<li><a href="#004_identifiers">Identifiers</a></li>
<li><a href="#005_keywords">Keywords</a></li>
<li><a href="#006_semicolon">Semicolon</a></li>
<li><a href="#007_global_declaration_order">Global declaration order</a></li>
<li><a href="#010_basic_types">Basic types</a></li>
<li><a href="#011_number_literals">Number literals</a></li>
<li><a href="#012_string">String</a></li>
<li><a href="#013_variables">Variables</a></li>
<li><a href="#014_const">Const</a></li>
<li><a href="#015_operators">Operators</a></li>
<li><a href="#016_cast">Cast</a></li>
<li><a href="#020_array">Array</a></li>
<li><a href="#021_slice">Slice</a></li>
<li><a href="#022_pointers">Pointers</a></li>
<li><a href="#023_references">References</a></li>
<li><a href="#024_any">Any</a></li>
<li><a href="#025_tuple">Tuple</a></li>
<li><a href="#030_enum">Enum</a></li>
<li><a href="#031_impl">Impl</a></li>
<li><a href="#035_namespace">Namespace</a></li>
<li><a href="#050_if">If</a></li>
<li><a href="#051_loop">Loop</a></li>
<li><a href="#052_visit">Visit</a></li>
<li><a href="#053_for">For</a></li>
<li><a href="#054_while">While</a></li>
<li><a href="#055_switch">Switch</a></li>
<li><a href="#056_break">Break</a></li>
<li><a href="#060_struct">Struct</a></li>
<ul>
<li><a href="#061_001_declaration">Declaration</a></li>
<li><a href="#062_002_impl">Impl</a></li>
<li><a href="#063_003_special_functions">Special functions</a></li>
<li><a href="#064_004_affectation">Affectation</a></li>
<li><a href="#064_005_count">Count</a></li>
<li><a href="#064_006_post_copy_and_post_move">Post copy and post move</a></li>
<li><a href="#064_007_visit">Visit</a></li>
<li><a href="#067_008_offset">Offset</a></li>
<li><a href="#068_009_packing">Packing</a></li>
</ul>
<li><a href="#070_union">Union</a></li>
<li><a href="#075_interface">Interface</a></li>
<li><a href="#100_function">Function</a></li>
<ul>
<li><a href="#101_001_declaration">Declaration</a></li>
<li><a href="#102_002_lambda">Lambda</a></li>
<li><a href="#103_003_closure">Closure</a></li>
<li><a href="#104_004_mixin">Mixin</a></li>
<li><a href="#105_005_macro">Macro</a></li>
<li><a href="#105_006_variadic_parameters">Variadic parameters</a></li>
<li><a href="#106_007_ufcs">Ufcs</a></li>
<li><a href="#107_008_constexpr">Constexpr</a></li>
<li><a href="#108_009_function_overloading">Function overloading</a></li>
<li><a href="#109_010_discard">Discard</a></li>
<li><a href="#110_011_retval">Retval</a></li>
<li><a href="#111_012_foreign">Foreign</a></li>
</ul>
<li><a href="#120_intrinsics">Intrinsics</a></li>
<li><a href="#121_init">Init</a></li>
<li><a href="#130_generic">Generic</a></li>
<ul>
<li><a href="#131_001_declaration">Declaration</a></li>
<li><a href="#132_002_validif">Validif</a></li>
<li><a href="#133_003_constraint">Constraint</a></li>
</ul>
<li><a href="#160_scoping">Scoping</a></li>
<ul>
<li><a href="#161_001_defer">Defer</a></li>
<li><a href="#162_002_using">Using</a></li>
<li><a href="#163_003_with">With</a></li>
</ul>
<li><a href="#164_alias">Alias</a></li>
<li><a href="#170_error_management">Error management</a></li>
<li><a href="#175_safety">Safety</a></li>
<li><a href="#180_compiler_declarations">Compiler declarations</a></li>
<ul>
<li><a href="#181_001_compile_time_evaluation">Compile time evaluation</a></li>
<li><a href="#182_002_special_functions">Special functions</a></li>
<li><a href="#183_003_run">Run</a></li>
<li><a href="#184_004_global">Global</a></li>
<li><a href="#185_005_var">Var</a></li>
</ul>
<li><a href="#190_attributes">Attributes</a></li>
<ul>
<li><a href="#191_001_user_attributes">User attributes</a></li>
<li><a href="#192_002_predefined_attributes">Predefined attributes</a></li>
</ul>
<li><a href="#200_type_reflection">Type reflection</a></li>
<li><a href="#210_code_inspection">Code inspection</a></li>
<li><a href="#220_meta_programmation">Meta programmation</a></li>
<ul>
<li><a href="#221_001_ast">Ast</a></li>
<li><a href="#222_002_compiler_interface">Compiler interface</a></li>
</ul>
<li><a href="#230_documentation">Documentation</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Swag language reference</h1>

<h2 id="000_introduction">Introduction</h2><p>This <code class="incode">swag-lang/swag/bin/reference/language</code> module provides a brief explanation of the language basic syntax and usage without the need of the <a href="std.php">Swag standard modules</a> (<code class="incode">Std</code>). </p>
<p>More advanced features such as dynamic arrays, dynamic strings or hash maps can be found in the <a href="std.core.php">Std.Core</a> module and will not be covered in the examples to come. For now we will focus on the language itself. </p>
<p>As <code class="incode">reference/language</code> is written as a test module, you can run it with : </p>
<div class="precode"><code></span><span class="SyntaxCode">swag test --workspace:c:/swag-lang/swag/bin/reference
swag test -w:c:/swag-lang/swag/bin/reference</code>
</div>
<p>This will run all the test modules of the specified workspace (including this one). You can also omit the <code class="incode">--workspace</code> parameter (or <code class="incode">-w</code> in its short form) if you run Swag directly from the workspace folder. </p>
<p>Note that if you want to compile and run a single module in the workspace, you can specify it with the <code class="incode">--module</code> (<code class="incode">-m</code>) parameter. </p>
<div class="precode"><code></span><span class="SyntaxCode">swag test -w:c:/swag-lang/swag/bin/reference -m:test_language</code>
</div>

<h2 id="001_hello_mad_world">Hello mad world</h2><p>Let's start with the most simple version of the "hello world" example. This is a version that does not require external dependencies like the <a href="std.php">Swag standard modules</a>. </p>
<p><code class="incode">#main</code> is the <b>program entry point</b>, a special compiler function (that's why the name starts with <code class="incode">#</code>). It must be defined only once for a native executable. <code class="incode">@print</code> is an <b>intrinsic</b>, a special built-in function (that's why the name starts with <code class="incode">@</code>). It is part of the <a href="std/swag.runtime.html">compiler runtime</a> which comes with the compiler. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#main</span><span class="SyntaxCode">
{
    </span><span class="SyntaxIntrinsic">@print</span><span class="SyntaxCode">(</span><span class="SyntaxString">"Hello mad world !\n"</span><span class="SyntaxCode">)
}</code>
</div>
<p>Next, a version that this time uses the <code class="incode">Console.print</code> function in the <a href="std/std.core.html">Std.Core</a> module. </p>
<p>The <code class="incode">Std.Core</code> module would have to be imported in order to be used, but let's keep it simple. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#main</span><span class="SyntaxCode">
{
    </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Console</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">print</span><span class="SyntaxCode">(</span><span class="SyntaxString">"Hello mad world !"</span><span class="SyntaxCode">, </span><span class="SyntaxString">"\n"</span><span class="SyntaxCode">)
    </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Console</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">printf</span><span class="SyntaxCode">(</span><span class="SyntaxString">"%\n"</span><span class="SyntaxCode">, </span><span class="SyntaxString">"Hello mad world again !"</span><span class="SyntaxCode">)
}</code>
</div>
<p>A <code class="incode">#run</code> block is executed at <b>compile time</b>, and can make Swag behaves like a kind of a <b>scripting language</b>. </p>
<p>So in the following example, the famous message will be printed by the compiler during compilation. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#run</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Msg</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"Hello mad world !\n"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// Creates a compiler constant of type 'string'</span><span class="SyntaxCode">
    </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Console</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">print</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Msg</span><span class="SyntaxCode">)             </span><span class="SyntaxComment">// And call 'Console.print' at compile time</span><span class="SyntaxCode">
}</code>
</div>
<p>A version that calls a <b>nested function</b> at compile time (only) to initialize the string constant to print. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxComment">// Brings the 'Core' namespace into scope, to avoid repeating it again and again</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">

</span><span class="SyntaxFunction">#main</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">nestedFunc</span><span class="SyntaxCode">() =&gt; </span><span class="SyntaxString">"Hello mad world !\n"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// Function short syntax</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// nestedFunc() can be called at compile time because it is marked with</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// the 'Swag.ConstExpr' attribute.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Msg</span><span class="SyntaxCode"> = </span><span class="SyntaxFunction">nestedFunc</span><span class="SyntaxCode">()
    </span><span class="SyntaxConstant">Console</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">print</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Msg</span><span class="SyntaxCode">)
}</code>
</div>
<p>Now a stupid version that generates the code to do the print thanks to <b>meta programming</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">

</span><span class="SyntaxFunction">#main</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Msg</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"Hello mad world !\n"</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// The result of an '#ast' block is a string that will be compiled in place.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// So this whole thing is equivalent to a simple 'Console.print(Msg)'.</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">#ast</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> sb = </span><span class="SyntaxConstant">StrConv</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">StringBuilder</span><span class="SyntaxCode">{}
        sb.</span><span class="SyntaxFunction">appendString</span><span class="SyntaxCode">(</span><span class="SyntaxString">"Console.print(Msg)"</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> sb.</span><span class="SyntaxFunction">toString</span><span class="SyntaxCode">()
    }
}</code>
</div>
<p>And finally let's be more and more crazy. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">

</span><span class="SyntaxFunction">#main</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// #run will force the call of mySillyFunction() at compile time even if it's not marked</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// with #[Swag.ConstExpr]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Msg</span><span class="SyntaxCode"> = </span><span class="SyntaxFunction">#run</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">mySillyFunction</span><span class="SyntaxCode">()
    </span><span class="SyntaxConstant">Console</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">print</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Msg</span><span class="SyntaxCode">)
}

</span><span class="SyntaxComment">// The attribute #[Swag.Compiler] tells Swag that this is a compile time function only.</span><span class="SyntaxCode">
</span><span class="SyntaxComment">// So this function will not be exported to the final executable or module.</span><span class="SyntaxCode">
</span><span class="SyntaxAttribute">#[Swag.Compiler]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">mySillyFunction</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxType">string</span><span class="SyntaxCode">
{
    </span><span class="SyntaxConstant">Console</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">print</span><span class="SyntaxCode">(</span><span class="SyntaxString">"Hello mad world at compile time !\n"</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// This creates a constant named 'MyConst'</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">#ast</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> sb = </span><span class="SyntaxConstant">StrConv</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">StringBuilder</span><span class="SyntaxCode">{}
        sb.</span><span class="SyntaxFunction">appendString</span><span class="SyntaxCode">(</span><span class="SyntaxString">"const MyConst = \"Hello "</span><span class="SyntaxCode">)
        sb.</span><span class="SyntaxFunction">appendString</span><span class="SyntaxCode">(</span><span class="SyntaxString">"mad world "</span><span class="SyntaxCode">)
        sb.</span><span class="SyntaxFunction">appendString</span><span class="SyntaxCode">(</span><span class="SyntaxString">"at runtime !\""</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> sb.</span><span class="SyntaxFunction">toString</span><span class="SyntaxCode">()
    }

    </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyConst</span><span class="SyntaxCode">
}</code>
</div>
<p>This whole piece of code is equivalent to... </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#main</span><span class="SyntaxCode">
{
    </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Console</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">print</span><span class="SyntaxCode">(</span><span class="SyntaxString">"Hello mad world at runtime !"</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="002_source_code_organization">Source code organization</h2><p>All source files in Swag have the <code class="incode">.swg</code> extension, except if you write a simple script with the <code class="incode">.swgs</code> extension. </p>
<p>They must be encoded in <b>UTF8</b>. </p>
<p>In Swag you cannot compile a single file (with the exception of <code class="incode">.swgs</code> script files). The source code is organized in a <b>workspace</b> which contains one or multiple <b>modules</b>. </p>
<p>For example, <code class="incode">Std</code> is a workspace that contains all the Swag standard modules. </p>
<p>A module is a <code class="incode">dll</code> (under windows) or an executable, and a workspace can include many of them. So a workspace will contain the modules you write (like your main executable) but also all your dependencies (some external modules you use). </p>
<p>Typically, the entire workspace is compiled. </p>

<h2 id="003_comments">Comments</h2><p>Let's start with the basics. Swag support classical single-line and multi-line comments. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxComment">// Single-line comment</span><span class="SyntaxCode">

/*
    </span><span class="SyntaxConstant">Multi</span><span class="SyntaxCode">-line comment on...
    ... multiple lines
*/

</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// This is a constant with the value '0' assigned to it</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> = /* </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode"> */ </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode"></code>
</div>
<p>Nested comments are supported. </p>
<div class="precode"><code></span><span class="SyntaxCode">/*
    /* </span><span class="SyntaxConstant">You</span><span class="SyntaxCode"> can also nest multi-line comments */
*/</code>
</div>

<h2 id="004_identifiers">Identifiers</h2><p>User identifiers (like variables, constants, function names...) must start with an underscore or an ascii letter. Those identifiers can then contain underscores, ascii letters and digit numbers. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> thisIsAValidIdentifier0   = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> this_is_also_valid        = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> this_1_is_2_also__3_valid = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"></code>
</div>
<p>But your identifiers cannot start with two underscores. This is reserved by the compiler. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxComment">// const __this_is_invalid = 0</span><span class="SyntaxCode"></code>
</div>
<p>Note that some identifiers may start with <code class="incode">#</code>. This indicates a <b>compiler special keyword</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode">#assert
</span><span class="SyntaxFunction">#run</span><span class="SyntaxCode">
</span><span class="SyntaxFunction">#main</span><span class="SyntaxCode"></code>
</div>
<p>Some identifiers can also start with <code class="incode">@</code>. This indicates an <b>intrinsic</b> function which can be compile time only or also available at runtime (this depends on the intrinsic). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@sqrt</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@print</span><span class="SyntaxCode">()</code>
</div>

<h2 id="005_keywords">Keywords</h2><p>This is the list of all keywords in the language. </p>
<h3>Basic types </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxType">s8</span><span class="SyntaxCode">
</span><span class="SyntaxType">s16</span><span class="SyntaxCode">
</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxType">s64</span><span class="SyntaxCode">
</span><span class="SyntaxType">u8</span><span class="SyntaxCode">
</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxType">f64</span><span class="SyntaxCode">
</span><span class="SyntaxType">bool</span><span class="SyntaxCode">
</span><span class="SyntaxType">string</span><span class="SyntaxCode">
</span><span class="SyntaxType">rune</span><span class="SyntaxCode">
</span><span class="SyntaxType">any</span><span class="SyntaxCode">
</span><span class="SyntaxType">typeinfo</span><span class="SyntaxCode">
</span><span class="SyntaxType">void</span><span class="SyntaxCode">
</span><span class="SyntaxType">code</span><span class="SyntaxCode">
</span><span class="SyntaxType">cstring</span><span class="SyntaxCode">
</span><span class="SyntaxType">cvarargs</span><span class="SyntaxCode"></code>
</div>
<h3>Language keywords </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxLogic">if</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">else</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">elif</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">and</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">or</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">switch</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">case</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">default</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">continue</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">fallthrough</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">unreachable</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">for</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">loop</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">while</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">visit</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">return</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">defer</span><span class="SyntaxCode">

</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">undefined</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">retval</span><span class="SyntaxCode">

</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">var</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">let</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">ref</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">acast</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">assume</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">try</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">catch</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">union</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">namespace</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">public</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">internal</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">private</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">interface</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">namealias</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">discard</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">dref</span><span class="SyntaxCode"></code>
</div>
<h3>Compiler keywords </h3>
<div class="precode"><code></span><span class="SyntaxCode">#arch
#backend
#callerfunction
#callerlocation
#cfg
#code
#file
#line
#location
#module
#os
#self
#swagbuildnum
#swagos
#swagrevision
#swagversion
</span><span class="SyntaxType">#type</span><span class="SyntaxCode">
#up

#assert
#elif
#else
#error
#global
#if
#import
#include
#load
#macro
#mixin
#placeholder
#print
#validifx
#validif
#warning
#scope

</span><span class="SyntaxFunction">#ast</span><span class="SyntaxCode">
</span><span class="SyntaxFunction">#drop</span><span class="SyntaxCode">
</span><span class="SyntaxFunction">#init</span><span class="SyntaxCode">
</span><span class="SyntaxFunction">#main</span><span class="SyntaxCode">
</span><span class="SyntaxFunction">#message</span><span class="SyntaxCode">
</span><span class="SyntaxFunction">#premain</span><span class="SyntaxCode">
</span><span class="SyntaxFunction">#run</span><span class="SyntaxCode"></code>
</div>
<h3>Intrinsics libc </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@acos</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@alloc</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@asin</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@atan</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@bitcountlz</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@bitcountnz</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@bitcounttz</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@byteswap</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@ceil</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@cos</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@cosh</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@cvaarg</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@cvaend</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@cvastart</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@exp</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@exp2</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@floor</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@free</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@log</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@log10</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@log2</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@memcmp</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@memcpy</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@memmove</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@memset</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@muladd</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@pow</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@realloc</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@round</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@sin</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@sinh</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@sqrt</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@strcmp</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@strlen</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@tan</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@tanh</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@trunc</span><span class="SyntaxCode"></code>
</div>
<h3>Other intrinsics </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@err</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@errmsg</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@alias1</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@alias2</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// and more generally @aliasN</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@alignof</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@args</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@breakpoint</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@compiler</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@dataof</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@dbgalloc</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@defined</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@drop</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@compilererror</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@compilerwarning</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@getcontext</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@getpinfos</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@gettag</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@hastag</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@init</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@isbytecode</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@isconstexpr</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@itftableof</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@location</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@mixin0</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@mixin1</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@mixin2</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// and more generally @mixinN</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@mkany</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@mkcallback</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@mkinterface</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@mkslice</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@mkstring</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@decltype</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@modules</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@nameof</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@panic</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@postcopy</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@postmove</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@print</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@rtflags</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@safety</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@setcontext</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@spread</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@stringcmp</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@stringof</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@sysalloc</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@typecmp</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode"></code>
</div>
<h3>Modifiers </h3>
<div class="precode"><code></span><span class="SyntaxCode">,over
,nodrop
,bit
,move
,moveraw
,up
,unconst</code>
</div>

<h2 id="006_semicolon">Semicolon</h2><p>In Swag, there's no need to end a statement with <code class="incode">;</code> like in C/C++. Most of the time a <code class="incode">end of line</code> is enough. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Declare two variables x and y of type s32 (signed 32 bits), and initialize them to 1.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Increment x and y by 1.</span><span class="SyntaxCode">
    x += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    y += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// When running the tests, the '@assert' intrinsic will verify that those values are correct.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)     </span><span class="SyntaxComment">// Verify that x is equal to 2, and raise an error if not.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == x)     </span><span class="SyntaxComment">// Verify that y is equal to x.</span><span class="SyntaxCode">
}</code>
</div>
<p>The semicolons are not mandatory, but it's possible to use them if you want. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// You can notice that the type of x and y is not specified here. This is due to type inference (we will see that later).</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x, y = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">;

    </span><span class="SyntaxComment">// You can also notice the short syntax to do the same operation on multiple variables at once.</span><span class="SyntaxCode">
    x, y += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">;
}</code>
</div>
<p>Semicolons can be usefull if you want to do multiple things on the same line. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Two instructions on the same line separated with ';'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> y = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Two instructions on the same line separated with ';'</span><span class="SyntaxCode">
    x += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">; y += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Two instructions on the same line separated with ';'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">); </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="007_global_declaration_order">Global declaration order</h2><p>The order of all <b>top level</b> declarations does not matter. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxComment">// Here we declare a constant 'A' and initialize it with 'B', which is not</span><span class="SyntaxCode">
</span><span class="SyntaxComment">// yet known (neither its value or its type).</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
</span><span class="SyntaxComment">// Declare a constant 'B' equals to 'C', still unknown at this point.</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">C</span><span class="SyntaxCode">
</span><span class="SyntaxComment">// Declare a constant C of type 'u64' (unsigned 64 bits integer) and assigned it to 1.</span><span class="SyntaxCode">
</span><span class="SyntaxComment">// At this point A and B are then also defined.</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">C</span><span class="SyntaxCode">: </span><span class="SyntaxType">u64</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"></code>
</div>
<p>In this test, we call the function <code class="incode">functionDeclaredLater</code> before it is known. This is fine. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxComment">// First, call a unknown function named 'functionDeclaredLater'</span><span class="SyntaxCode">
</span><span class="SyntaxFunction">#run</span><span class="SyntaxCode">
{
    </span><span class="SyntaxFunction">functionDeclaredLater</span><span class="SyntaxCode">()
}

</span><span class="SyntaxComment">// Then declare it after</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">functionDeclaredLater</span><span class="SyntaxCode">()
{
}</code>
</div>
<p>Note that the order is not relevant in the same file, but it is also irrelevant across multiple files. You can for example call a function in one file and declare it in another one. Global ordrer does not matter !. </p>

<h2 id="010_basic_types">Basic types</h2><p>These are all signed integers types <code class="incode">s8</code>, <code class="incode">s16</code>, <code class="incode">s32</code> and <code class="incode">s64</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">  = -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// 8 bits signed integer</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">s16</span><span class="SyntaxCode"> = -</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// 16 bits signed integer</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = -</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// 32 bits signed integer</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> d: </span><span class="SyntaxType">s64</span><span class="SyntaxCode"> = -</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// 64 bits signed integer</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Remember that the instrinsic '@assert' will raise an error at runtime if</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// the enclosed expression is false.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == -</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == -</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(d == -</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// The instrinsic '@sizeof' gives the size, in bytes, of a variable.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)    </span><span class="SyntaxComment">// 'a' has type 's8', so this should be 1 byte.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(b) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)    </span><span class="SyntaxComment">// 'b' has type 's16', so this should be 2 bytes.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(c) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(d) == </span><span class="SyntaxNumber">8</span><span class="SyntaxCode">)
}</code>
</div>
<p>These are all unsigned integers types <code class="incode">u8</code>, <code class="incode">u16</code>, <code class="incode">u32</code> and <code class="incode">u64</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">  = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">      </span><span class="SyntaxComment">// 8 bits unsigned integer</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">u16</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">      </span><span class="SyntaxComment">// 16 bits unsigned integer</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">      </span><span class="SyntaxComment">// 32 bits unsigned integer</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> d: </span><span class="SyntaxType">u64</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">      </span><span class="SyntaxComment">// 64 bits unsigned integer</span><span class="SyntaxCode">

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(d == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(b) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(c) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(d) == </span><span class="SyntaxNumber">8</span><span class="SyntaxCode">)
}</code>
</div>
<p>These are all floating point types <code class="incode">f32</code> and <code class="incode">f64</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">3.14</span><span class="SyntaxCode">       </span><span class="SyntaxComment">// 32 bits floating point value</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">f64</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">3.14159</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// 64 bits floating point value</span><span class="SyntaxCode">

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">3.14</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">3.14159</span><span class="SyntaxCode">)

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(b) == </span><span class="SyntaxNumber">8</span><span class="SyntaxCode">)
}</code>
</div>
<p>The boolean type <code class="incode">bool</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">      </span><span class="SyntaxComment">// Stored in 1 byte</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// Stored in 1 byte</span><span class="SyntaxCode">

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(b) == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}</code>
</div>
<p>The <code class="incode">string</code> type. Strings are <b>UTF8</b>, and are stored as two 64 bits (the pointer to the value and the string length in bytes). Note that a string literal also ends with a null byte like in C. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string 是"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxString">"string 是"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> * </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(*</span><span class="SyntaxType">void</span><span class="SyntaxCode">))
}</code>
</div>
<p>The <code class="incode">rune</code> type is a 32 bits unicode code point. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">rune</span><span class="SyntaxCode"> = </span><span class="SyntaxString">`是`</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxString">`是`</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Type reflection </h3>
<p>Swag has <b>type reflection</b> at <b>compile time</b> and at <b>runtime</b>. We will see that later in more details. </p>
<p>You can use <code class="incode">@decltype</code> to create a type based on an expression. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">                   </span><span class="SyntaxComment">// 'a' is inferred to have the 's32' type</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b: </span><span class="SyntaxIntrinsic">@decltype</span><span class="SyntaxCode">(a) = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// 'b' will have the same type as 'a'</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// We can verify with '@typeof' that 'a' and 'b' have the same type.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(b))
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// As the types of 'a' and 'b' are known by the compiler, we can use '#assert' instead of '@assert'.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// The '#assert' will be done at compile time, and will not generate code (unlike '@assert').</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(b)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
}</code>
</div>
<p>Short notice that types are also values, at compile time and at runtime. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">)   </span><span class="SyntaxComment">// 'x' is now a variable that contains a type</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)   </span><span class="SyntaxComment">// So it can be compared to a type</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// A type is a predefined struct, which contains some fields to inspect it.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// You can for example get the name.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x.name == </span><span class="SyntaxString">"s32"</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// @typeof is not really necessary when there's no ambiguity about the expression.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y = </span><span class="SyntaxType">bool</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="011_number_literals">Number literals</h2><p>Integers in <i>decimal</i>, <i>hexadecimal</i> or <i>binary</i> forms. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">123456</span><span class="SyntaxCode">           </span><span class="SyntaxComment">// Decimal</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0xFFFF</span><span class="SyntaxCode">           </span><span class="SyntaxComment">// Hexadecimal, with '0x'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> c: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0b00001111</span><span class="SyntaxCode">       </span><span class="SyntaxComment">// Binary, with '0b'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">123456</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">65535</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxNumber">15</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can separate the digits with the <code class="incode">_</code> character. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">123_456</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0xF_F_F_F</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> c: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0b0000_1111</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">123456</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">65_535</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxNumber">15</span><span class="SyntaxCode">)
}</code>
</div>
<p>The default type of an hexadecimal number or a binary number is <code class="incode">u32</code> or <code class="incode">u64</code> depending on its value. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// The compiler will deduce that the type of 'a' is 'u32'.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0xFF</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">u32</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// The compiler will deduce that the type of 'b' is 'u64' because the constant</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// is too big for 32 bits.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b = </span><span class="SyntaxNumber">0xF_FFFFF_FFFFFF</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(b) == </span><span class="SyntaxType">u64</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> c = </span><span class="SyntaxNumber">0b00000001</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(c) == </span><span class="SyntaxType">u32</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> d = </span><span class="SyntaxNumber">0b00000001_00000001_00000001_00000001_00000001</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(d) == </span><span class="SyntaxType">u64</span><span class="SyntaxCode">
}</code>
</div>
<p>A boolean is <code class="incode">true</code> or <code class="incode">false</code>. Note again that as constants are known at compile time, we can  use <code class="incode">#assert</code> to check the values. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
    #assert a == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b, c = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
    #assert b == </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
    #assert c == </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
}</code>
</div>
<p>A floating point value has the usual C/C++ form. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b = </span><span class="SyntaxNumber">0.11</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">0.11</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c = </span><span class="SyntaxNumber">15e2</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxNumber">15e2</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> d = </span><span class="SyntaxNumber">15e+2</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(d == </span><span class="SyntaxNumber">15e2</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> e = -</span><span class="SyntaxNumber">1E-1</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(e == -</span><span class="SyntaxNumber">0.1</span><span class="SyntaxCode">)
}</code>
</div>
<p>By default, a floating point value is <code class="incode">f32</code>, not <code class="incode">f64</code> (aka <code class="incode">double</code>) like in C/C++. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) != </span><span class="SyntaxType">f64</span><span class="SyntaxCode">
}</code>
</div>
<h3>Postfix </h3>
<p>You can also <b>postfix</b> a literal number by a type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Declare 'a' to be a 'f64' variable assigned to '1.5'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">'</span><span class="SyntaxType">f64</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">'</span><span class="SyntaxType">f64</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">f64</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// 'b' is a new variable of type 'u8' initialized with '10'.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(b) == </span><span class="SyntaxType">u8</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(c) == </span><span class="SyntaxType">u32</span><span class="SyntaxCode">
}</code>
</div>

<h2 id="012_string">String</h2><p>In Swag, strings are encoded in UTF8. </p>
<p>They also can be compared. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">"this is a chinese character: 是"</span><span class="SyntaxCode">
    #assert a == </span><span class="SyntaxString">"this is a chinese character: 是"</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b = </span><span class="SyntaxString">"these are some cyrillic characters: ӜИ"</span><span class="SyntaxCode">
    #assert b == </span><span class="SyntaxString">"these are some cyrillic characters: ӜИ"</span><span class="SyntaxCode">
}</code>
</div>
<p>A rune is an unicode codepoint, and is 32 bits. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">rune</span><span class="SyntaxCode"> = </span><span class="SyntaxString">`是`</span><span class="SyntaxCode">
    #assert a == </span><span class="SyntaxString">`是`</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxType">u32</span><span class="SyntaxCode">)
}</code>
</div>
<p>You cannot index a string to get a rune, except in ascii strings. This is because we didn't want the runtime to come with the cost of UTF8 encoding/decoding. But note that the <code class="incode">Std.Core</code> module will have all you need to manipulate UTF8 strings. </p>
<p>So in that case you will retrieve a byte. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">"this is a chinese character: 是"</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Get the first byte of the string</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b = a[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">]
    #assert b == </span><span class="SyntaxString">`t`</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(b) == </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Here, the 'X' character in the middle does not have the index '1', because the</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// chinese character before is encoded in UTF8 with more than 1 byte.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> c = </span><span class="SyntaxString">"是X是"</span><span class="SyntaxCode">
    #assert c[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] != </span><span class="SyntaxString">`X`</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// False because the byte number 1 is not the character 'X'</span><span class="SyntaxCode">
}</code>
</div>
<p>Multiple adjacent strings are compiled as one. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">"this is "</span><span class="SyntaxCode">   </span><span class="SyntaxString">"a"</span><span class="SyntaxCode">   </span><span class="SyntaxString">" string"</span><span class="SyntaxCode">
    #assert a == </span><span class="SyntaxString">"this is a string"</span><span class="SyntaxCode">
}</code>
</div>
<p>You can concatenate some values if the values are known at compile time. Use the <code class="incode">++</code> operator for that. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">"the devil number is "</span><span class="SyntaxCode"> ++ </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
    #assert a == </span><span class="SyntaxString">"the devil number is 666"</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c = </span><span class="SyntaxString">"the devil number is not "</span><span class="SyntaxCode"> ++ (b + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">) ++ </span><span class="SyntaxString">"!"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxString">"the devil number is not 667!"</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> d = </span><span class="SyntaxString">"they are "</span><span class="SyntaxCode"> ++ </span><span class="SyntaxNumber">4</span><span class="SyntaxCode"> ++ </span><span class="SyntaxString">" apples in "</span><span class="SyntaxCode"> ++ (</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">*</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) ++ </span><span class="SyntaxString">" baskets"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(d == </span><span class="SyntaxString">"they are 4 apples in 4 baskets"</span><span class="SyntaxCode">)
}</code>
</div>
<p>A string can be <code class="incode">null</code> if not defined. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">string</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)
    a = </span><span class="SyntaxString">"null"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a != </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)
    a = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Character literals </h3>
<p>A <i>character</i> is enclosed with <b>backticks</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> char0 = </span><span class="SyntaxString">`a`</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> char1 = </span><span class="SyntaxString">`我`</span><span class="SyntaxCode">
}</code>
</div>
<p>By default, it's a lazy 32 bits integer that can be assigned to all integers (as long as it fits) and to the type <code class="incode">rune</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">   = </span><span class="SyntaxString">`a`</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">  = </span><span class="SyntaxString">`a`</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">  = </span><span class="SyntaxString">`我`</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> d: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">  = </span><span class="SyntaxString">`我`</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> e: </span><span class="SyntaxType">rune</span><span class="SyntaxCode"> = </span><span class="SyntaxString">`我`</span><span class="SyntaxCode">
    }

    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">   = </span><span class="SyntaxString">`a`</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">  = </span><span class="SyntaxString">`a`</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">  = </span><span class="SyntaxString">`我`</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> d: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">  = </span><span class="SyntaxString">`我`</span><span class="SyntaxCode">
    }
}</code>
</div>
<p>But the underlying type of a character can be forced with the use of a type postfix. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">`0`</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">48</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b = </span><span class="SyntaxString">`1`</span><span class="SyntaxCode">'</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">49</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(b) == </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c = </span><span class="SyntaxString">`2`</span><span class="SyntaxCode">'</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxNumber">50</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(c) == </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> d = </span><span class="SyntaxString">`3`</span><span class="SyntaxCode">'</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(d == </span><span class="SyntaxNumber">51</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(d) == </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> e = </span><span class="SyntaxString">`4`</span><span class="SyntaxCode">'</span><span class="SyntaxType">rune</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(e == </span><span class="SyntaxNumber">52</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(e) == </span><span class="SyntaxType">rune</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Escape sequences </h3>
<p>A string and a character can contain some <i>escape sequences</i> to specify special characters. </p>
<p>An escape sequence starts with a backslash <code class="incode">\</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">"this is code ascii 0x00:   \0"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// null</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b = </span><span class="SyntaxString">"this is code ascii 0x07:   \a"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// bell</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> c = </span><span class="SyntaxString">"this is code ascii 0x08:   \b"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// backspace</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> d = </span><span class="SyntaxString">"this is code ascii 0x09:   \t"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// horizontal tab</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> e = </span><span class="SyntaxString">"this is code ascii 0x0A:   \n"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// line feed</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> f = </span><span class="SyntaxString">"this is code ascii 0x0B:   \v"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// vertical tab</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> g = </span><span class="SyntaxString">"this is code ascii 0x0C:   \f"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// form feed</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> h = </span><span class="SyntaxString">"this is code ascii 0x0D:   \r"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// carriage return</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> i = </span><span class="SyntaxString">"this is code ascii 0x22:   \""</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// double quote</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> j = </span><span class="SyntaxString">"this is code ascii 0x27:   \'"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// single quote</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> k = </span><span class="SyntaxString">"this is code ascii 0x60:   \`"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// backtick</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> l = </span><span class="SyntaxString">"this is code ascii 0x5C:   \\"</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// backslash</span><span class="SyntaxCode">
}</code>
</div>
<p>An escape sequence can also be used to specify an ascii or a unicode value. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">"\x26"</span><span class="SyntaxCode">        </span><span class="SyntaxComment">// 1 byte, hexadecimal, extended ascii</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b = </span><span class="SyntaxString">"\u2626"</span><span class="SyntaxCode">      </span><span class="SyntaxComment">// 2 bytes, hexadecimal, unicode 16 bits</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> c = </span><span class="SyntaxString">"\U26262626"</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// 4 bytes, hexadecimal, unicode 32 bits</span><span class="SyntaxCode">
    }

    {
        </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> d = </span><span class="SyntaxString">"\u2F46\u2F46"</span><span class="SyntaxCode">
        #assert d == </span><span class="SyntaxString">"⽆⽆"</span><span class="SyntaxCode">

        </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> e = </span><span class="SyntaxString">`\u2F46`</span><span class="SyntaxCode">
        #assert e == </span><span class="SyntaxString">`⽆`</span><span class="SyntaxCode">
    }
}</code>
</div>
<h3>Raw string </h3>
<p>A <i>raw string</i> does not transform the escape sequences inside it. </p>
<p>A raw string starts and ends with the character <code class="incode">@</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">@"\u2F46"@</span><span class="SyntaxCode">
    #assert a != </span><span class="SyntaxString">"⽆"</span><span class="SyntaxCode">
    #assert a == </span><span class="SyntaxString">@"\u2F46"@</span><span class="SyntaxCode">
}</code>
</div>
<p>This are equivalent. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">"\\hello \\world"</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b = </span><span class="SyntaxString">@"\hello \world"@</span><span class="SyntaxCode">
    #assert a == b
}</code>
</div>
<p>A raw string can spawn on multiple lines because the line feed is now part of the string. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">@"this is
                a
                string
                "@</span><span class="SyntaxCode">
}</code>
</div>
<p>Every blanks <b>before</b> the ending mark <code class="incode">"@</code> will be removed from every other lines, so the string before is equivalent to :. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// this is</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// a</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// string</span><span class="SyntaxCode">
}</code>
</div>
<h3>Multiline string </h3>
<p>A multiline string starts and ends with <code class="incode">"""</code>. Unlike raw strings, they still evaluate escape sequences. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">""</span><span class="SyntaxCode"></span><span class="SyntaxString">"this is
                 a
                 string
                 "</span><span class="SyntaxCode"></span><span class="SyntaxString">""</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Equivalent to :</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// this is</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// a</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// string</span><span class="SyntaxCode">
}</code>
</div>
<p>In a multiline or a raw string, if you end a line with <code class="incode">\</code>, the following "eol" will <b>not</b> be part of the string. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">""</span><span class="SyntaxCode"></span><span class="SyntaxString">"\
              this is
              a
              string
              "</span><span class="SyntaxCode"></span><span class="SyntaxString">""</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Is equivalent to :</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// this is</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// a</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// string</span><span class="SyntaxCode">
}</code>
</div>
<h3>@stringof and @nameof </h3>
<p>You can use the instrinsic <code class="incode">@stringof</code> to return at compile time the result of a constant expression as a string. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">X</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@stringof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">X</span><span class="SyntaxCode">) == </span><span class="SyntaxString">"1"</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@stringof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">X</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">) == </span><span class="SyntaxString">"11"</span><span class="SyntaxCode">
}</code>
</div>
<p>You can also use <code class="incode">@nameof</code> to return the name of a variable, function etc. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">X</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@nameof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">X</span><span class="SyntaxCode">) == </span><span class="SyntaxString">"X"</span><span class="SyntaxCode">
}</code>
</div>

<h2 id="013_variables">Variables</h2><p>To declare a variable, use the <code class="incode">let</code> or <code class="incode">var</code> keyword, followed by <code class="incode">:</code> and then the type. </p>
<p><code class="incode">let</code> is used for a variable that cannot be changed after the initial affectation. </p>
<p><code class="incode">var</code> is used for a variable that can be changed again after the initial affectation. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Variable 'a' is of type 'u32' and its value is '1'. It cannot be changed.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Variable 'c' is declared with 'var', so it can be changed.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">42</span><span class="SyntaxCode">
    c += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxNumber">43</span><span class="SyntaxCode">)
}</code>
</div>
<p>We have already seen that we can declare multiple variables on the same line. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a, b: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">123</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">123</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">123</span><span class="SyntaxCode">)
}</code>
</div>
<p>Or. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">12</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">12</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">)
}</code>
</div>
<p>If you don't assign a value, then the variable will be initialized with its default value (0). So a variable is <b>always</b> initialized. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">string</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> c: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>But if you really do not want the variable to be initialized, you can assign it with <code class="incode">undefined</code>. To use with care, of course, but this is sometimes necessary to avoid the initialization cost. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">undefined</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">undefined</span><span class="SyntaxCode">
}</code>
</div>
<p>We have seen that the type is optional in the declaration if it can be deduced from the assignment. </p>
<p>These are a bunch of <b>type inferences</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(b) == </span><span class="SyntaxType">string</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c = </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">'</span><span class="SyntaxType">f64</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(c) == </span><span class="SyntaxType">f64</span><span class="SyntaxCode">
}</code>
</div>
<p>The same goes for multiple variables. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a, b = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(b) == </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c = </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">, d = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(d == </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(c) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(d) == </span><span class="SyntaxType">string</span><span class="SyntaxCode">
}</code>
</div>

<h2 id="014_const">Const</h2><p>If you use <code class="incode">const</code> instead of <code class="incode">var</code>, the value must be known by the compiler. There's no memory footprint if the type is a value or a string. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// These are constants and not variables. So they cannot be changed after the declaration.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
    #assert a == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> b: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    #assert b == </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
}</code>
</div>
<p>Constants can have more than just simple types. In that case, there's a memory footprint, because those constants are stored in the data segment. But that means also you could take the address of such constants at runtime. </p>
<p>This is our first static array. It contains <code class="incode">3</code> elements, and the type of the elements is <code class="incode">s32</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a: [</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr = &a[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] </span><span class="SyntaxComment">// Take the address of the first element</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// But as this is a constant, we can also test the values at compile time.</span><span class="SyntaxCode">
    #assert a[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    #assert a[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    #assert a[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
}</code>
</div>
<p>An example of a multidimensional array as a constant. We will detail arrays later. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">M4x4</span><span class="SyntaxCode">: [</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = [
        [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">],
        [</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">],
        [</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">],
        [</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">]
    ]
}</code>
</div>
<p>So the difference between <code class="incode">let</code> and <code class="incode">const</code> is that the value of a constant should be known at compile time, where the value of a <code class="incode">let</code> can be dynamic. Other than that, both must be assigned once and only once. </p>

<h2 id="015_operators">Operators</h2><p>These are all the Swag operators that can be used to manipulate variables and values. </p>
<h3>Arithmetic operators </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Addition</span><span class="SyntaxCode">
    x = x + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Substraction</span><span class="SyntaxCode">
    x = x - </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Multiplication</span><span class="SyntaxCode">
    x = x * </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Division</span><span class="SyntaxCode">
    x = x / </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Modulus</span><span class="SyntaxCode">
    x = x % </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
}</code>
</div>
<h3>Bitwise operators </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Xor</span><span class="SyntaxCode">
    x = x ^ </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Bitmask and</span><span class="SyntaxCode">
    x = x & </span><span class="SyntaxNumber">0b0000_0001</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Bitmask or</span><span class="SyntaxCode">
    x = x | </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxNumber">0b0000_0001</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Shift bits left</span><span class="SyntaxCode">
    x = x &lt;&lt; </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Shift bits right</span><span class="SyntaxCode">
    x = x &gt;&gt; </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
}</code>
</div>
<h3>Assignment operators </h3>
<p>All the arithmetic and bitwise operators have an <code class="incode">affect</code> version. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">

    x += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    x -= </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    x *= </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    x /= </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    x %= </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    x ^= </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    x |= </span><span class="SyntaxNumber">0b0000_0001</span><span class="SyntaxCode">
    x &= </span><span class="SyntaxNumber">0b0000_0001</span><span class="SyntaxCode">
    x &lt;&lt;= </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    x &gt;&gt;= </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
}</code>
</div>
<h3>Unary operators </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> y = </span><span class="SyntaxNumber">0b0000_0001</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> z = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Invert boolean</span><span class="SyntaxCode">
    x = !x

    </span><span class="SyntaxComment">// Invert bits</span><span class="SyntaxCode">
    y = ~y

    </span><span class="SyntaxComment">// Negative</span><span class="SyntaxCode">
    z = -z

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(z == -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxNumber">0b1111_1110</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Comparison operators </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// Equal</span><span class="SyntaxCode">
        a = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> ? </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode"> : </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// Not equal</span><span class="SyntaxCode">
        a = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> != </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> ? </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode"> : </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// Lower equal</span><span class="SyntaxCode">
        a = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> &lt;= </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> ? </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode"> : </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// Greater equal</span><span class="SyntaxCode">
        a = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> &gt;= </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> ? </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode"> : </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// Lower</span><span class="SyntaxCode">
        a = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> &lt; </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> ? </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode"> : </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// Greater</span><span class="SyntaxCode">
        a = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> &gt; </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> ? </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode"> : </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
    }

    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x != </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x &lt;= </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x &lt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x &gt;= </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x &gt; </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    }
}</code>
</div>
<h3>Logical operators </h3>
<p>This is <b>not</b> <code class="incode">&&</code> and <code class="incode">||</code> like in C/C++, but <code class="incode">and</code> and <code class="incode">or</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
    a = (</span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> &gt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">) </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> (</span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> &lt; </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    a = (</span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> &gt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">) </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> (</span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> &lt; </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Ternary operator </h3>
<p>The ternary operator will test an expression, and will return a value depending on the result of the test. <code class="incode">A = Expression ? B : C</code> will return <code class="incode">B</code> if the expression is true, and will return <code class="incode">C</code> if the expression is false. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Returns 1 because the expression 'true' is... true.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode"> ? </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> : </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Returns 666 because the expression 'x == 52' is false.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y = (x == </span><span class="SyntaxNumber">52</span><span class="SyntaxCode">) ? </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> : </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Spaceshift operator </h3>
<p>Operator <code class="incode">&lt;=&gt;</code> will return -1, 0 or 1 if the expression on the left is <b>lower</b>, <b>equal</b> or <b>greater</b> than the expression on the right. The returned type is <code class="incode">s32</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> &lt;=&gt; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> &lt; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> &lt;=&gt; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">  </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> == </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> &lt;=&gt; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">  </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> &gt; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"></code>
</div>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = -</span><span class="SyntaxNumber">1.5</span><span class="SyntaxCode"> &lt;=&gt; </span><span class="SyntaxNumber">2.31</span><span class="SyntaxCode">
        #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(-</span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> &lt;=&gt;  </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> == -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">( </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> &lt;=&gt; -</span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">( </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> &lt;=&gt;  </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    }

    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x1 = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> &lt;=&gt; </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x1 == -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x2 = </span><span class="SyntaxNumber">20</span><span class="SyntaxCode"> &lt;=&gt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x2 == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x3 = </span><span class="SyntaxNumber">20</span><span class="SyntaxCode"> &lt;=&gt; </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x3 == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    }
}</code>
</div>
<h3>Null-coalescing operator </h3>
<p>The operator <code class="incode">orelse</code> will return the left expression if it is not zero, otherwise it will return the right expression. </p>
<p>Works with strings, pointers and structures with the <code class="incode">opData</code> special function (we'll see that later). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxString">"string1"</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b = </span><span class="SyntaxString">"string2"</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// c = a if a is not null, else c = b.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> c = a </span><span class="SyntaxLogic">orelse</span><span class="SyntaxCode"> b
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxString">"string1"</span><span class="SyntaxCode">)

    a = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">
    c = a </span><span class="SyntaxLogic">orelse</span><span class="SyntaxCode"> b
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == </span><span class="SyntaxString">"string2"</span><span class="SyntaxCode">)
}</code>
</div>
<p>Works also for basic types like integers. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> c = a </span><span class="SyntaxLogic">orelse</span><span class="SyntaxCode"> b
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c == b)
}</code>
</div>
<h3>Type promotion </h3>
<p>Unlike C, types are not promoted to 32 bits when dealing with 8 or 16 bits types. But types will be promoted if the two sides of an operation do not have the same type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)  == </span><span class="SyntaxType">u8</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">u16</span><span class="SyntaxCode">) == </span><span class="SyntaxType">u16</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// Priority to bigger type</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">u32</span><span class="SyntaxCode">) == </span><span class="SyntaxType">u32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">u64</span><span class="SyntaxCode">) == </span><span class="SyntaxType">u64</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s8</span><span class="SyntaxCode">)  == </span><span class="SyntaxType">s8</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// Priority to signed type</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s16</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s16</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s64</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s64</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">f64</span><span class="SyntaxCode">) == </span><span class="SyntaxType">f64</span><span class="SyntaxCode">

    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">s8</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">u16</span><span class="SyntaxCode">) == </span><span class="SyntaxType">u16</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// Priority to bigger type also</span><span class="SyntaxCode">
}</code>
</div>
<p>This means that a 8/16 bits operation (like an addition) can more easily <b>overflow</b> if you do not take care. </p>
<p>In that case, you can use the <code class="incode">,up</code> <b>modifier</b> on the operation, which will promote the type to at least 32 bits. The operation will be done accordingly. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">255</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> +,up </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode">) == </span><span class="SyntaxType">u32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxNumber">255</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> +,up </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">256</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// No overflow, because the operation is done in 32 bits.</span><span class="SyntaxCode">
}</code>
</div>
<p>We'll see later how Swag deals with that kind of overflow, and more generally, with <b>safety</b>. </p>
<h3>Operator precedence </h3>
<div class="precode"><code></span><span class="SyntaxCode">~
* / %
+ -
&gt;&gt; &lt;&lt;
&
|
^
&lt;=&gt;
== !=
&lt; &lt;= &gt; &gt;=
</span><span class="SyntaxLogic">and</span><span class="SyntaxCode">
</span><span class="SyntaxLogic">or</span><span class="SyntaxCode"></code>
</div>
<p>If two operators have the same precedence, the expression is evaluated from left to right. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Multiplication before addition</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> * </span><span class="SyntaxNumber">3</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">16</span><span class="SyntaxCode">)
    </span><span class="SyntaxComment">// Parentheses change precedence</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">((</span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) * </span><span class="SyntaxNumber">3</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">36</span><span class="SyntaxCode">)
    </span><span class="SyntaxComment">// Addition and subtraction before comparison</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">((</span><span class="SyntaxNumber">5</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">3</span><span class="SyntaxCode"> &lt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> - </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) == </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxComment">// 'and' before 'or'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">((</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">) == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxComment">// '&lt;&lt;' before '&'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">((</span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> & </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> &lt;&lt; </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(((</span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> & </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) &lt;&lt; </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="016_cast">Cast</h2><h3>Explicit cast </h3>
<p>Sometimes it can be necessary to change the type of a value. Use <code class="incode">cast(type)</code> to cast from one type to another. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// 'x' is a float</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(x) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// 'y' is a 32 bits integer</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) x
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(y) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}</code>
</div>
<h3>acast </h3>
<p><code class="incode">acast</code> stands for <code class="incode">automatic</code> cast. Use it to let the compiler automatically cast to the expression on the left. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">acast</span><span class="SyntaxCode"> x    </span><span class="SyntaxComment">// cast 'x' to 's32'</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(y) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}</code>
</div>
<h3>bitcast </h3>
<p>Use <code class="incode">cast,bit</code> to convert a native type to another without converting the value. Works only if the two types are of the same size. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">,</span><span class="SyntaxFunction">bit</span><span class="SyntaxCode">(</span><span class="SyntaxType">u32</span><span class="SyntaxCode">) x
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxNumber">0x3f800000</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">,</span><span class="SyntaxFunction">bit</span><span class="SyntaxCode">(</span><span class="SyntaxType">u32</span><span class="SyntaxCode">) </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0x3f800000</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">,</span><span class="SyntaxFunction">bit</span><span class="SyntaxCode">(</span><span class="SyntaxType">f32</span><span class="SyntaxCode">) </span><span class="SyntaxNumber">0x3f800000</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">
}</code>
</div>
<h3>Implicit casts </h3>
<p>Swag can sometimes cast from one type to another for you. This is an <i>implicit</i> cast. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// An implicit cast is done if there's no loss of precision.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// In that case, you can affect different types.</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x: </span><span class="SyntaxType">s16</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s8</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// 8 bits to 16 bits, fine</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s16</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// 16 bits to 32 bits, fine</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// But the following would generate an error as you cannot affect 's16' to 's8'</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// without an explicit cast(s16).</span><span class="SyntaxCode">

    /*
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> z0: </span><span class="SyntaxType">s16</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> z1: </span><span class="SyntaxType">s8</span><span class="SyntaxCode"> = z0
    */
}</code>
</div>

<h2 id="020_array">Array</h2><p>Remember that dynamic arrays are part of the <code class="incode">Std.Core</code> module. Here we are only talking about native static arrays. </p>
<p>A static array is declared with <code class="incode">[N]</code> followed by the type, where <code class="incode">N</code> is the dimension. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// Static array of two s32</span><span class="SyntaxCode">
        array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    }
}</code>
</div>
<p>You can get the number of elements of an array with <code class="incode">@countof</code>, and the size in bytes with <code class="incode">@sizeof</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(array) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(array) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> * </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can get the address of the array with <code class="incode">@dataof</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> ptr0 = </span><span class="SyntaxIntrinsic">@dataof</span><span class="SyntaxCode">(array)
    ptr0[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// This is equivalent of taking the address of the first element</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> ptr1 = &array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">]
    ptr1[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>An <b>array literal</b> has the form <code class="incode">[A, B, ...]</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] </span><span class="SyntaxComment">// An array of four s32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(arr) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(arr) == </span><span class="SyntaxType">#type</span><span class="SyntaxCode"> [</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
}</code>
</div>
<p>The size of the array can be deduced from the initialisation expression. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Here the dimension is not specified, but as the array is initialized with 2 elements, it can be deduced.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [] </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(array) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Here both dimensions and types are deduced thanks to the initialization expression.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array1 = [</span><span class="SyntaxString">"10"</span><span class="SyntaxCode">, </span><span class="SyntaxString">"20"</span><span class="SyntaxCode">, </span><span class="SyntaxString">"30"</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array1[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxString">"10"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array1[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxString">"20"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array1[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] == </span><span class="SyntaxString">"30"</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(array1) == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">
}</code>
</div>
<p>Like every other types, an array is initialized by default to 0. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>But for speed, you can force the array to be not initialized with <code class="incode">undefined</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [</span><span class="SyntaxNumber">100</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">undefined</span><span class="SyntaxCode">
}</code>
</div>
<p>A static array (with compile time values) can be stored as a constant. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> array = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">]
    #assert array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// Dereference is done at compile time</span><span class="SyntaxCode">
    #assert array[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">
}</code>
</div>
<p>If the type of the array is not specified, the type of the <b>first</b> literal value will be used for all other members. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">f64</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">]    </span><span class="SyntaxComment">// Every values are forced to be 'f64'</span><span class="SyntaxCode">

    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(arr) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(arr) == </span><span class="SyntaxType">#type</span><span class="SyntaxCode"> [</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] </span><span class="SyntaxType">f64</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(arr[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">4.0</span><span class="SyntaxCode">)
}</code>
</div>
<p>Of course an array can have multiple dimensions. </p>
<p>Syntax is <code class="incode">[X, Y, Z...]</code> where <code class="incode">X</code>, <code class="incode">Y</code> and <code class="incode">Z</code> are dimensions. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// Declare a 2x2 array</span><span class="SyntaxCode">
    array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">         </span><span class="SyntaxComment">// To access it, the same syntax is used</span><span class="SyntaxCode">
    array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">
    array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">
}</code>
</div>
<p>But the C/C++ syntax is also accepted. You will then declare an array of array instead of an array with multiple dimensions, which in fact is the same... </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">][</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">
    array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">
}</code>
</div>
<p>The sizes can be deduced from the initialization expression too. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">]
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array1 = [[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">], [</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">]]

    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(array) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(array1) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
}</code>
</div>
<p>You can initialize a whole array variable (but <b>not a constant</b>) with one single value. Only basic types are accepted (integers, float, string, bool, rune). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// The whole array is initialized with 'true'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr: [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(arr[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(arr[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// The whole array is initialized with 'string'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr1: [</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">] </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(arr1[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(arr1[</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">9</span><span class="SyntaxCode">] == </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="021_slice">Slice</h2><p>A slice is a pointer on a buffer of datas, and a <code class="incode">u64</code> to count the number of elements. Unlike a static array, its value can be changed at runtime. </p>
<p>It is declared with <code class="incode">[..]</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a: [..] </span><span class="SyntaxType">bool</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// Slice of bools</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(*</span><span class="SyntaxType">void</span><span class="SyntaxCode">) + </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxType">u64</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can initialize a slice like an array. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">50</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a[</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">50</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// But as this is a slice and not a static array, you can change the value at runtime.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// So now 'a' is a slice of two s32 values.</span><span class="SyntaxCode">
    a = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>At runtime, <code class="incode">@dataof</code> will return the address of the values, <code class="incode">@countof</code> the number of elements. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">50</span><span class="SyntaxCode">]
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> count = </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(a)
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> addr = </span><span class="SyntaxIntrinsic">@dataof</span><span class="SyntaxCode">(a)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(count == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(addr[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(addr[</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">50</span><span class="SyntaxCode">)

    a = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can create a slice with your own <code class="incode">pointer</code> and <code class="incode">count</code> using <code class="incode">@mkslice</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">]

    </span><span class="SyntaxComment">// Creates a slice of 'array' (a view, or subpart) starting at index 2, with 2 elements.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice: [..] </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxIntrinsic">@mkslice</span><span class="SyntaxCode">(&array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] + </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(slice) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
    slice[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] = </span><span class="SyntaxNumber">314</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">314</span><span class="SyntaxCode">)
}</code>
</div>
<p>For a string, the slice must be <code class="incode">const</code> because a string is immutable. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> str = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> strSlice: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode"> = </span><span class="SyntaxIntrinsic">@mkslice</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@dataof</span><span class="SyntaxCode">(str), </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(strSlice[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxString">`s`</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(strSlice[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxString">`t`</span><span class="SyntaxCode">)
}</code>
</div>
<h3>The slicing operator </h3>
<p>Instead of <code class="incode">@mkslice</code>, you can slice something with the <code class="incode">..</code> operator. For example you can slice a string. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> str = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Creates a slice starting at byte 0 and ending at byte 3.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice = str[</span><span class="SyntaxNumber">1.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">]

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice == </span><span class="SyntaxString">"tri"</span><span class="SyntaxCode">)
}</code>
</div>
<p>The upper limit is <b>included</b> by default. If you want to exclude it, use <code class="incode">..&lt;</code> insteand of <code class="incode">..</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> str = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice = str[</span><span class="SyntaxNumber">1.</span><span class="SyntaxCode">.&lt;</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice == </span><span class="SyntaxString">"tr"</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can omit the upper bound if you want to slice to the end. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> str = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice = str[</span><span class="SyntaxNumber">2.</span><span class="SyntaxCode">.]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice == </span><span class="SyntaxString">"ring"</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can omit the lower bound if you want to slice from the start (0). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> str = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice = str[..</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]        </span><span class="SyntaxComment">// Index 2 is included</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice == </span><span class="SyntaxString">"str"</span><span class="SyntaxCode">)
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice1 = str[..&lt;</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]      </span><span class="SyntaxComment">// Index 2 is excluded</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice1 == </span><span class="SyntaxString">"st"</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can also slice an array. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> arr = [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">]
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice = arr[</span><span class="SyntaxNumber">2.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(slice) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Creates a slice for the whole array</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice1 = arr[..]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(slice1) == </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(arr))
}</code>
</div>
<p>You can slice another slice. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> arr = [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">]
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice1 = arr[</span><span class="SyntaxNumber">1.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice1[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice1[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice1[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice2 = slice1[</span><span class="SyntaxNumber">1.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice2[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice2[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can transform a pointer to a slice. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr = [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">]
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr = &arr[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> slice = ptr[</span><span class="SyntaxNumber">0.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(slice[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(slice) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="022_pointers">Pointers</h2><h3>Single value pointers </h3>
<p>A pointer to a <b>single element</b> is declared with <code class="incode"><i></code>.</i> </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> ptr1: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// This is a pointer to one single 'u8'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> ptr2: **</span><span class="SyntaxType">u8</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// This is a pointer to one other pointer to one single 'u8'</span><span class="SyntaxCode">
}</code>
</div>
<p>A pointer can be <code class="incode">null</code> (i know some of you will collapse here). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> ptr1: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr1 == </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can take the address of something with <code class="incode">&</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr = &arr </span><span class="SyntaxComment">// Take the address of the variable 'arr'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(ptr) == *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can get the pointed value with <code class="incode">dref</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr = </span><span class="SyntaxNumber">42</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr = &arr
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">dref</span><span class="SyntaxCode"> ptr == </span><span class="SyntaxNumber">42</span><span class="SyntaxCode">)
}</code>
</div>
<p>Pointers can be <code class="incode">const</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> str = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> = </span><span class="SyntaxIntrinsic">@dataof</span><span class="SyntaxCode">(str)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">dref</span><span class="SyntaxCode"> ptr == </span><span class="SyntaxString">`s`</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can be weird, but is this necessary ?. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> ptr:  *</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">        </span><span class="SyntaxComment">// Normal pointer to a const pointer</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> ptr1: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// Const pointer to a const pointer</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> ptr2: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> **</span><span class="SyntaxType">u8</span><span class="SyntaxCode">        </span><span class="SyntaxComment">// Const pointer to a normal pointer</span><span class="SyntaxCode">
}</code>
</div>
<h3>Multiple values pointers </h3>
<p>If you want to enable <b>pointer arithmetic</b>, and make a pointer to <b>multiple values</b>, declare your pointer with <code class="incode">^</code> instead of <code class="incode"><i></code>.</i> </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// `ptr` is a pointer to a memory block of `u8`.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> ptr: ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Pointer arithmetic is now possible</span><span class="SyntaxCode">
    ptr = ptr - </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
}</code>
</div>
<p>Taking the address of an array element enables pointer arithmetic. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: [</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> ptr = &x[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">]
    ptr = ptr - </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(ptr) == ^</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
}</code>
</div>
<p>As pointer arithmetic is enabled, you can dereference that kind of pointer by index. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">]
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr = &arr[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(ptr) == ^</span><span class="SyntaxType">s32</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// The type of 'ptr' is ^s32, so it can be dereferenced by index</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value1 = ptr[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value1 == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(value1) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value2 = ptr[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value2 == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(value2) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// But 'dref' still works for the first element</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value = </span><span class="SyntaxKeyword">dref</span><span class="SyntaxCode"> ptr
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(value) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
}</code>
</div>

<h2 id="023_references">References</h2><p>Swag has also <b>references</b>, which are pointers that behave like values. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">42</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Use 'ref' to declare a reference.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Here we declare a reference to the variable 'x'.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Note that unlike C++, you have to take the address of 'x' to convert it to a reference.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> myRef: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">ref</span><span class="SyntaxCode"> </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = &x

    </span><span class="SyntaxComment">// This is a pointer that behaves like a value, so no explicit dereferencing is necessary.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// You can see this as a kind of an alias.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(myRef == </span><span class="SyntaxNumber">42</span><span class="SyntaxCode">)
}</code>
</div>
<p>When an affectation is done outside of an initialization, you will change the pointed value, and not the reference itself. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">42</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> myRef: </span><span class="SyntaxKeyword">ref</span><span class="SyntaxCode"> </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = &x </span><span class="SyntaxComment">// Note here that the reference is no more 'const'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(myRef == </span><span class="SyntaxNumber">42</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Here we will change the pointed value 'x'</span><span class="SyntaxCode">
    myRef = </span><span class="SyntaxNumber">66</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(myRef == </span><span class="SyntaxNumber">66</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Remember that 'myRef' is an alias for 'x', so 'x' has also been changed.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">66</span><span class="SyntaxCode">)
}</code>
</div>
<p>But unlike C++, you can change the reference (reassign it) and not the pointed value if you want. </p>
<p>You must then use <code class="incode">ref</code> in the affectation. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> y = </span><span class="SyntaxNumber">1000</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> myRef: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">ref</span><span class="SyntaxCode"> </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = &x
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(myRef == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Here we force 'myRef' to point to 'y' and not to 'x' anymore.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// We do *NOT* change the value of 'x'.</span><span class="SyntaxCode">
    myRef = </span><span class="SyntaxKeyword">ref</span><span class="SyntaxCode"> &y
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(myRef == </span><span class="SyntaxNumber">1000</span><span class="SyntaxCode">)
}</code>
</div>
<p>Most of the time, you have to take the address of a variable to make a reference to it. The only exception are function parameters, if the reference is <code class="incode">const</code>. In that case, taking the address is not necessary. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// We can pass a literal because the parameter 'x' of 'toto' is 'const ref' and not just 'ref'.</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">(x: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">ref</span><span class="SyntaxCode"> </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)
{
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Under the hood, you will get a const address to an 's32'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr = &x
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">dref</span><span class="SyntaxCode"> ptr == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}</code>
</div>
<p>This is usefull for structs for examples, as you can directly pass a literal to a function. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode"> {x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> }  </span><span class="SyntaxComment">// Our first little struct !</span><span class="SyntaxCode">

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxFunction">titi0</span><span class="SyntaxCode">({</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">})
    </span><span class="SyntaxFunction">titi1</span><span class="SyntaxCode">({</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">})
    </span><span class="SyntaxFunction">titi2</span><span class="SyntaxCode">({</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">})
}

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">titi0</span><span class="SyntaxCode">(param: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">ref</span><span class="SyntaxCode"> {x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">})
{
    </span><span class="SyntaxComment">// We'll see later about tuples and naming of fields.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(param.item0 == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(param.item1 == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>Note that declaring a tuple type or a struct type is equivalent to a constant reference. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">titi1</span><span class="SyntaxCode">(param: {x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">})
{
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(param.x == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(param.y == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">titi2</span><span class="SyntaxCode">(param: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">)
{
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(param.x == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(param.y == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="024_any">Any</h2><p><code class="incode">any</code> is a specific type that can store every other types. <code class="incode">any</code> is <b>not a variant</b>. It's a dynamic typed reference to an existing value. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">any</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Store a 's32' literal value</span><span class="SyntaxCode">
    a = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// In order to reference the value inside the 'any', you need to cast to the wanted type.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) a == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Then now we store a string instead of the 's32' value</span><span class="SyntaxCode">
    a = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">string</span><span class="SyntaxCode">) a == </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Then we store a bool instead</span><span class="SyntaxCode">
    a = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">bool</span><span class="SyntaxCode">) a == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
}</code>
</div>
<p><code class="incode">any</code> is in fact a pointer to the value, and a <code class="incode">typeinfo</code>. <code class="incode">@dataof</code> can be used to retrieve the pointer to the value. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">any</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxIntrinsic">@dataof</span><span class="SyntaxCode">(a)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">dref</span><span class="SyntaxCode"> ptr == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">)
}</code>
</div>
<p><code class="incode">@typeof</code> will give you the type <code class="incode">any</code>, but <code class="incode">@kindof</code> will give you the real underlying type. In that case, <code class="incode">@kindof</code> is evaluted at runtime. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">any</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">any</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">string</span><span class="SyntaxCode">)

    a = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(a) == </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can declare an array with multiple types, with <code class="incode">any</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [] </span><span class="SyntaxType">any</span><span class="SyntaxCode"> = [</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3.0</span><span class="SyntaxCode">, </span><span class="SyntaxString">"4"</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">string</span><span class="SyntaxCode">)

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">bool</span><span class="SyntaxCode">) array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">f32</span><span class="SyntaxCode">) array[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">3.0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">string</span><span class="SyntaxCode">) array[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">] == </span><span class="SyntaxString">"4"</span><span class="SyntaxCode">)
}</code>
</div>
<p>An <code class="incode">any</code> can be set to null, and tested against null. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxType">any</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)
    x = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x != </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) x == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">)
    x = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x != </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">string</span><span class="SyntaxCode">) x == </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">)
    x = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="025_tuple">Tuple</h2><p>A tuple is an anonymous structure, aka a struct literal. Syntax is <code class="incode">{}</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> tuple1 = {</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">}
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> tuple2 = {</span><span class="SyntaxString">"string"</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">}
}</code>
</div>
<p>Tuple values have default names to access them, in the form of <code class="incode">itemX</code> where <code class="incode">X</code> is the field rank. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> tuple = {</span><span class="SyntaxString">"string"</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">}
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(tuple.item0 == </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(tuple.item1 == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(tuple.item2 == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
}</code>
</div>
<p>But you can specify your own names. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> tuple = {x = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">, y = </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">}
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(tuple.x == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(tuple.item0 == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(tuple.y == </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(tuple.item1 == </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">)
}</code>
</div>
<p>When creating a tuple literal with variables, the tuple fields will take the name of the variables (except if specified). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> t = {x, y}
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(t.x == </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(t.item0 == t.x)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(t.y == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(t.item1 == t.y)
}</code>
</div>
<p>Even if two tuples do not have the same field names, they can be assigned to each other if the field types are the same. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: {a: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">}
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> y: {c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, d: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">}

    y = {</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">}
    x = y
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x.a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x.b == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// But note that 'x' and 'y' to not have the same type</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(x) != </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(y)
}</code>
</div>
<h3>Tuple unpacking </h3>
<p>You can unpack a tuple field by field. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> tuple1 = {x = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">, y = </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">}

    </span><span class="SyntaxComment">// 'value0' will be assigned with 'x', and 'value1' will be assigned with 'y'.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> (value0, value1) = tuple1
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value0 == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value1 == </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> tuple2 = {</span><span class="SyntaxString">"name"</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">}
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> (name, value) = tuple2
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(name == </span><span class="SyntaxString">"name"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can ignore a tuple field by naming the variable <code class="incode">?</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> tuple1 = {x = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">, y = </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">}
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> (x, ?) = tuple1
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> (?, y) = tuple1
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="030_enum">Enum</h2><p>Enums values, unlike C/C++, can end with <code class="incode">;</code> or <code class="incode">,</code> or an <code class="incode">eol</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values0</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values1</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">,
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">,
    }

    </span><span class="SyntaxComment">// The last comma is not necessary</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values2</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">,
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values3</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> }
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values4</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">; }
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values5</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> }
}</code>
</div>
<p>By default, an enum is of type <code class="incode">s32</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> }
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> type = </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// 'type' here is a 'typeinfo' dedicated to the enum type.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// In that case, 'type.rawType' is the enum underlying type.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(type.rawType == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">) == </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">
}</code>
</div>
<p><code class="incode">@kindof</code> will return the underlying type of the enum. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">R</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">G</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> }
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">) == </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">) != </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
}</code>
</div>
<p>You can specify your own type after the enum name. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values1</span><span class="SyntaxCode">: </span><span class="SyntaxType">s64</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// Forced to be 's64' instead of 's32'</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">C</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Values1</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s64</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Values1</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">) == </span><span class="SyntaxConstant">Values1</span><span class="SyntaxCode">
}</code>
</div>
<p>Enum values, if not specified, start at 0 and are increased by 1 at each new value. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">C</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
}</code>
</div>
<p>You can specify your own values. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">
}</code>
</div>
<p>If you omit one value, it will be assigned to the previous value + 1. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// 11</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// 12</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">11</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">12</span><span class="SyntaxCode">
}</code>
</div>
<p>For non integer types, you <b>must</b> specify the values as they cannot be deduced. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value1</span><span class="SyntaxCode">: </span><span class="SyntaxType">string</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string 1"</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string 2"</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string 3"</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxConstant">Value1</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> == </span><span class="SyntaxString">"string 1"</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value1</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxString">"string 2"</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value1</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> == </span><span class="SyntaxString">"string 3"</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value2</span><span class="SyntaxCode">: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">3.14</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxConstant">Value2</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value2</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">3.14</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value2</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
}</code>
</div>
<p><code class="incode">@countof</code> returns the number of values of an enum. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">: </span><span class="SyntaxType">string</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string 1"</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string 2"</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string 3"</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">
}</code>
</div>
<p>You can use the keyword <code class="incode">using</code> for an enum. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">C</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// No need to say 'Value.A' thanks to the 'using' above</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// But the normal form is still possible</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Enum as flags </h3>
<p>An enum can be a set of flags if you declare it with the <code class="incode">#<a href="swag.runtime.html#Swag_EnumFlags">Swag.EnumFlags</a></code> <b>attribute</b>. Its type should be <code class="incode">u8</code>, <code class="incode">u16</code>, <code class="incode">u32</code> or <code class="incode">u64</code>. </p>
<p>That kind of enum starts by default at 1, and not 0, and each value should be a power of 2. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.EnumFlags]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// First value is 1 and *not* 0</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// Value is 2</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">C</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// Value is 4</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">D</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// Value is 8</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0b00000001</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0b00000010</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0b00000100</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">D</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0b00001000</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value = </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> | </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value == </span><span class="SyntaxNumber">0b00000110</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value & </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> == </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value & </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> == </span><span class="SyntaxConstant">MyFlags</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Enum of arrays </h3>
<p>You can have an enum of const static arrays. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">]
    }

    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">
}</code>
</div>
<h3>Enum of slices </h3>
<p>You can have an enum of const slices. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
        </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">]
    }

    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y = </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Enum type inference </h3>
<p>The type of the enum is not necessary in the assignement expression when declaring a variable. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">; }

    </span><span class="SyntaxComment">// The normal form</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x: </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// But in fact 'Values' is not necessary because it can be deduced</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y: </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == y)
}</code>
</div>
<p>The enum type is not necessary in a <code class="incode">case</code> expression of a <code class="incode">switch</code> block (it is deduced from the switch expression). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">; }
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// The 'normal' form</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> x
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">: </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">: </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// The 'simplified' form</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> x
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">: </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">: </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    }
}</code>
</div>
<p>In an expression, and if the enum name can be deduced, you can omit it and use the <code class="incode">.Value</code> syntax. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> }

    </span><span class="SyntaxComment">// The normal form</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// The simplified form, because 'Values' can be deduced from type of x</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == .</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x != .</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">)
}</code>
</div>
<p>Works also for flags. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.EnumFlags]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> }

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> | </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">((x & .</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">) </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> (x & .</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">))
}</code>
</div>
<p>Works also (most of the time), for functions. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">; }
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">(v1, v2: </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">) {}
    </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">(.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">, .</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">)
}</code>
</div>
<p>By type reflection, you can loop/visit all values of an enum. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">R</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">G</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> }

        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> idx: </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
            cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)

        </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> val: </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> val
            {
            </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">R</span><span class="SyntaxCode">:     </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G</span><span class="SyntaxCode">:     </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">:     </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">default</span><span class="SyntaxCode">:    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
            }
        }
    }
}</code>
</div>

<h2 id="031_impl">Impl</h2><p><code class="incode">impl</code> can be used to declare some stuff in the scope of an enum. The keyword <code class="incode">self</code> represents the enum value. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">R</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">G</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">; }</code>
</div>
<p>Note the <code class="incode">impl enum</code> syntax. We'll see later that <code class="incode">impl</code> is also used for structs. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isRed</span><span class="SyntaxCode">(self)       =&gt; self == </span><span class="SyntaxConstant">R</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isRedOrBlue</span><span class="SyntaxCode">(self) =&gt; self == </span><span class="SyntaxConstant">R</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> self == </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">isRed</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">R</span><span class="SyntaxCode">))
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">isRedOrBlue</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">))

    </span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">isRedOrBlue</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">R</span><span class="SyntaxCode">))
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">isRedOrBlue</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">))

    </span><span class="SyntaxComment">// A first look at 'ufcs' (uniform function call syntax)</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">R</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">isRedOrBlue</span><span class="SyntaxCode">())
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(!</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">G</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">isRedOrBlue</span><span class="SyntaxCode">())
}</code>
</div>

<h2 id="035_namespace">Namespace</h2><p>You can create a global scope with a namespace. All symbols inside the namespace will be in the corresponding global scope. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">namespace</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">a</span><span class="SyntaxCode">() =&gt; </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
}</code>
</div>
<p>You can also specify more than one name. Here <code class="incode">C</code> will be a namespace inside <code class="incode">B</code> which is itself inside <code class="incode">A</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">namespace</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">a</span><span class="SyntaxCode">() =&gt; </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">a</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">B</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">C</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">a</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can also put <code class="incode">using</code> in front of the namespace to be able to access the content without scoping in the <b>current</b> file. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">namespace</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Private</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FileSymbol</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">B</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Private</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FileSymbol</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">C</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">FileSymbol</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// No need to specify 'Private' because of the 'using'</span><span class="SyntaxCode"></code>
</div>
<p>This is equivalent to <code class="incode">private</code>, but you don't have to specify a name, the compiler will generate it for you. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">private</span><span class="SyntaxCode"> {
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OtherSymbol</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">D</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">OtherSymbol</span><span class="SyntaxCode"></code>
</div>
<p>All symbols from a Swag source file are exported to other files of the same module. So using <code class="incode">private</code> can protect from name conflicts. </p>

<h2 id="050_if">If</h2><p>A basic test with <code class="incode">if</code>. Curlies are optional, and the expression doesn't need to be enclosed with <code class="incode">()</code> like in C/C++. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> (a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"> {
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxComment">// 'else' is of course also a thing</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
        a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">else</span><span class="SyntaxCode">
        a += </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// 'elif' is equivalent to 'else if'</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">else</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">elif</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">elif</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Boolean expression with 'and' and 'or'</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can also at the same time declare and test one variable in an <code class="incode">if</code> expression. <code class="incode">var</code>, <code class="incode">let</code> or <code class="incode">const</code> is mandatory in that case. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// This will declare a variable 'a', and test it against 0.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 'a' is then only visible in the 'if' block, and not outside.</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxComment">// So you can redeclare 'a' (this time as a constant).</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">else</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)

    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">else</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="051_loop">Loop</h2><p><code class="incode">loop</code> are used to iterate a given amount of time. </p>
<p>The loop expression value is evaluated <b>once</b>, and must be a <b>positive value</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// Loops 10 times</span><span class="SyntaxCode">
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
}</code>
</div>
<p>The intrinsic <code class="incode">@index</code> returns the current index of the loop (starting at 0). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
    {
        cpt += </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can name that index if you want. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt1 = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> i: </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// index is named 'i'</span><span class="SyntaxCode">
    {
        cpt  += i
        cpt1 += </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// @index is always available, even when named</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt  == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt1 == cpt)
}</code>
</div>
<p><code class="incode">loop</code> can be used on every types that accept the <code class="incode">@countof</code> intrinsic. So you can loop on a slice, an array, a string... and we'll see later, even on a struct. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr = [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">]
    #assert </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(arr) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> arr    </span><span class="SyntaxComment">// The array contains 4 elements, so the loop count is 4</span><span class="SyntaxCode">
        cpt += arr[</span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">20</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">30</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)
}</code>
</div>
<blockquote>
<p><b>Warning !</b> On a string, it will loop for each byte, <b>not</b> runes (if a rune is encoded in more than one byte). If you want to iterate on all runes, you will have to use the <code class="incode">Std.Core</code> module. </p>
</blockquote>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxString">"⻘"</span><span class="SyntaxCode">
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// cpt is equal to 3 because '⻘' is encoded with 3 bytes</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<h3>break, continue </h3>
<p><code class="incode">break</code> and <code class="incode">continue</code> can be used inside a loop. </p>
<p>You can exit a loop with <code class="incode">break</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> x: </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> x == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can force to return to the loop evaluation with <code class="incode">continue</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> x: </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> x == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">continue</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// Do not count 5</span><span class="SyntaxCode">
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">9</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Ranges </h3>
<p>Loop can also be used to iterate on a <b>range</b> of signed values. </p>
<p>Syntax is <code class="incode">lower bound..upper bound</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> count = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> sum = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> i: -</span><span class="SyntaxNumber">1.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// loop from -1 to 1, all included</span><span class="SyntaxCode">
    {
        count += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        sum += i
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(sum == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(count == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p>With a range, you can loop in reverse order. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Loop from 5 to 0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">5.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    {
    }

    </span><span class="SyntaxComment">// Loop from 1 to -1</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">1.</span><span class="SyntaxCode">.-</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    {
    }
}</code>
</div>
<p>You can exclude the last value with the <code class="incode">..&lt;</code> syntax. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Will loop from 1 to 2 and **not** 1 to 3</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> i: </span><span class="SyntaxNumber">1.</span><span class="SyntaxCode">.&lt;</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">
    {
        cpt += i
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<h3>Infinite loop </h3>
<p>A loop without an expression but with a block is infinite. This is equivalent to <code class="incode">while true {}</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// @index is still valid in that case (but cannot be renamed)</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    }
}</code>
</div>

<h2 id="052_visit">Visit</h2><p><code class="incode">visit</code> is used to visit all the elements of a collection. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Here we visit every bytes of the string.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// At each iteration, the byte will be stored in the variable 'value'</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> value: </span><span class="SyntaxString">"ABC"</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxComment">// '@index' is also available. It stores the loop index.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> a
        {
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value == </span><span class="SyntaxString">`A`</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value == </span><span class="SyntaxString">`B`</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value == </span><span class="SyntaxString">`C`</span><span class="SyntaxCode">)
        }
    }
}</code>
</div>
<p>You can name both the <b>value</b> and the loop <b>index</b>, in that order. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> value, index: </span><span class="SyntaxString">"ABC"</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = index
        </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> a
        {
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value == </span><span class="SyntaxString">`A`</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value == </span><span class="SyntaxString">`B`</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value == </span><span class="SyntaxString">`C`</span><span class="SyntaxCode">)
        }
    }
}</code>
</div>
<p>Both names are optional. In that case, you can use <code class="incode">@alias0</code> and <code class="incode">@alias1</code>. <code class="incode">@alias0</code> for the value, and <code class="incode">@alias1</code> for the index. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> </span><span class="SyntaxString">"ABC"</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxIntrinsic">@alias1</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// This is the index</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> a
        {
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> == </span><span class="SyntaxString">`A`</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> == </span><span class="SyntaxString">`B`</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> == </span><span class="SyntaxString">`C`</span><span class="SyntaxCode">)
        }
    }
}</code>
</div>
<p>You can visit arrays or slices. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array = [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">]

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> it: array
        result += it

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(result == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">20</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
}</code>
</div>
<p>Works also for multi dimensional arrays. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = [[</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">], [</span><span class="SyntaxNumber">30</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">]]

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> it: array
        result += it

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(result == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">20</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">30</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can visit with a pointer to the value, and not the value itself, by adding <code class="incode">&</code> before the value name. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> array: [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = [[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">], [</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">]]

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> &it: array
    {
        result += </span><span class="SyntaxKeyword">dref</span><span class="SyntaxCode"> it
        </span><span class="SyntaxKeyword">dref</span><span class="SyntaxCode"> it = </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(result == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(array[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="053_for">For</h2><p><code class="incode">for</code> accepts a <i>start statement</i>, an <i>expression to test</i>, and an <i>ending statement</i>. This is in fact the same as the C/C++ <code class="incode">for</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">for</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> i = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; i &lt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">; i += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// ';' can be replaced by a newline (like always)</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">for</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> i = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
        i &lt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        i += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    {
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Instead of one single expression, you can use a statement</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">for</span><span class="SyntaxCode"> { </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> i = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; }
        i &lt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        i += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    {
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)

    </span><span class="SyntaxLogic">for</span><span class="SyntaxCode"> { </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> i = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; }
        i &lt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        { i += </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">; i -= </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">; }
    {
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
}</code>
</div>
<p>Like <code class="incode">loop</code>, <code class="incode">visit</code> and <code class="incode">while</code>, you have access to <code class="incode">@index</code>, the <b>current loop index</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">for</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> i: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">; i &lt; </span><span class="SyntaxNumber">15</span><span class="SyntaxCode">; i += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        cpt += </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt1 = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">for</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> i = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">; i &lt; </span><span class="SyntaxNumber">15</span><span class="SyntaxCode">; i += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        cpt1 += </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt1 == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="054_while">While</h2><p><code class="incode">while</code> is a loop that runs <b>until the expression is false</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> i = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">while</span><span class="SyntaxCode"> i &lt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        i += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(i == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can also <code class="incode">break</code> and <code class="incode">continue</code> inside a <code class="incode">while</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> i = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">while</span><span class="SyntaxCode"> i &lt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> i == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
        i += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(i == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="055_switch">Switch</h2><p><code class="incode">switch</code> works like in C/C++, except that no <code class="incode">break</code> is necessary (except if the <code class="incode">case</code> is empty). That means that there's no automatic <code class="incode">fallthrough</code> from one case to another. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> value
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">:  </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">:  </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">:  </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">default</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ch = </span><span class="SyntaxString">`A`</span><span class="SyntaxCode">'</span><span class="SyntaxType">rune</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> ch
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxString">`B`</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxString">`A`</span><span class="SyntaxCode">: </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    }
}</code>
</div>
<p>You can put multiple values on the same <code class="incode">case</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> value
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">:   </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">default</span><span class="SyntaxCode">:        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> value
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">,
            </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">,
            </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">:
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">default</span><span class="SyntaxCode">:
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p><code class="incode">switch</code> works with every types that accept the <code class="incode">==</code> operator. So you can switch on strings for example. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value = </span><span class="SyntaxString">"myString"</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> value
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxString">"myString"</span><span class="SyntaxCode">:    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxString">"otherString"</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">default</span><span class="SyntaxCode">:            </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p>If you want to pass from one case to another like in C/C++, use <code class="incode">fallthrough</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> value
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">:
        </span><span class="SyntaxLogic">fallthrough</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">7</span><span class="SyntaxCode">:
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">default</span><span class="SyntaxCode">:
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p><code class="incode">break</code> can be used to exit the current <code class="incode">case</code> statement. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> value
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">:
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> value == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">default</span><span class="SyntaxCode">:
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p>A <code class="incode">case</code> statement cannot be empty. Use <code class="incode">break</code> if you want to do nothing. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> value
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">:     </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">:     </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">default</span><span class="SyntaxCode">:    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p><code class="incode">switch</code> can be marked with <code class="incode">Swag.Complete</code> to force all the cases to be covered. If one or more values are missing, an error will be raised by the compiler. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Color</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">Red</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Green</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Blue</span><span class="SyntaxCode"> }
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> color = </span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Red</span><span class="SyntaxCode">

    </span><span class="SyntaxAttribute">#[Swag.Complete]</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> color
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Red</span><span class="SyntaxCode">:     </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Green</span><span class="SyntaxCode">:   </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Blue</span><span class="SyntaxCode">:    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p>If the switch expression is omitted, then it will behave like a serie of if/else, resolved in order. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> value1 = </span><span class="SyntaxString">"true"</span><span class="SyntaxCode">

    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode">
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> value == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> value == </span><span class="SyntaxNumber">7</span><span class="SyntaxCode">:
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">fallthrough</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> value1 == </span><span class="SyntaxString">"true"</span><span class="SyntaxCode">:
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">default</span><span class="SyntaxCode">:
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p>When used on an <code class="incode">any</code> variable, switch is done on the underlying variable type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x: </span><span class="SyntaxType">any</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"value"</span><span class="SyntaxCode">

    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> x
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxType">string</span><span class="SyntaxCode">: </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">default</span><span class="SyntaxCode">:     </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p>A <code class="incode">switch</code> can also be used with a (constant) range of values. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> success = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> x
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">:  </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">6.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">15</span><span class="SyntaxCode">: success = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(success)
}</code>
</div>
<p>If they overlap, the first valid range will be used. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> success = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> x
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">:  success = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">5.</span><span class="SyntaxCode">.&lt;</span><span class="SyntaxNumber">15</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(success)
}</code>
</div>
<p>A <code class="incode">case</code> expression doesn't need to be constant. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> test = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> test
    {
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> a:     </span><span class="SyntaxComment">// Test with a variable</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> b:     </span><span class="SyntaxComment">// Test with a variable</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> b + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">: </span><span class="SyntaxComment">// Test with an expression</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    }
}</code>
</div>

<h2 id="056_break">Break</h2><p>We have already seen than <code class="incode">break</code> is used to exit a <code class="incode">loop</code>, <code class="incode">visit</code>, <code class="incode">while</code>, <code class="incode">for</code>, <code class="incode">switch</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">for</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> i = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; i &lt; </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">; i += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">while</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
}</code>
</div>
<p>By default, <code class="incode">break</code> will exit the parent scope only. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// Exit the inner loop...</span><span class="SyntaxCode">
        }

        </span><span class="SyntaxComment">// ...and continue execution here</span><span class="SyntaxCode">
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
}</code>
</div>
<p>But you can <b>name a scope</b> with the <code class="incode">#scope</code> compiler keyword, and exit to the end of it with a <code class="incode">break</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Creates a scope named 'BigScope'</span><span class="SyntaxCode">
    #scope </span><span class="SyntaxConstant">BigScope</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        {
            cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">BigScope</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// Break to the outer scope...</span><span class="SyntaxCode">
        }

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxComment">// ...and continue execution here</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}</code>
</div>
<p>When used with a scope, a continue is a way to go back to the start of the scope. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    #scope </span><span class="SyntaxConstant">Loop</span><span class="SyntaxCode">
    {
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> cpt == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">continue</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// Loop</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
}</code>
</div>
<p>You are not obliged to name the scope, so this can also be used (for example) as an alternative of a bunch of if/else. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    #scope
    {
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> cpt == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
        }

        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> cpt == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
        }

        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> cpt == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
        }
    }
}</code>
</div>
<p>Note that a scope can be followed by a simple statement, not always a block. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    #scope </span><span class="SyntaxConstant">Up</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
                </span><span class="SyntaxLogic">break</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Up</span><span class="SyntaxCode">
        }

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    }
}</code>
</div>

<h2 id="060_struct">Struct</h2>
<h3 id="061_001_declaration">Declaration</h3><p>This is a <code class="incode">struct</code> declaration. <code class="incode">var</code> is not necessary for the fields. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    {
        name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">
    {
        x:      </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
        y, z:   </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
        val:    </span><span class="SyntaxType">bool</span><span class="SyntaxCode">
        myS:    </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    }
}</code>
</div>
<p>Variables can also be separated with <code class="incode">;</code> or <code class="incode">,</code>. The last one is not mandatory. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">{name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, val1: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">}
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">{x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">; y, z: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">; val: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">; myS: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">}
}</code>
</div>
<p>A struct can be anonymous when declared as a variable type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> tuple: </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    }
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> tuple1: </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">{x, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">}

    tuple.x = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">
    tuple.y = </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(tuple.x == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(tuple.y == </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">)
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    {
        rgb: </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">{x, y, z: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">}
        hsl: </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">{h, s, l: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">}
    }
}</code>
</div>
<p>The fields of a struct can be initialized at the declaration. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">    = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"454"</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v = </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">{}
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.x == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxString">"454"</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can initialize a struct variable in different ways. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// At fields declaration</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    {
        x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// Without parameters, all fields will have the default values as defined</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// in the struct itself.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v0: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v0.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v0.y == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// You can add parameters between {..}</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// The initialization must be done in the order of the fields.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v1: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">{</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">}
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v1.x == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v1.y == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// You can name fields, and omit some.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v2 = </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">{y = </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">}
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v2.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v2.y == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// You can assign a tuple as long as the types are correct.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v3: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode"> = {</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">}
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v3.x == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v3.y == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
}</code>
</div>
<p>A struct can be affected to a constant, as long as it can be evaluated at compile time. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">    = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"454"</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">X</span><span class="SyntaxCode">: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">{</span><span class="SyntaxNumber">50</span><span class="SyntaxCode">, </span><span class="SyntaxString">"value"</span><span class="SyntaxCode">}
    #assert </span><span class="SyntaxConstant">X</span><span class="SyntaxCode">.x == </span><span class="SyntaxNumber">50</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxConstant">X</span><span class="SyntaxCode">.y == </span><span class="SyntaxString">"value"</span><span class="SyntaxCode">
}</code>
</div>
<p>A function can take an argument of type <code class="incode">struct</code>. </p>
<p>No copy is done (this is equivalent to a const reference in C++). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Struct3</span><span class="SyntaxCode">
{
    x, y, z: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">(v: </span><span class="SyntaxConstant">Struct3</span><span class="SyntaxCode">)
{
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.x == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.z == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
}

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">titi</span><span class="SyntaxCode">(v: </span><span class="SyntaxConstant">Struct3</span><span class="SyntaxCode">)
{
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.x == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Call with a struct literal</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Struct3</span><span class="SyntaxCode">{</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">})

    </span><span class="SyntaxComment">// Type can be deduced from the argument</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">({</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">})

    </span><span class="SyntaxComment">// You can also just specify some parts of the struct, in the declaration order of the fields</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">titi</span><span class="SyntaxCode">({</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">})      </span><span class="SyntaxComment">// Initialize x, which is the first field</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">titi</span><span class="SyntaxCode">({</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">}) </span><span class="SyntaxComment">// Initialize x and y</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// You can also name the fields, and omit some of them</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">titi</span><span class="SyntaxCode">({x = </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, z = </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">}) </span><span class="SyntaxComment">// Here y will stay to the default value, which is 666</span><span class="SyntaxCode">
}</code>
</div>

<h3 id="062_002_impl">Impl</h3><p>Like for an enum, <code class="incode">impl</code> is used to declare some stuff in the scope of a struct. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.ExportType("methods")]</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// See later, used to export 'methods' in type reflection</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
{
    x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
    y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    z: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Declare a constant in the scope of the struct</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyConst</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Declare a function in the scope of the struct</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">returnTrue</span><span class="SyntaxCode">() =&gt; </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
}</code>
</div>
<p>So to access the constant and the function, you have to use the <code class="incode">MyStruct</code> namespace. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">MyConst</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">returnTrue</span><span class="SyntaxCode">())
}</code>
</div>
<p>You can have multiple <code class="incode">impl</code> blocks. The difference with a namespace is that <code class="incode">self</code> and <code class="incode">Self</code> are defined inside an <code class="incode">impl</code> block. They refere to the corresponding type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// 'self' is an alias for 'var self: Self'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">returnX</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self) =&gt; x
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">returnY</span><span class="SyntaxCode">(self)       =&gt; self.y

    </span><span class="SyntaxComment">// 'Self' is the corresponding type, in that case 'MyStruct'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">returnZ</span><span class="SyntaxCode">(me: </span><span class="SyntaxConstant">Self</span><span class="SyntaxCode">)   =&gt; me.z
}</code>
</div>
<p>If you declare your function with <code class="incode">mtd</code> (method) instead of <code class="incode">func</code>, then the first parameter is forced to be <code class="incode">using self</code>. </p>
<p>If you declare your function with <code class="incode">mtd const</code> (method const) instead of <code class="incode">func</code>, then the first parameter is forced to be <code class="incode">const using self</code>. Other than that, it's exactly the same. So this is just <b>syntaxic sugar</b> to avoid repeating the <code class="incode">using self</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode">  </span><span class="SyntaxFunction">methodReturnX</span><span class="SyntaxCode">()          =&gt; x
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">funcReturnX</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)  =&gt; x
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> c: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c.</span><span class="SyntaxFunction">returnX</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c.</span><span class="SyntaxFunction">methodReturnX</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c.</span><span class="SyntaxFunction">funcReturnX</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c.</span><span class="SyntaxFunction">returnY</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(c.</span><span class="SyntaxFunction">returnZ</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
}</code>
</div>
<p>All functions in an <code class="incode">impl</code> block can be retrieved by reflection, as long as the struct is declared with <code class="incode">#<a href="swag.runtime.html#Swag_ExportType("methods")">Swag.ExportType("methods")</a></code> (by default, methods are not exported). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Creates a type alias named 'Lambda'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Lambda</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> fnX: </span><span class="SyntaxConstant">Lambda</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> fnY: </span><span class="SyntaxConstant">Lambda</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> fnZ: </span><span class="SyntaxConstant">Lambda</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// The 'typeinfo' of a struct contains a field 'methods' which is a slice</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> t = </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">)
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> p: t.methods
    {
        </span><span class="SyntaxComment">// When visiting 'methods', the 'value' field contains the function pointer,</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// which can be casted to the correct type</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> p.name
        {
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxString">"returnX"</span><span class="SyntaxCode">: fnX = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Lambda</span><span class="SyntaxCode">) p.value
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxString">"returnY"</span><span class="SyntaxCode">: fnY = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Lambda</span><span class="SyntaxCode">) p.value
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxString">"returnZ"</span><span class="SyntaxCode">: fnZ = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Lambda</span><span class="SyntaxCode">) p.value
        }
    }

    </span><span class="SyntaxComment">// These are now valid functions, which can be called</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">fnX</span><span class="SyntaxCode">(v) == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">fnY</span><span class="SyntaxCode">(v) == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">fnZ</span><span class="SyntaxCode">(v) == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="063_003_special_functions">Special functions</h3><p>A struct can have special operations in the <code class="incode">impl</code> block. This operations are predefined, and known by the compiler.This is the way to go to <b>overload operators</b> for example. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">
{
    x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OneType</span><span class="SyntaxCode">      = </span><span class="SyntaxType">bool</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AnotherType</span><span class="SyntaxCode">  = </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WhateverType</span><span class="SyntaxCode"> = </span><span class="SyntaxType">bool</span><span class="SyntaxCode">

</span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Called each time a variable needs to be dropped</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// This is the destructor in C++</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opDrop</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self) {}

    </span><span class="SyntaxComment">// Called after a raw copy operation from one value to another</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opPostCopy</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self) {}

    </span><span class="SyntaxComment">// Called after a move semantic operation from one value to another. We'll see that later.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opPostMove</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self) {}

    </span><span class="SyntaxComment">// Get value by slice [low..up]</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Must return a string or a slice</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opSlice</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, low, up: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">string</span><span class="SyntaxCode"> { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxString">"true"</span><span class="SyntaxCode">; }

    </span><span class="SyntaxComment">// Get value by index</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opIndex</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, index: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">WhateverType</span><span class="SyntaxCode"> { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">; }

    </span><span class="SyntaxComment">// Called by @countof</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Use in a 'loop' block for example</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opCount</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">          { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; }
    </span><span class="SyntaxComment">// Called by @dataof</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opData</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;*</span><span class="SyntaxConstant">WhateverType</span><span class="SyntaxCode">  { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">; }

    </span><span class="SyntaxComment">// Called for explicit/implicit casting between struct value and return type</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Can be overloaded by a different return type</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Example: var x = cast(OneType) v</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opCast</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">OneType</span><span class="SyntaxCode">      { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">; }
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opCast</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">AnotherType</span><span class="SyntaxCode">  { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; }

    </span><span class="SyntaxComment">// Called to compare the struct value with something else</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Can be overloaded</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Returns true if they are equal, otherwise returns false</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Called by '==', '!='</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opEquals</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">OneType</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode">      { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">; }
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opEquals</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">AnotherType</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode">  { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">; }

    </span><span class="SyntaxComment">// Called to compare the struct value with something else</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Can be overloaded</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Returns -1, 0 or 1</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Called by '&lt;', '&gt;', '&lt;=', '&gt;=', '&lt;=&gt;'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opCmp</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">OneType</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">      { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; }
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opCmp</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">AnotherType</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">  { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; }

    </span><span class="SyntaxComment">// Affect struct with another value</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Can be overloaded</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Called by '='</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">OneType</span><span class="SyntaxCode">) {}
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">AnotherType</span><span class="SyntaxCode">) {}

    </span><span class="SyntaxComment">// Affect struct with a literal value with a specified suffix</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Generic function, can be overloaded</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Called by '='</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(suffix: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opAffectSuffix</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxConstant">OneType</span><span class="SyntaxCode">) {}
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(suffix: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opAffectSuffix</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxConstant">AnotherType</span><span class="SyntaxCode">) {}

    </span><span class="SyntaxComment">// Affect struct with another value at a given index</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Can be overloaded</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Called by '[] ='</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opIndexAffect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, index: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxConstant">OneType</span><span class="SyntaxCode">) {}
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opIndexAffect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, index: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxConstant">AnotherType</span><span class="SyntaxCode">) {}

    </span><span class="SyntaxComment">// Binary operator. 'op' generic argument contains the operator string</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Generic function, can be overloaded</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Called by '+', '-', '*', '/', '%', '|', '&', '^', '&lt;&lt;', '&gt;&gt;'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(op: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opBinary</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">OneType</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Self</span><span class="SyntaxCode">     { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> {</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">}; }
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(op: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opBinary</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">AnotherType</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Self</span><span class="SyntaxCode"> { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> {</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">}; }

    </span><span class="SyntaxComment">// Unary operator. 'op' generic argument contains the operator string (see below)</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Generic function</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Called by '!', '-', '~'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(op: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opUnary</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Self</span><span class="SyntaxCode"> { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> {</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">}; }

    </span><span class="SyntaxComment">// Affect operator. 'op' generic argument contains the operator string (see below)</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Generic function, can be overloaded</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '&lt;&lt;=', '&gt;&gt;='</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(op: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opAssign</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">OneType</span><span class="SyntaxCode">) {}
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(op: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opAssign</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">AnotherType</span><span class="SyntaxCode">)  {}

    </span><span class="SyntaxComment">// Affect operator. 'op' generic argument contains the operator string (see below)</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Generic function, can be overloaded</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '&lt;&lt;=', '&gt;&gt;='</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(op: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opIndexAssign</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, index: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxConstant">OneType</span><span class="SyntaxCode">) {}
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(op: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opIndexAssign</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, index: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxConstant">AnotherType</span><span class="SyntaxCode">) {}

    </span><span class="SyntaxComment">// Called by a 'visit' block</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Can have multiple versions, by adding a name after 'opVisit'</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// This is the way to go for iterators</span><span class="SyntaxCode">
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(ptr: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opVisit</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, stmt: </span><span class="SyntaxType">code</span><span class="SyntaxCode">) {}
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(ptr: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opVisitWhatever</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, stmt: </span><span class="SyntaxType">code</span><span class="SyntaxCode">) {}
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(ptr: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opVisitAnother</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, stmt: </span><span class="SyntaxType">code</span><span class="SyntaxCode">) {}
    }
}</code>
</div>

<h3 id="064_004_affectation">Affectation</h3><p><code class="incode">opAffect</code> is a way of assigning to a struct with <code class="incode">=</code>. </p>
<p>You can have more the one <code class="incode">opAffect</code> with different types. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">
{
    x, y, z: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)  { x, y = value; }
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) { x, y = value ? </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> : </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">; }
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// This will initialize v and then call opAffect(s32) with '4'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v: </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.x == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxComment">// Note that variable 'v' is also initiliazed with the default values.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// So here 'z' is still 666 because 'opAffect' does not change it.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.z == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// This will call opAffect(bool) with 'true'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v1: </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v1.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v1.y == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// This will call opAffect(bool) with 'false'</span><span class="SyntaxCode">
    v1 = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v1.x == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v1.y == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>If <code class="incode">opAffect</code> is supposed to initialize the full content of the struct, you can add <code class="incode">#<a href="swag.runtime.html#Swag_Complete">Swag.Complete</a></code>. This will avoid every variables to be initialized to the default values, then changed later with the <code class="incode">opAffect</code> call. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Complete]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)  { x, y, z = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) value; }

    </span><span class="SyntaxComment">// For later</span><span class="SyntaxCode">
    </span><span class="SyntaxAttribute">#[Swag.Implicit]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)  { x, y = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) value; }
}</code>
</div>
<p>Here the variable <code class="incode">v</code> will not be initialized prior to the affectation. This is more optimal, as there's only one initialization. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v: </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">'</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.x == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.z == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">(v: </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.x == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.z == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">titi</span><span class="SyntaxCode">(v: </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxComment">// By default, there's no automatic conversion for a function argument, so you must cast.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Here, this will create a 'Struct' on the stack, initialized with a call to 'opAffect(s32)'</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">) </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// But if opAffect is marked with #[Swag.Implicit], automatic conversion can occur.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// No need for an explicit cast.</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">'</span><span class="SyntaxType">u16</span><span class="SyntaxCode">)
}</code>
</div>
<p>A structure can be converted to a constant by a call to <code class="incode">opAffect</code> if the function is marked with <code class="incode">Swag.ConstExpr</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Vector2</span><span class="SyntaxCode">
{
    x, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Vector2</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(one: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)
    {
        x, y = one
    }
}

</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">One</span><span class="SyntaxCode">: </span><span class="SyntaxConstant">Vector2</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// This will call opAffect(1.0) at compile time</span><span class="SyntaxCode">
#assert </span><span class="SyntaxConstant">One</span><span class="SyntaxCode">.x == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">
#assert </span><span class="SyntaxConstant">One</span><span class="SyntaxCode">.y == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode"></code>
</div>

<h3 id="064_005_count">Count</h3><div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
{
}

</span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opCount</span><span class="SyntaxCode">() =&gt; </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">'</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
}</code>
</div>
<p>You can loop on a struct value if <code class="incode">opCount</code> has been defined. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v = </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">{}

    </span><span class="SyntaxComment">// '@countof' will call 'opCount'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(v) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// You can then loop on a struct value if 'opCount' has been defined</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> v
        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="064_006_post_copy_and_post_move">Post copy and post move</h3><p>Swag accepts copy and move semantics for structures. In this examples, we use a <code class="incode">Vector3</code> to illustrate, even if of course that kind of struct does not need a move semantic, as there's no heap involved. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">
{
    x, y, z: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// This is used for 'copy semantic'.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opPostCopy</span><span class="SyntaxCode">()
    {
        x, y, z += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// This is used for 'move semantic'.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opPostMove</span><span class="SyntaxCode">()
    {
        x, y, z += </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// Just imagine that we have something to drop. Like a memory allocated buffer.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opDrop</span><span class="SyntaxCode">()
    {
    }
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">{}
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> b = </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">{</span><span class="SyntaxNumber">100</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">200</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">300</span><span class="SyntaxCode">}

    </span><span class="SyntaxComment">// "copy semantic". The default behaviour.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 1. This will call 'opDrop' on 'a' if the function exists because 'a' could already be assigned.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 2. This will raw copy 'b' to 'a'.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 3. This will call 'opPostCopy' on 'a' if it exists.</span><span class="SyntaxCode">
    a = b
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a.x == </span><span class="SyntaxNumber">101</span><span class="SyntaxCode">)     </span><span class="SyntaxComment">// +1 because of the call to opPostCopy</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a.y == </span><span class="SyntaxNumber">201</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a.z == </span><span class="SyntaxNumber">301</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// "move semantic" by adding the modifier ',move' just after '='.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 1. This will call 'opDrop' on 'a' if it exists</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 2. This will raw copy 'b' to 'a'</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 3. This will call 'opPostMove' on 'a' if it exists</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 4. This will reinitialize 'b' to the default values if 'opDrop' exists</span><span class="SyntaxCode">
    a =,move b
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a.x == </span><span class="SyntaxNumber">102</span><span class="SyntaxCode">)     </span><span class="SyntaxComment">// +2 because of the call to opPostMove</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a.y == </span><span class="SyntaxNumber">202</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a.z == </span><span class="SyntaxNumber">302</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// 'Vector3' contains an 'opDrop' special function, so 'b' will be reinitialized to</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// the default values after the 'move'. Default values are 666.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b.x == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// If you know what you're doing, you can avoid the first call to 'opDrop' with '=,nodrop'</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Do it when you know the state of 'a' and do not want an extra unnecessary call</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// (for example if 'a' is in an undetermined state).</span><span class="SyntaxCode">

    a =,nodrop b            </span><span class="SyntaxComment">// Copy b to a without dropping 'a' first</span><span class="SyntaxCode">
    a =,nodrop,move b       </span><span class="SyntaxComment">// Move b to a without dropping 'a' first</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// For the 'move semantic', you can avoid the last reinitialization by using '=,moveraw'. Of course, do this at your own risk, if you know that 'b' will never by dropped by the compiler or if you reinitialize its state yourself.</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// instead of '=,move'</span><span class="SyntaxCode">
    a =,moveraw b
    a =,nodrop,moveraw b
}</code>
</div>
<h4>moveref </h4>
<p><code class="incode">moveref</code> can be used instead of <code class="incode">ref</code> in a function parameter to declare a <b>move semantic</b> intention. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// This is the 'move' version of 'assign'. With 'moveref', we tell the compiler that this version will take the owership on 'from'.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">assign</span><span class="SyntaxCode">(to: </span><span class="SyntaxKeyword">ref</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">, from: </span><span class="SyntaxKeyword">moveref</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">)
    {
        to =,move from
    }

    </span><span class="SyntaxComment">// This is the normal 'copy' version. In this version, 'from' will not be changed, that's why it's constant (not a ref).</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">assign</span><span class="SyntaxCode">(to: </span><span class="SyntaxKeyword">ref</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">, from: </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">)
    {
        to = from
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">{</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">}
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> b: </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Call the 'copy' version of 'assign'.</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">assign</span><span class="SyntaxCode">(&b, a)
    </span><span class="SyntaxComment">// As this will call 'opPostCopy', we have +1 on each field.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b.x == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> b.y == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> b.z == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxComment">// 'a' remains unchanged</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> a.y == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> a.z == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Now we tell the compiler to use the 'moveref' version of 'assign'.</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">assign</span><span class="SyntaxCode">(&b, </span><span class="SyntaxKeyword">moveref</span><span class="SyntaxCode"> &a)
    </span><span class="SyntaxComment">// As 'opPostMove' has been called, we have +2 on each field.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b.x == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> b.y == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> b.z == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxComment">// And as this is a move, then 'a' is now reinitialized to its default values.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a.x == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> a.y == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> a.z == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="064_007_visit">Visit</h3><div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
{
    x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">
    z: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">
}</code>
</div>
<p>You can visit a struct variable if a macro <code class="incode">opVisit</code> has been defined. This is the equivalent of an <b>iterator</b>. </p>
<p><code class="incode">opVisit</code> is a macro, so it should be marked with the <code class="incode">#<a href="swag.runtime.html#Swag_Macro">Swag.Macro</a></code> attribute. <code class="incode">opVisit</code> is also a generic function which takes a constant generic parameter of type <code class="incode">bool</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(ptr: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opVisit</span><span class="SyntaxCode">(self, stmt: </span><span class="SyntaxType">code</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxComment">// 'ptr' is a generic parameter that tells if we want to visit by pointer or by value.</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// We do not use it in this example, so we check at compile time that it's not true.</span><span class="SyntaxCode">
        #if ptr #error </span><span class="SyntaxString">"visiting myStruct by pointer is not supported"</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// Loop on the 3 fields</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> idx: </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxComment">// The '#macro' keyword forces its body to be in the scope of the caller</span><span class="SyntaxCode">
            #macro
            {
                </span><span class="SyntaxComment">// @alias0 will be the value</span><span class="SyntaxCode">
                </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">undefined</span><span class="SyntaxCode">

                </span><span class="SyntaxComment">// As this code is in the caller scope, with need to add a '#up' before 'idx' to</span><span class="SyntaxCode">
                </span><span class="SyntaxComment">// reference the variable of this function (and not a potential variable in</span><span class="SyntaxCode">
                </span><span class="SyntaxComment">// the caller scope)</span><span class="SyntaxCode">
                </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> #up idx
                {
                </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> = #up self.x   </span><span class="SyntaxComment">// Same for function parameter 'self'</span><span class="SyntaxCode">
                </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> = #up self.y
                </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> = #up self.z
                }

                </span><span class="SyntaxComment">// @alias1 will be the index</span><span class="SyntaxCode">
                </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@alias1</span><span class="SyntaxCode"> = </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">

                </span><span class="SyntaxComment">// include user code</span><span class="SyntaxCode">
                #mixin #up stmt
            }
        }
    }
}</code>
</div>
<p>So now that the <code class="incode">opVisit</code> has been defined, we can <code class="incode">visit</code> it. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> myStruct = </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">{}
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Visiting each field in declaration order</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 'v' is an alias for @alias0 (value)</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 'i' is an alias for @alias1 (index)</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> v, i: myStruct
    {
        </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> i
        {
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
        }

        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can have variants of <code class="incode">opVisit</code> by specifying an <b>additional name</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode">(ptr: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opVisitReverse</span><span class="SyntaxCode">(stmt: </span><span class="SyntaxType">code</span><span class="SyntaxCode">)   </span><span class="SyntaxComment">// We add 'Reverse' in the name</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxComment">// Visit fields in reverse order (z, y then x)</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> idx: </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">
        {
            #macro
            {
                </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">undefined</span><span class="SyntaxCode">
                </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> #up idx
                {
                </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> = #up self.z
                </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> = #up self.y
                </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> = #up self.x
                }

                </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@alias1</span><span class="SyntaxCode"> = </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">
                #mixin #up stmt
            }
        }
    }
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> myStruct = </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">{}
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// To call a variant, add the extra name between parenthesis.</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Reverse</span><span class="SyntaxCode">) v, i: myStruct
    {
        </span><span class="SyntaxLogic">switch</span><span class="SyntaxCode"> i
        {
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">case</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">: </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
        }

        cpt += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="067_008_offset">Offset</h3><p>You can force the layout of a field with the <code class="incode">Swag.Offset</code> attribute. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// 'y' is located at the same offset as 'x', so they share the same space</span><span class="SyntaxCode">
        </span><span class="SyntaxAttribute">#[Swag.Offset("x")]</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// They are 2 fields in the struct but stored in only one s32, so the total size is 4 bytes.</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v = </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">{}
    v.x = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// As 'y' and 'x' are sharing the same space, affecting to 'x' also affects to 'y'.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
}</code>
</div>
<p>An example to reference a field by index. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    {
        x, y, z:    </span><span class="SyntaxType">f32</span><span class="SyntaxCode">

        </span><span class="SyntaxAttribute">#[Swag.Offset("x")]</span><span class="SyntaxCode">
        idx:        [</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">] </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    v.x = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">; v.y = </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">; v.z = </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.idx[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == v.x)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.idx[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == v.y)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.idx[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] == v.z)
}</code>
</div>

<h3 id="068_009_packing">Packing</h3><p>You can also control the struct layout with two attributes: <code class="incode">#<a href="swag.runtime.html#Swag_Pack">Swag.Pack</a></code> and <code class="incode">#<a href="swag.runtime.html#Swag_Align">Swag.Align</a></code>. </p>
<p>The default struct packing is the same as in C: each field is aligned to the size of the type. This is the equivalent of <code class="incode">#<a href="swag.runtime.html#Swag_Pack(0)">Swag.Pack(0)</a></code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// offset 0: aligned to 1 byte</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">      </span><span class="SyntaxComment">// offset 4: s32 is aligned to 4 bytes (so here there's 3 bytes of padding before)</span><span class="SyntaxCode">
        z: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">      </span><span class="SyntaxComment">// offset 8: aligned to 8 bytes</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// '@offsetof' can be used to retrieve the offset of a field</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">.x) == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">.y) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">.z) == </span><span class="SyntaxNumber">8</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">)     == </span><span class="SyntaxNumber">16</span><span class="SyntaxCode">
}</code>
</div>
<p>You can <i>reduce</i> the packing of the fields with <code class="incode">#<a href="swag.runtime.html#Swag_Pack">Swag.Pack</a></code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Pack(1)]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// offset 0: 1 byte</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// offset 1: 4 bytes (no padding)</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">.x) == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">.y) == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">)     == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">

    </span><span class="SyntaxAttribute">#[Swag.Pack(2)]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct2</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// offset 0: 1 byte</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// offset 2: 4 bytes (1 byte of padding before)</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct2</span><span class="SyntaxCode">.x) == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct2</span><span class="SyntaxCode">.y) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct2</span><span class="SyntaxCode">)     == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">

    </span><span class="SyntaxAttribute">#[Swag.Pack(4)]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct3</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// offset 0: 1 byte</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// offset 4: 8 bytes (3 bytes of padding before)</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct3</span><span class="SyntaxCode">.x) == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct3</span><span class="SyntaxCode">.y) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct3</span><span class="SyntaxCode">)     == </span><span class="SyntaxNumber">12</span><span class="SyntaxCode">
}</code>
</div>
<p>The total struct size is always a multiple of the biggest alignement of the fields. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// 4 bytes</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// 1 byte</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// 3 bytes of padding because of 'x', to aligned on 's32'</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">8</span><span class="SyntaxCode">
}</code>
</div>
<p>You can force the struct alignement with <code class="incode">#<a href="swag.runtime.html#Swag_Align">Swag.Align</a></code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// 1 byte</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// 1 byte</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">.x) == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">.y) == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">)     == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">

    </span><span class="SyntaxAttribute">#[Swag.Align(8)]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct2</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// 1 byte</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// 1 byte</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// 6 bytes of padding to be a multiple of '8'</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct2</span><span class="SyntaxCode">.x) == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct2</span><span class="SyntaxCode">.y) == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct2</span><span class="SyntaxCode">)     == </span><span class="SyntaxNumber">8</span><span class="SyntaxCode">
}</code>
</div>
<p>You can also force each field to be aligned on a specific value. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// offset 0: 1 byte</span><span class="SyntaxCode">
        </span><span class="SyntaxAttribute">#[Swag.Align(8)]</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// offset 8: aligned to 8 (7 bytes of padding before)</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct1</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">9</span><span class="SyntaxCode">

    </span><span class="SyntaxAttribute">#[Swag.Align(8)]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct2</span><span class="SyntaxCode">
    {
        x: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// offset 0: 1 byte</span><span class="SyntaxCode">
        </span><span class="SyntaxAttribute">#[Swag.Align(4)]</span><span class="SyntaxCode">
        y: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// offset 4: aligned to 4 (3 bytes of padding before)</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// 3 bytes of padding to be a multiple of 8</span><span class="SyntaxCode">
    }

    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyStruct2</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">8</span><span class="SyntaxCode">
}</code>
</div>

<h2 id="070_union">Union</h2><p>An union is just a struct where all fields are located at offset 0. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxKeyword">union</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyUnion</span><span class="SyntaxCode">
        {
            x, y, z: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
        }

        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v = </span><span class="SyntaxConstant">MyUnion</span><span class="SyntaxCode">{x = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">}
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.z == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
    }
}</code>
</div>

<h2 id="075_interface">Interface</h2><p>Interfaces are <b>virtual tables</b> (a list of function pointers) that can be associated to a struct. </p>
<p>Unlike C++, the virtual table is not embedded with the struct. It is a separate object. You can then <i>implement</i> an interface for a given struct without changing the struct definition. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point2</span><span class="SyntaxCode">
{
    x, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point3</span><span class="SyntaxCode">
{
    x, y, z: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
}</code>
</div>
<p>Here we declare an interface, with two functions <code class="incode">set</code> and <code class="incode">reset</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">interface</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// The first parameter must be 'self'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(self, val: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// You can also use the 'mtd' declaration to avoid specifying the 'self' yourself</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">reset</span><span class="SyntaxCode">()
}</code>
</div>
<p>You can implement an interface for any given struct with <code class="incode">impl</code> and <code class="incode">for</code>. </p>
<p>For example here, we implement interface <code class="incode">IReset</code> for struct <code class="incode">Point2</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">for</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point2</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// You must add 'impl' to indicate that you want to implement a function of the interface.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(val: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)
    {
        x = val
        y = val+</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// Don't forget that 'mtd' is just syntaxic sugar. 'func' still works.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">reset</span><span class="SyntaxCode">(self)
    {
        self.x, self.y = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// Not that you can also declare 'normal' functions or methods in an 'impl block'.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myOtherMethod</span><span class="SyntaxCode">()
    {
    }
}</code>
</div>
<p>And we implement interface <code class="incode">IReset</code> also for struct <code class="incode">Point3</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">for</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point3</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(val: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)
    {
        x = val
        y = val+</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        z = val+</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">reset</span><span class="SyntaxCode">()
    {
        x, y, z = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    }
}</code>
</div>
<p>We can then use these interfaces on either <code class="incode">Vector2</code> or <code class="incode">Vector3</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt2: </span><span class="SyntaxConstant">Point2</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt3: </span><span class="SyntaxConstant">Point3</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// To get the interface associated to a given struct, use the 'cast' operator.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// If the compiler does not find the corresponding implementation, it will raise an error.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> itf = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">) pt2
    itf.</span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt2.x == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt2.y == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

    itf = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">) pt3
    itf.</span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt3.x == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt3.y == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt3.z == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    itf.</span><span class="SyntaxFunction">reset</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt3.x == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> pt3.y == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can also access, with a normal call, all functions declared in an interface implementation block for a given struct. </p>
<p>They are located in a dedicated scope. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt2: </span><span class="SyntaxConstant">Point2</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt3: </span><span class="SyntaxConstant">Point3</span><span class="SyntaxCode">

    pt2.</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    pt2.</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">reset</span><span class="SyntaxCode">()
    pt3.</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    pt3.</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">reset</span><span class="SyntaxCode">()
}</code>
</div>
<p>An interface is a real type, with a size equivalent to 2 pointers. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt2: </span><span class="SyntaxConstant">Point2</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt3: </span><span class="SyntaxConstant">Point3</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> itf = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">) pt2
    #assert </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(itf) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> * </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(*</span><span class="SyntaxType">void</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// You can retrieve the concrete type associated with an interface instance with '@kindof'.</span><span class="SyntaxCode">
    itf = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">) pt2
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(itf) == </span><span class="SyntaxConstant">Point2</span><span class="SyntaxCode">)
    itf = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">) pt3
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(itf) == </span><span class="SyntaxConstant">Point3</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// You can retrieve the concrete data associated with an interface instance with '@dataof'</span><span class="SyntaxCode">
    itf = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">) pt2
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@dataof</span><span class="SyntaxCode">(itf) == &pt2)
    itf = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">IReset</span><span class="SyntaxCode">) pt3
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@dataof</span><span class="SyntaxCode">(itf) == &pt3)
}</code>
</div>

<h2 id="100_function">Function</h2>
<h3 id="101_001_declaration">Declaration</h3><p>A function declaration usually starts with the <code class="incode">func</code> keyword followed by the function name. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">()
{
}</code>
</div>
<p>If the function needs to return a value, you must add <code class="incode">-&gt;</code> followed by the type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toto1</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
{
    </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
}</code>
</div>
<p>The return type can be deduced in case of a simple expression, by using <code class="incode">=&gt;</code> instead of <code class="incode">-&gt;</code>. </p>
<p>Here the return type will be deduced to be <code class="incode">s32</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x + y</code>
</div>
<p>A short version exists, in case of a function returning nothing. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">print</span><span class="SyntaxCode">(val: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) = </span><span class="SyntaxIntrinsic">@print</span><span class="SyntaxCode">(val)</code>
</div>
<p>Parameters are specified after the function name, between parenthesis. </p>
<p>Here we declare two parameters <code class="incode">x</code> and <code class="incode">y</code> of type <code class="incode">s32</code>, and one last parameter of type <code class="incode">f32</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum1</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, unused: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
{
    </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> x + y
}</code>
</div>
<p>Parameters can have default values. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum2</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, unused: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
{
    </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> x + y
}</code>
</div>
<p>The type of the parameters can be deduced if you specify a default value. </p>
<p>Here <code class="incode">x</code> and <code class="incode">y</code> have the type <code class="incode">f32</code> because <code class="incode">0.0</code> is a 32 bits floating point literal. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum3</span><span class="SyntaxCode">(x, y = </span><span class="SyntaxNumber">0.0</span><span class="SyntaxCode">)
{
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(x) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(y) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">; }
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y = </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">A</span><span class="SyntaxCode">)
{
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(y) == </span><span class="SyntaxConstant">Values</span><span class="SyntaxCode">
}</code>
</div>
<p>Functions can be nested inside other functions. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// This are not closure but just functions in a sub scope.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sub</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x - y

    </span><span class="SyntaxComment">// Simple call.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxFunction">sub</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can name parameters, and don't have to respect parameters order in that case. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sub</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x - y

    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x1 = </span><span class="SyntaxFunction">sub</span><span class="SyntaxCode">(x = </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, y = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x1 == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x2 = </span><span class="SyntaxFunction">sub</span><span class="SyntaxCode">(y = </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, x = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x2 == -</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
    }

    {
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">returnMe</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">) =&gt; x + y * </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">returnMe</span><span class="SyntaxCode">(x = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">returnMe</span><span class="SyntaxCode">(y = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
    }
}</code>
</div>
<h4>Multiple return values </h4>
<p>An <b>anonymous struct</b> can be used to return multiple values in a function. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunction</span><span class="SyntaxCode">() -&gt; {x, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">}
    {
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> {</span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">}
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result = </span><span class="SyntaxFunction">myFunction</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(result.item0 == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(result.item1 == </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> (x, y) = </span><span class="SyntaxFunction">myFunction</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> (z, w) = </span><span class="SyntaxFunction">myFunction</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(z == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(w == </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">)
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">returns2</span><span class="SyntaxCode">() -&gt; {x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">}
    {
        </span><span class="SyntaxComment">// You can return a tuple literal as long as the types match</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> {</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">}

        </span><span class="SyntaxComment">// Or use the specifal type 'retval' which is an typealias to the</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// function return type (but not only, we'll see later)</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result: </span><span class="SyntaxKeyword">retval</span><span class="SyntaxCode">
        x = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        y = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> result
    }

    </span><span class="SyntaxComment">// You can access the struct fields with the names or with 'item?'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result = </span><span class="SyntaxFunction">returns2</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(result.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(result.item0 == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(result.y == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(result.item1 == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// You can deconstruct the returned struct</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> (x, y) = </span><span class="SyntaxFunction">returns2</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="102_002_lambda">Lambda</h3><p>A lambda is just a <b>pointer to a function</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunction0</span><span class="SyntaxCode">() {}
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunction1</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x * x

    </span><span class="SyntaxComment">// 'ptr0' is declared as a pointer to a function that takes no parameter, and returns nothing</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr0: </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">() = &myFunction0
    </span><span class="SyntaxFunction">ptr0</span><span class="SyntaxCode">()

    </span><span class="SyntaxComment">// Here type of 'ptr1' is deduced from 'myFunction1'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr1 = &myFunction1
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunction1</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">ptr1</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}</code>
</div>
<p>A lambda can be null. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> lambda: </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(lambda == </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can use a lambda as a function parameter type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> callback = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toDo</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, ptr: callback)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> =&gt; </span><span class="SyntaxFunction">ptr</span><span class="SyntaxCode">(value)

    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">square</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x * x
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toDo</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, &square) == </span><span class="SyntaxNumber">16</span><span class="SyntaxCode">)
}</code>
</div>
<h4>Anonymous functions </h4>
<p>You can also create <i>anonymous functions</i> (aka functions as literals). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cb = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> =&gt; x * x
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">cb</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">16</span><span class="SyntaxCode">)
    cb = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> =&gt; x * x * x
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">cb</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">64</span><span class="SyntaxCode">)
}</code>
</div>
<p>Anonymous functions can be passed as parameters to another function. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> callback = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toDo</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, ptr: callback)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> =&gt; </span><span class="SyntaxFunction">ptr</span><span class="SyntaxCode">(value)

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toDo</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x * x) == </span><span class="SyntaxNumber">16</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toDo</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x + x) == </span><span class="SyntaxNumber">8</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toDo</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> x - x; }) == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>The types of the parameters can be deduced. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> callback = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toDo</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, ptr: callback)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> =&gt; </span><span class="SyntaxFunction">ptr</span><span class="SyntaxCode">(value)

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toDo</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x) =&gt; x * x) == </span><span class="SyntaxNumber">16</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toDo</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x) =&gt; x + x) == </span><span class="SyntaxNumber">8</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toDo</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x) { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> x - x; }) == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>When you affect a lambda to a variable, the type of parameters and the return type can also be omitted, as they will be deduced from the variable type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> fct: </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode">

    fct = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x, y) =&gt; x == y
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">))

    fct = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x, y) { </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> x != y; }
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">120</span><span class="SyntaxCode">))
}</code>
</div>
<p>Lambdas can have default parameters values. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(val = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">) {
            </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(val == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
        }

        </span><span class="SyntaxFunction">x</span><span class="SyntaxCode">()
    }

    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(val: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)

        x = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(val) {
            </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(val == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
        }

        </span><span class="SyntaxFunction">x</span><span class="SyntaxCode">()
        </span><span class="SyntaxFunction">x</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    }
}</code>
</div>

<h3 id="103_003_closure">Closure</h3><p>Swag supports a limited set of the <code class="incode">closure</code> concept. </p>
<p>Only a given amount of bytes of capture are possible (for now 48 bytes). That way there's never an hidden allocation. Another limitation is that you can only capture <code class="incode">simple</code> variables (no struct with <code class="incode">opDrop</code>, <code class="incode">opPostCopy</code>, <code class="incode">opPostMove</code> for example). </p>
<p>A closure is declared like a lambda, with the captured variables between <code class="incode">|...|</code> before the function parameters. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">125</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b = </span><span class="SyntaxNumber">521</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Capture 'a' and 'b'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> fct: </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">() = </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">|a, b|()
    {
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">125</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">521</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">()
}</code>
</div>
<p>You can also capture by pointer with <code class="incode">&</code> (otherwise it's a copy). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">125</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Capture 'a' by pointer</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> fct: </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">() = </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">|&a|()
    {
        </span><span class="SyntaxComment">// We can change the value of the local variable 'a'</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">dref</span><span class="SyntaxCode"> a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">126</span><span class="SyntaxCode">)
    </span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">127</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can also capture by reference with <code class="incode">ref</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">125</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Capture 'a' by reference</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> fct = </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">|</span><span class="SyntaxKeyword">ref</span><span class="SyntaxCode"> a|()
    {
        </span><span class="SyntaxComment">// We can change the value of the local variable 'a'</span><span class="SyntaxCode">
        a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">126</span><span class="SyntaxCode">)
    </span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">127</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can assign a normal lambda (no capture) to a closure variable. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> fct: </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">

    fct = </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(x, y) =&gt; x + y
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can capture arrays, structs, slices etc... as long as it fits in the maximum storage of <code class="incode">n</code> bytes (and as long as the struct is a pod). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">]

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> fct: </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Capture the array 'x' by making a copy</span><span class="SyntaxCode">
    fct = </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">|x|(toAdd)
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> res = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> v: x
            res += v
        res += toAdd
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> res
    }

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> result = </span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(result == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">3</span><span class="SyntaxCode"> + </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}</code>
</div>
<p>Captured variables are mutable, and part of the closure. So you can modify them. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getInc</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">

        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">|x|()-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> {
            x += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> x
        }
    }

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> fct = </span><span class="SyntaxFunction">getInc</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">11</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">12</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">fct</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">13</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="104_004_mixin">Mixin</h3><p>A mixin is declared like a function, with the attribute <code class="incode">#<a href="swag.runtime.html#Swag_Mixin">Swag.Mixin</a></code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Mixin]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myMixin</span><span class="SyntaxCode">()
    {
    }
}</code>
</div>
<p>A mixin function is inserted in the scope of the caller. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Mixin]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myMixin</span><span class="SyntaxCode">()
    {
        a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">myMixin</span><span class="SyntaxCode">()   </span><span class="SyntaxComment">// Equivalent to 'a += 1'</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">myMixin</span><span class="SyntaxCode">()   </span><span class="SyntaxComment">// Equivalent to 'a += 1'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>This behaves like a function, so you can add parameters. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Mixin]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myMixin</span><span class="SyntaxCode">(increment: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    {
        a += increment
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">myMixin</span><span class="SyntaxCode">()   </span><span class="SyntaxComment">// Equivalent to 'a += 1'</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">myMixin</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)  </span><span class="SyntaxComment">// Equivalent to 'a += 2'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p>A mixin accepts parameters of type <code class="incode">code</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Mixin]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">doItTwice</span><span class="SyntaxCode">(what: </span><span class="SyntaxType">code</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxComment">// You can then insert the code with '#mixin'</span><span class="SyntaxCode">
        #mixin what
        #mixin what
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">doItTwice</span><span class="SyntaxCode">(#code {a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">;})
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>When the last parameter of a mixin is of type <code class="incode">code</code>, the caller can declare that code in a statement just after the call. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Mixin]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">doItTwice</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, what: </span><span class="SyntaxType">code</span><span class="SyntaxCode">)
    {
        #mixin what
        #mixin what
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">doItTwice</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    {
        a += value
    }

    </span><span class="SyntaxFunction">doItTwice</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    {
        a += value
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">12</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can use the special name <code class="incode">@alias</code> to create a named alias for an identifier. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Mixin]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">inc10</span><span class="SyntaxCode">()
    {
        </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> += </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a, b = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">inc10</span><span class="SyntaxCode">(|a|)  </span><span class="SyntaxComment">// Passing alias name 'a'</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">inc10</span><span class="SyntaxCode">(|b|)  </span><span class="SyntaxComment">// Passing alias name 'b'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == b </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> b == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Mixin]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setVar</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> = value
    }

    </span><span class="SyntaxFunction">setVar</span><span class="SyntaxCode">(|a| </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)  </span><span class="SyntaxComment">// Passing alias name 'a'</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">setVar</span><span class="SyntaxCode">(|b| </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)  </span><span class="SyntaxComment">// Passing alias name 'b'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
    </span><span class="SyntaxFunction">setVar</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)      </span><span class="SyntaxComment">// No typealias, so name is @alias0</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can declare special variables named <code class="incode">@mixin?</code>. Those variables will have a unique name each time the mixin is used. So the same mixin, even if it declares local variables, can be used multiple time in the same scope. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> total: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">

    </span><span class="SyntaxAttribute">#[Swag.Mixin]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toScope</span><span class="SyntaxCode">()
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@mixin0</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        total += </span><span class="SyntaxIntrinsic">@mixin0</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxFunction">toScope</span><span class="SyntaxCode">()
    </span><span class="SyntaxFunction">toScope</span><span class="SyntaxCode">()
    </span><span class="SyntaxFunction">toScope</span><span class="SyntaxCode">()

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(total == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="105_005_macro">Macro</h3><p>A macro, like a mixin, is declared like a function, but with the attribute <code class="incode">Swag.Macro</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">()
    {
    }
}</code>
</div>
<p>Unlike a mixin, a macro has its own scope, and cannot conflict with the function it is inserted inside. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">()
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// 'a' is declared in the scope of 'myMacro'</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">()   </span><span class="SyntaxComment">// no conflict with the 'a' declared above</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>But you can force an identifier to be found <b>outside</b> of the scope of the macro with <code class="incode">#up</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">()
    {
        #up a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// Add '#up' before the identifier to reference the 'a' of the caller</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">()   </span><span class="SyntaxComment">// This will change the 'a' above</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">()   </span><span class="SyntaxComment">// This will change the 'a' above</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>Like a mixin, macro accepts <code class="incode">code</code> parameters. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">(what: </span><span class="SyntaxType">code</span><span class="SyntaxCode">)
    {
        #mixin what
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

    </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">(#code { #up a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">; } )

    </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">()
    {
        #up a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can use <code class="incode">#macro</code> inside a macro to force the code after to be in the same scope of the caller. That is, no <code class="incode">#up</code> is necessary to reference variables of the caller. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">(what: </span><span class="SyntaxType">code</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxComment">// No conflict, in its own scope</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// Isolate the caller code, to avoid conflicts with the macro internals</span><span class="SyntaxCode">
        #macro
        {
            </span><span class="SyntaxComment">// In the scope of the caller</span><span class="SyntaxCode">
            #mixin #up what
        }
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">myMacro</span><span class="SyntaxCode">()
    {
        a += </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">  </span><span class="SyntaxComment">// will reference the 'a' above because this code has been inserted inside '#macro'</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can extend the language with macros, without using pointers to functions (no lambda call cost). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">repeat</span><span class="SyntaxCode">(count: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, what: </span><span class="SyntaxType">code</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">while</span><span class="SyntaxCode"> a &lt; count
        {
            #macro
            {
                </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> index = #up a
                #mixin #up what
            }

            a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        }
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">repeat</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">) { a += index; }
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxFunction">repeat</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">) { a += index; }
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p>When you need <code class="incode">break</code> in the user code to break outside of a multi loop. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">repeatSquare</span><span class="SyntaxCode">(count: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, what: </span><span class="SyntaxType">code</span><span class="SyntaxCode">)
    {
        #scope </span><span class="SyntaxConstant">Up</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> count
        {
            </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> count
            {
                #macro
                {
                    </span><span class="SyntaxComment">// 'break' in the user code will be replaced by 'break Up'</span><span class="SyntaxCode">
                    </span><span class="SyntaxComment">// So it will break outside the outer loop</span><span class="SyntaxCode">
                    #mixin #up what { </span><span class="SyntaxLogic">break</span><span class="SyntaxCode"> = </span><span class="SyntaxLogic">break</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Up</span><span class="SyntaxCode">; }
                }
            }
        }
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">repeatSquare</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    {
        a += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> a == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(a == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
}</code>
</div>
<p>In a macro, you can use special variables named <code class="incode">@alias&lt;num&gt;</code>. Note that this is also valid for mixins. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Macro]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">call</span><span class="SyntaxCode">(v: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, stmt: </span><span class="SyntaxType">code</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> = v
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@alias1</span><span class="SyntaxCode"> = v * </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
        #mixin stmt
    }

    </span><span class="SyntaxFunction">call</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@alias0</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@alias1</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxComment">// The caller can then name those special variables</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Use |name0, name1, ...| before the function call parameters</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">call</span><span class="SyntaxCode">(|x| </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)        </span><span class="SyntaxComment">// x is @alias0</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@alias1</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)  </span><span class="SyntaxComment">// @alias1 is not renamed</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxFunction">call</span><span class="SyntaxCode">(|x, y| </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)   </span><span class="SyntaxComment">// x is @alias0</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y == </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)   </span><span class="SyntaxComment">// y is @alias1</span><span class="SyntaxCode">
    }
}</code>
</div>

<h3 id="105_006_variadic_parameters">Variadic parameters</h3><p>A function can take a variable number of arguments with <code class="incode">...</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunction</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">, parameters: ...)
    {
    }

    </span><span class="SyntaxFunction">myFunction</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxString">"true"</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">5.6</span><span class="SyntaxCode">)
}</code>
</div>
<p>In that case, <code class="incode">parameters</code> is a slice of <code class="incode">any</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunction</span><span class="SyntaxCode">(parameters: ...)
    {
        </span><span class="SyntaxComment">// We can know the number of parameters at runtime with '@countof'</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(parameters) == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)

        </span><span class="SyntaxComment">// Each parameter is a type 'any'</span><span class="SyntaxCode">
        #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">any</span><span class="SyntaxCode">
        #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">any</span><span class="SyntaxCode">
        #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">any</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// But you can use '@kindof' to get the real type (at runtime) of the parameter</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">string</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxFunction">myFunction</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">, </span><span class="SyntaxString">"true"</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">5.6</span><span class="SyntaxCode">)
}</code>
</div>
<p>If all variadic parameters are of the same type, you can force it. Parameters then won't be of type <code class="incode">any</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunction</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">, parameters: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">...)
    {
        #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">]).name == </span><span class="SyntaxString">"s32"</span><span class="SyntaxCode">
        #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">]).name == </span><span class="SyntaxString">"s32"</span><span class="SyntaxCode">
        #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
        #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(parameters[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxFunction">myFunction</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)
}</code>
</div>
<p>Variadic parameters can be passed from function to function. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">(params: ...)
    {
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(params) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(params[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">string</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">(params[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">]) == </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">string</span><span class="SyntaxCode">) params[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxString">"value"</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">bool</span><span class="SyntaxCode">) params[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">(params: ...)
    {
        </span><span class="SyntaxConstant">A</span><span class="SyntaxCode">(params)
    }

    </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">(</span><span class="SyntaxString">"value"</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can spread the content of an array or a slice to variadic parameters with <code class="incode">@spread</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(params: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">...)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxComment">// Note that variadic parameters can be visited, as this is a slice</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> total = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> v: params
            total += v
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> total
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">]
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> res = </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@spread</span><span class="SyntaxCode">(arr)) </span><span class="SyntaxComment">// is equivalent to sum(1, 2, 3, 4)</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(res == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> res1 = </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@spread</span><span class="SyntaxCode">(arr[</span><span class="SyntaxNumber">1.</span><span class="SyntaxCode">.</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">])) </span><span class="SyntaxComment">// is equivalent to sum(2, 3)</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(res1 == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">+</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="106_007_ufcs">Ufcs</h3><p><i>ufcs</i> stands for <i>uniform function call syntax</i>. It allows every functions to be called with a <code class="incode">param.func()</code> form when the first parameter of <code class="incode">func()</code> is of the same type as <code class="incode">param</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(param: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) =&gt; param

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> b = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(b) == b.</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">())
}</code>
</div>
<p>This means that in Swag, there are only <i>static</i> functions, but which can be called like <i>methods</i>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"> { x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">; }
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> pt: *</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) { x, y = value; }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt: </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Ufcs call</span><span class="SyntaxCode">
    pt.</span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> pt.y == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// Normal call</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(&pt, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> pt.y == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="107_008_constexpr">Constexpr</h3><p>A function marked with <code class="incode">Swag.ConstExpr</code> can be executed by the compiler if it can. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">) =&gt; x + y</code>
</div>
<p>Here <code class="incode">G</code> will be baked to 3 by the compiler. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> = </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
#assert </span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode"></code>
</div>
<p>If a function is not <code class="incode">ConstExpr</code>, you can force the compile time call with <code class="incode">#run</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">mul</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">) =&gt; x * y
</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G1</span><span class="SyntaxCode"> = </span><span class="SyntaxFunction">#run</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">mul</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">)
#assert </span><span class="SyntaxConstant">G1</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">18</span><span class="SyntaxCode"></code>
</div>

<h3 id="108_009_function_overloading">Function overloading</h3><p>Functions can have the same names as long as their parameters are different. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x + y
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(x, y, z: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x + y + z
}

#assert </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">
#assert </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode"></code>
</div>
<p>Note that in Swag, there is no implicit cast for function parameters. So you must always specify the right type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">over</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x + y
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">over</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">) =&gt; x * y

    </span><span class="SyntaxComment">// This would generate an error because it's ambiguous, as 1 and 2 are not strong types</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// var res0 = over(1, 2)</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> res0 = </span><span class="SyntaxFunction">over</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(res0 == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> res1 = </span><span class="SyntaxFunction">over</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">'</span><span class="SyntaxType">s64</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(res1 == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="109_010_discard">Discard</h3><p>By default, you must always use the returned value of a function, otherwise the compiler will generate an error. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x + y

    </span><span class="SyntaxComment">// This would generated an error, because the return value of 'sum' is not used</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// sum(2, 3)</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// To force the return value to be ignored, you can use 'discard' at the call site</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">discard</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)

}</code>
</div>
<p>If a function authorizes the caller to not use its return value, because it's not that important, it can be marked with <code class="incode">Swag.Discardable</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Discardable]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">mul</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> =&gt; x * y

    </span><span class="SyntaxComment">// This is fine to ignore the return value of 'mul' (even if strange)</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">mul</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="110_011_retval">Retval</h3><p>Inside a function, you can use the <code class="incode">retval</code> type which is an alias to the function return type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result: </span><span class="SyntaxKeyword">retval</span><span class="SyntaxCode">
        result = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> result
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
}</code>
</div>
<p>But <code class="incode">retval</code> will also make a direct reference to the caller storage, to avoid an unnecessary copy (if possible). So this is mostly a hint for the compiler, and usefull when the function returns a complexe type like a struct, a tuple or an array. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode"> { x, y, z: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">; }

    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getWhite</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxComment">// To avoid the clear of the returned struct, we use = undefined</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result: </span><span class="SyntaxKeyword">retval</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">undefined</span><span class="SyntaxCode">
        result.x = </span><span class="SyntaxNumber">0.5</span><span class="SyntaxCode">
        result.y = </span><span class="SyntaxNumber">0.1</span><span class="SyntaxCode">
        result.z = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> result
    }

    </span><span class="SyntaxComment">// Here the 'getWhite' function can directly modify r, g and b without storing</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// a temporary value on the stack.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> (r, g, b) = </span><span class="SyntaxFunction">getWhite</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(r == </span><span class="SyntaxNumber">0.5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(g == </span><span class="SyntaxNumber">0.1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(b == </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)
}</code>
</div>
<p>This is the preferred way (because optimal) to return a struct or an array. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">()-&gt;[</span><span class="SyntaxNumber">255</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxComment">// To avoid the clear of the array, we use = undefined</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result: </span><span class="SyntaxKeyword">retval</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">undefined</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> i: </span><span class="SyntaxNumber">255</span><span class="SyntaxCode">
            result[i] = i
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> result
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> arr = </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(arr[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(arr[</span><span class="SyntaxNumber">100</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">100</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(arr[</span><span class="SyntaxNumber">254</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">254</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="111_012_foreign">Foreign</h3><p>Swag can interop with external "modules" (dlls under windows), which contain exported C functions. </p>
<p>Put a special attribute <code class="incode">Swag.Foreign</code> before the function prototype, and specify the module name where the function is located. </p>
<p>The module name can be a swag compiled module, or an external system module (where the location depends on the OS). </p>
<p>In the case below, the function is located in <code class="incode">kernel32.dll</code> (under windows). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Foreign("kernel32")]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExitProcess</span><span class="SyntaxCode">(uExitCode: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">);

</span><span class="SyntaxComment">// Like for other attributes, you can use a block.</span><span class="SyntaxCode">
</span><span class="SyntaxAttribute">#[Swag.Foreign("kernel32")]</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExitProcess</span><span class="SyntaxCode">(uExitCode: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">);
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Sleep</span><span class="SyntaxCode">(dwMilliseconds: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">);
}</code>
</div>
<p>Note that in the case of an external module, you will have to declare somewhere the imported library too. </p>
<p><code class="incode">#foreignlib</code> is here to force a link to the given library (when generating executables). </p>
<div class="precode"><code></span><span class="SyntaxCode">#foreignlib </span><span class="SyntaxString">"kernel32"</span><span class="SyntaxCode"></code>
</div>

<h2 id="120_intrinsics">Intrinsics</h2><p>This is the list of all intrinsics. </p>
<p>All intrinsics start with <code class="incode">@</code>, which is reserved for them. </p>
<div class="precode"><code></span><span class="SyntaxCode">#global skip</code>
</div>
<h3>Base </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">);
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@breakpoint</span><span class="SyntaxCode">();
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@getcontext</span><span class="SyntaxCode">()-&gt;*</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Context</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@setcontext</span><span class="SyntaxCode">(context: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Context</span><span class="SyntaxCode">);
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@isbytecode</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@compiler</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ICompiler</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@args</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">string</span><span class="SyntaxCode">;
</span><span class="SyntaxIntrinsic">@panic</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@compilererror</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@compilerwarning</span><span class="SyntaxCode">()</code>
</div>
<h3>Buildin </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxIntrinsic">@spread</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@init</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@drop</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@postmove</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@postcopy</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@alignof</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@offsetof</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@kindof</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@stringof</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@dataof</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@mkslice</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@mkstring</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@mkany</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@mkinterface</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@mkcallback</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@getpinfos</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@isconstexpr</span><span class="SyntaxCode">()
</span><span class="SyntaxIntrinsic">@itftableof</span><span class="SyntaxCode">()

</span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode">
</span><span class="SyntaxIntrinsic">@errmsg</span><span class="SyntaxCode"></code>
</div>
<h3>Memory related </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@alloc</span><span class="SyntaxCode">(size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxType">void</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@realloc</span><span class="SyntaxCode">(ptr: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxType">void</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@free</span><span class="SyntaxCode">(ptr: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">);
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@memset</span><span class="SyntaxCode">(dst: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">);
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@memcpy</span><span class="SyntaxCode">(dst: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, src: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">);
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@memmove</span><span class="SyntaxCode">(dst: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, src: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">);
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@memcmp</span><span class="SyntaxCode">(dst, src: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@strlen</span><span class="SyntaxCode">(value: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;</code>
</div>
<h3>Atomic operations </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">,  exchangeWith: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">,  exchangeWith: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">,  compareTo, exchangeWith: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, compareTo, exchangeWith: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, compareTo, exchangeWith: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, compareTo, exchangeWith: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">,  compareTo, exchangeWith: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, compareTo, exchangeWith: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, compareTo, exchangeWith: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, compareTo, exchangeWith: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;</code>
</div>
<h3>Math </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sqrt</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sqrt</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sin</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sin</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@cos</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@cos</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@tan</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@tan</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sinh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sinh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@cosh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@cosh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@tanh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@tanh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@asin</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@asin</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@acos</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@acos</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atan</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atan</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log2</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log2</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log10</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log10</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@floor</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@floor</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@ceil</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@ceil</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@trunc</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@trunc</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@round</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@round</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@exp</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@exp</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@exp2</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@exp2</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@pow</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@pow</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1, value2: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountnz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountnz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountnz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountnz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcounttz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcounttz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcounttz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcounttz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountlz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountlz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountlz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountlz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@byteswap</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@byteswap</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@byteswap</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">;

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@muladd</span><span class="SyntaxCode">(val1, val2, val3: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">;
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@muladd</span><span class="SyntaxCode">(val1, val2, val3: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">;</code>
</div>

<h2 id="121_init">Init</h2><h3>@init </h3>
<p><code class="incode">@init</code> can be used to reinitialize a variable/array to the default value. </p>
<p>For a simple variable, the default value is 0. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@init</span><span class="SyntaxCode">(&x)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>Work also for an array, as you can specify the number of elements you want to initialize. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@init</span><span class="SyntaxCode">(&x, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)    </span><span class="SyntaxComment">// Initialize 2 elements</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can also specify a <i>value</i> to initialize, instead of the default one. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@init</span><span class="SyntaxCode">(&x)(</span><span class="SyntaxNumber">3.14</span><span class="SyntaxCode">)  </span><span class="SyntaxComment">// Initialize to 3.14 instead of zero</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">3.14</span><span class="SyntaxCode">)
}</code>
</div>
<p>Same for an array. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = [</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
    </span><span class="SyntaxIntrinsic">@init</span><span class="SyntaxCode">(&x, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)(</span><span class="SyntaxNumber">555</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">)
}</code>
</div>
<p>When called on a struct, the struct will be restored to the values defined in it. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">{r = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, g = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, b = </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">}

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> rgb: </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">{</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">}
    </span><span class="SyntaxIntrinsic">@init</span><span class="SyntaxCode">(&rgb)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb.r == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb.g == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb.b == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p>But you can also specified the values. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">{r = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, g = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, b = </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">}

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> rgb: </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">{</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">}
    </span><span class="SyntaxIntrinsic">@init</span><span class="SyntaxCode">(&rgb)(</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">7</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb.r == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb.g == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb.b == </span><span class="SyntaxNumber">7</span><span class="SyntaxCode">)
}</code>
</div>
<p>And this works also for array. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">{r = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, g = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, b = </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">}

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> rgb: [</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@init</span><span class="SyntaxCode">(&rgb, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)(</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">7</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">].r == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">].g == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">].b == </span><span class="SyntaxNumber">7</span><span class="SyntaxCode">)
}</code>
</div>
<h3>@drop </h3>
<p>For a struct, <code class="incode">@init</code> will <b>not</b> call <code class="incode">opDrop</code>, so this is mostly useful to initialize a plain old data. </p>
<p>But there is also <code class="incode">@drop</code> intrinsic, which works the same, except that it will <code class="incode">drop</code> all the content by calling <code class="incode">opDrop</code> if it is defined. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">{r = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, g = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, b = </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">}

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> rgb: [</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// In fact this is a no op, as struct RGB is plain old data, without a defined 'opDrop'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@drop</span><span class="SyntaxCode">(&rgb, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)

    </span><span class="SyntaxIntrinsic">@init</span><span class="SyntaxCode">(&rgb, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)(</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">7</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">].r == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">].g == </span><span class="SyntaxNumber">6</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(rgb[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">].b == </span><span class="SyntaxNumber">7</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="130_generic">Generic</h2>
<h3 id="131_001_declaration">Declaration</h3><p>A function can be generic by specifying some parameters after <code class="incode">func</code>. </p>
<p>At the call site, you specify the generic parameters with <code class="incode">funcCall'(type1, type2, ...)(parameters)</code>. Note that parenthesis can be omitted if there's only one generic parameter. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxComment">// Here 'T' is a generic type.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(val: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) =&gt; </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> * val

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4.0</span><span class="SyntaxCode">)
    }

    {
        </span><span class="SyntaxComment">// You can omit 'var' to declare the generic type, because a single identifier</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// is considered to be a type.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(val: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) =&gt; </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> * val

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4.0</span><span class="SyntaxCode">)
    }

    {
        </span><span class="SyntaxComment">// You can set a default value to the type.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode"> = </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(val: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) =&gt; </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> * val

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4.0</span><span class="SyntaxCode">)
    }

    {
        </span><span class="SyntaxComment">// Of course you can specify more than one generic parameter</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">K</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">V</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(key: </span><span class="SyntaxConstant">K</span><span class="SyntaxCode">, value: </span><span class="SyntaxConstant">V</span><span class="SyntaxCode">) =&gt; value

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxString">"value"</span><span class="SyntaxCode">) == </span><span class="SyntaxString">"value"</span><span class="SyntaxCode">)              </span><span class="SyntaxComment">// K and V are deduced</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxType">string</span><span class="SyntaxCode">)(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxString">"value"</span><span class="SyntaxCode">) == </span><span class="SyntaxString">"value"</span><span class="SyntaxCode">)    </span><span class="SyntaxComment">// K and V are explicit</span><span class="SyntaxCode">

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">) == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">) == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p>Generic types can be deduced from parameters, so <code class="incode">func'type()</code> is not always necessary. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(val: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) =&gt; </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> * val

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)         </span><span class="SyntaxComment">// T is deduced to be s32</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">4.0</span><span class="SyntaxCode">)     </span><span class="SyntaxComment">// T is deduced to be f32</span><span class="SyntaxCode">
}</code>
</div>
<p>You can also specify constants as generic parameters. </p>
<p><code class="incode">N</code> is a constant a type <code class="incode">s32</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">N</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">() = </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">N</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">()
}</code>
</div>
<p><code class="incode">const</code> can also be omitted, as an identifier followed by a type definition is considered to be a constant and not a type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">N</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">() = </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">N</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">()
}</code>
</div>
<p>You can also assign a default value to the constant. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">N</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">() = </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">N</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">()
}</code>
</div>
<p>You can ommit the type if you declare the constant with <code class="incode">const</code>. It will be deduced from the assignment expression. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">N</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">() = </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">N</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
    </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">()
}</code>
</div>
<p>You can mix types and constants. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxComment">// `T` is a type, `N` is a constant of type `s32`, because remember that an identifier</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// alone is considered to be a generic type.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">N</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(x: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) =&gt; x * </span><span class="SyntaxConstant">N</span><span class="SyntaxCode">

        </span><span class="SyntaxKeyword">namealias</span><span class="SyntaxCode"> call = </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">call</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">call</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">100</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">1000</span><span class="SyntaxCode">)
    }

    {
        </span><span class="SyntaxComment">// So if you want to declare multiple constants, specify the type (or 'const') for each.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">N</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">() =&gt; </span><span class="SyntaxConstant">T</span><span class="SyntaxCode"> * </span><span class="SyntaxConstant">N</span><span class="SyntaxCode">

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">'(</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)() == </span><span class="SyntaxNumber">50</span><span class="SyntaxCode">)
    }

    {
        </span><span class="SyntaxComment">// And if you want to declare multiple types with default values, specify the value for each.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode"> = </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">V</span><span class="SyntaxCode"> = </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(x: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">, y: </span><span class="SyntaxConstant">V</span><span class="SyntaxCode">) =&gt; x * y

        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">2.0</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p>Like functions, a struct can also be generic. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">
        {
            val: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
        }

        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(x.val) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x1: </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(x1.val) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)
    }

    {
        </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">N</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">
        {
            val: [</span><span class="SyntaxConstant">N</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
        }

        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxConstant">Struct</span><span class="SyntaxCode">'(</span><span class="SyntaxType">bool</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(x.val) == </span><span class="SyntaxType">#type</span><span class="SyntaxCode"> [</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">] </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)
    }
}</code>
</div>

<h3 id="132_002_validif">Validif</h3><h4>One time evaluation </h4>
<p>On a function, you can use <code class="incode">#validif</code> to check if the usage of the function is correct. </p>
<p>If the <code class="incode">#validif</code> expression returns false, then the function will not be considered for the call. If there's no other overload to match, then the compiler will raise an error. </p>
<p>The <code class="incode">#validif</code> expression is evaluated <b>only once</b>, whatever the call, so it is typically used to check generic parameters. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Here we validate the function only if the generic type is `s32` or `s64`.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(x: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">...)-&gt;</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
        #validif </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s64</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> total = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> it: x
            total += it
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> total
    }

    </span><span class="SyntaxComment">// This is ok.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> res1 = </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(res1 == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> res2 = </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">'</span><span class="SyntaxType">s64</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">10</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(res2 == </span><span class="SyntaxNumber">30</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// But the following would generate an error because the type is `f32`.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// So there's no match possible for that type.</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// var res1 = sum'f32(1, 2)</span><span class="SyntaxCode">
}</code>
</div>
<p>You can use <code class="incode">#validif</code> to make a kind of a generic specialisation. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// s32 version</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">isNull</span><span class="SyntaxCode">(x: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode">
        #validif </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> x == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// f32/f64 version</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">isNull</span><span class="SyntaxCode">(x: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode">
        #validif </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) == </span><span class="SyntaxType">f64</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(x) &lt; </span><span class="SyntaxNumber">0.01</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">isNull</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">))
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">isNull</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0.001</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">))
}</code>
</div>
<p>Instead of a single expression, <code class="incode">#validif</code> can be followed by a block that returns a <code class="incode">bool</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(x: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">...)-&gt;</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
        #validif
        {
            </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s64</span><span class="SyntaxCode">
                </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
        }
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> total = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> it: x
            total += it
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> total
    }
}</code>
</div>
<p>By using <code class="incode">@compilererror</code>, you can then trigger your own errors at compile time if the type is incorrect. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
        #validif
        {
            </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) == </span><span class="SyntaxType">s64</span><span class="SyntaxCode">
                </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
            </span><span class="SyntaxIntrinsic">@compilererror</span><span class="SyntaxCode">(</span><span class="SyntaxString">"invalid type "</span><span class="SyntaxCode"> ++ </span><span class="SyntaxIntrinsic">@stringof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">), </span><span class="SyntaxIntrinsic">@location</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">))
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
        }
    {
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> x + y
    }

    </span><span class="SyntaxComment">// This will trigger an error</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// var x = sum'f32(1, 2)</span><span class="SyntaxCode">
}</code>
</div>
<p><code class="incode">#validif</code> can also be used on a generic struct. Unlike functions, if the expression failed, then you will have an error right away because there's no overload in the case of structures. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">
        #validif </span><span class="SyntaxConstant">T</span><span class="SyntaxCode"> == </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">T</span><span class="SyntaxCode"> == </span><span class="SyntaxType">f64</span><span class="SyntaxCode">
    {
        x, y: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// Fine.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v: </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Error.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//var v: Point's32</span><span class="SyntaxCode">
}</code>
</div>
<h4>Multiple evaluations </h4>
<p>Instead of <code class="incode">#validif</code>, you can use <code class="incode">#validifx</code>. <code class="incode">#validifx</code> is evaluated for <b>each</b> call, so it can be used to check parameters, as long as they can be <b>evaluated at compile time</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    {
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">div</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
            #validifx
            {
                </span><span class="SyntaxComment">// Here we use '@isconstexpr'.</span><span class="SyntaxCode">
                </span><span class="SyntaxComment">// If 'y' cannot be evaluated at compile time, then we can do nothing about it.</span><span class="SyntaxCode">
                </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> !</span><span class="SyntaxIntrinsic">@isconstexpr</span><span class="SyntaxCode">(y)
                    </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
                </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> y == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
                    </span><span class="SyntaxIntrinsic">@compilererror</span><span class="SyntaxCode">(</span><span class="SyntaxString">"division by zero"</span><span class="SyntaxCode">, </span><span class="SyntaxIntrinsic">@location</span><span class="SyntaxCode">(y))
                </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
            }
        {
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> x / y
        }

        </span><span class="SyntaxComment">// Fine</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x1 = </span><span class="SyntaxFunction">div</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

        </span><span class="SyntaxComment">// Error at compile time, division by zero.</span><span class="SyntaxCode">

        </span><span class="SyntaxComment">// var x2 = div(1, 0)</span><span class="SyntaxCode">
    }

    {
        </span><span class="SyntaxComment">// A version of 'first' where 'x' is known at compile time.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">first</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
            #validifx </span><span class="SyntaxIntrinsic">@isconstexpr</span><span class="SyntaxCode">(x)
        {
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">
        }

        </span><span class="SyntaxComment">// A version of 'first' where 'x' is **not** known at compile time.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">first</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
            #validifx !</span><span class="SyntaxIntrinsic">@isconstexpr</span><span class="SyntaxCode">(x)
        {
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
        }

        </span><span class="SyntaxComment">// Will call version 1 because parameter is a literal.</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">first</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">555</span><span class="SyntaxCode">)

        </span><span class="SyntaxComment">// Will call version 2 because parameter is a variable.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> a: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">first</span><span class="SyntaxCode">(a) == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
    }
}</code>
</div>

<h3 id="133_003_constraint">Constraint</h3><p>Swag provides also a simple way of checking generic parameters, without the need of <code class="incode">#validif</code>. </p>
<p>A type constraint can be added when declaring a generic type. If a function or a struct is instantiated with a type that does not conform to the constraint, then an error will be raised. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// The type constraint is a compile time function (with #[Swag.ConstExpr]) that should return a bool.</span><span class="SyntaxCode">
    </span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isS32</span><span class="SyntaxCode">(t: </span><span class="SyntaxType">typeinfo</span><span class="SyntaxCode">) =&gt; t == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Here we check that the function generic type is 's32' by calling 'isS32'.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">: </span><span class="SyntaxFunction">isS32</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">)) </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(x: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">...)-&gt;</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> total = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> it: x
            total += it
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> total
    }

    </span><span class="SyntaxComment">// This is ok.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> res1 = </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(res1 == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// But the following would generate an error because the type is 'f32'.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// let res1 = sum'f32(1, 2)</span><span class="SyntaxCode">
}</code>
</div>
<p>The type constraint can be any compile time expression, as long as the resulting type is <code class="incode">bool</code>. </p>
<p>So you could do something like this. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isS32</span><span class="SyntaxCode">(t: </span><span class="SyntaxType">typeinfo</span><span class="SyntaxCode">)  =&gt; t == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isBool</span><span class="SyntaxCode">(t: </span><span class="SyntaxType">typeinfo</span><span class="SyntaxCode">) =&gt; t == </span><span class="SyntaxType">bool</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Here we check that the function generic type is 's32' or 'bool'.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">: </span><span class="SyntaxFunction">isS32</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isBool</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">)) </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(x: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) =&gt; x

    </span><span class="SyntaxComment">// This is ok.</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)  == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">) == </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
}</code>
</div>
<p>Works also for structs. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isFloat</span><span class="SyntaxCode">(t: </span><span class="SyntaxType">typeinfo</span><span class="SyntaxCode">) =&gt; t == </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">or</span><span class="SyntaxCode"> t == </span><span class="SyntaxType">f64</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">: </span><span class="SyntaxFunction">isFloat</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">)) </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">
    {
        x, y: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt:  </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt1: </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">'</span><span class="SyntaxType">f64</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// This will generate a type constraint error.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//var pt: Point's32</span><span class="SyntaxCode">
}</code>
</div>

<h2 id="160_scoping">Scoping</h2>
<h3 id="161_001_defer">Defer</h3><p><code class="incode">defer</code> is used to call an expression when the current scope is left. It's purely compile time, so it does not evaluate until the block is left. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">defer</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    v += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// defer expression will be executed here</span><span class="SyntaxCode">
}</code>
</div>
<p><code class="incode">defer</code> can also be used with a block. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">defer</span><span class="SyntaxCode">
    {
        v += </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v == </span><span class="SyntaxNumber">15</span><span class="SyntaxCode">)
    }

    v += </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// defer block will be executed here</span><span class="SyntaxCode">
}</code>
</div>
<p><code class="incode">defer</code> expressions are called when leaving the corresponding scope, even with <code class="incode">return</code>, <code class="incode">break</code>, <code class="incode">continue</code> etc., and even inside a <code class="incode">loop/while/for</code> etc. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">defer</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// will be called here, before breaking the loop</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// will be called here also</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p><code class="incode">defer</code> are executed in reverse order of their declaration. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">defer</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)   </span><span class="SyntaxComment">// Will be executed second</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">defer</span><span class="SyntaxCode"> x *= </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">            </span><span class="SyntaxComment">// Will be executed first</span><span class="SyntaxCode">
}</code>
</div>
<p>It's typically used to unregister/destroy a resource, by putting the release code just after the creation one. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createResource</span><span class="SyntaxCode">() =&gt; </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">releaseResource</span><span class="SyntaxCode">(resource: *</span><span class="SyntaxType">bool</span><span class="SyntaxCode">) = </span><span class="SyntaxKeyword">dref</span><span class="SyntaxCode"> resource = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isResourceCreated</span><span class="SyntaxCode">(b: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) =&gt; b

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> resource = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    {
        resource = </span><span class="SyntaxFunction">createResource</span><span class="SyntaxCode">()
        </span><span class="SyntaxLogic">defer</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(resource.</span><span class="SyntaxFunction">isResourceCreated</span><span class="SyntaxCode">())
            </span><span class="SyntaxFunction">releaseResource</span><span class="SyntaxCode">(&resource)
        }

        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@index</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">break</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(!resource.</span><span class="SyntaxFunction">isResourceCreated</span><span class="SyntaxCode">())
}</code>
</div>

<h3 id="162_002_using">Using</h3><p><code class="incode">using</code> brings the scope of a namespace, a struct or an enum in the current one. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">R</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">G</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">; }
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">R</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}</code>
</div>
<p><code class="incode">using</code> can also be used with a variable. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"> { x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt: </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> pt
    x = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// no need to specify 'pt'</span><span class="SyntaxCode">
    y = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// no need to specify 'pt'</span><span class="SyntaxCode">

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.y == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can declare a variable with <code class="incode">using</code> just before. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"> { x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">; y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">; }

    </span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt: </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">
    x = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// no need to specify 'pt'</span><span class="SyntaxCode">
    y = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// no need to specify 'pt'</span><span class="SyntaxCode">

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.y == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<h4>For a function parameter </h4>
<p><code class="incode">using</code> applied to a function parameter can be seen as the equivalent of the hidden <code class="incode">this</code> in C++. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"> { x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">; y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">; }

    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setOne</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> point: *</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxComment">// Here there's no need to specify 'point'</span><span class="SyntaxCode">
        x, y = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt: </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">setOne</span><span class="SyntaxCode">(&pt)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.y == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// ufcs</span><span class="SyntaxCode">
    pt.</span><span class="SyntaxFunction">setOne</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.y == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}</code>
</div>
<h4>For a field </h4>
<p><code class="incode">using</code> can also be used with a field inside a struct. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point2</span><span class="SyntaxCode">
    {
        x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point3</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> base: </span><span class="SyntaxConstant">Point2</span><span class="SyntaxCode">
        z: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// That way the content of the field can be referenced directly</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> value: </span><span class="SyntaxConstant">Point3</span><span class="SyntaxCode">
    value.x = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// Equivalent to value.base.x = 0</span><span class="SyntaxCode">
    value.y = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// Equivalent to value.base.y = 0</span><span class="SyntaxCode">
    value.z = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(&value.x == &value.base.x)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(&value.y == &value.base.y)

    </span><span class="SyntaxComment">// The compiler can then cast automatically 'Point3' to 'Point2'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">set1</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> ptr: *</span><span class="SyntaxConstant">Point2</span><span class="SyntaxCode">)
    {
        x, y = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxFunction">set1</span><span class="SyntaxCode">(&value) </span><span class="SyntaxComment">// Here the cast is automatic thanks to the using</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value.y == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value.base.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value.base.y == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}</code>
</div>

<h3 id="163_003_with">With</h3><p>You can use <code class="incode">with</code> to avoid repeating the same variable again and again. You can then access fields with a simple <code class="incode">.</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"> { x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">; }

</span><span class="SyntaxKeyword">impl</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">mtd</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setOne</span><span class="SyntaxCode">()
    {
        x, y = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    }
}</code>
</div>
<p><code class="incode">with</code> on a variable. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt: </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">with</span><span class="SyntaxCode"> pt
    {
        .x = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// equivalent to pt.x</span><span class="SyntaxCode">
        .y = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// equivalent to pt.y</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.y == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>
<p>Works for function calls to. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt: </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">with</span><span class="SyntaxCode"> pt
    {
        .</span><span class="SyntaxFunction">setOne</span><span class="SyntaxCode">()           </span><span class="SyntaxComment">// equivalent to pt.setOne() or setOne(pt)</span><span class="SyntaxCode">
        .y = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">              </span><span class="SyntaxComment">// equivalent to pt.y</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)    </span><span class="SyntaxComment">// equivalent to pt.x</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(.y == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)    </span><span class="SyntaxComment">// equivalent to pt.y</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.y == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p>Works also with a namespace. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">with</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">NameSpace</span><span class="SyntaxCode">
    {
        .</span><span class="SyntaxFunction">inside0</span><span class="SyntaxCode">()
        .</span><span class="SyntaxFunction">inside1</span><span class="SyntaxCode">()
    }
}</code>
</div>
<p>Instead of an identifier name, <code class="incode">with</code> also accepts a variable declaration. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">with</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt = </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">{</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">}
    {
        .x = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        .y = </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> pt.y == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">with</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt: </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">
    {
        .x = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        .y = </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> pt.y == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
}</code>
</div>
<p>Or an affectation statement. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> pt: </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">with</span><span class="SyntaxCode"> pt = </span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">{</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">}
    {
        .x = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        .y = </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(pt.x == </span><span class="SyntaxNumber">10</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> pt.y == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">)
}


</span><span class="SyntaxKeyword">namespace</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">NameSpace</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">inside0</span><span class="SyntaxCode">() {}
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">inside1</span><span class="SyntaxCode">() {}
}</code>
</div>

<h2 id="164_alias">Alias</h2><h3>Type alias </h3>
<p><code class="incode">typealias</code> is used to make a shortcut to another type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode"> { </span><span class="SyntaxConstant">R</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">G</span><span class="SyntaxCode">; </span><span class="SyntaxConstant">B</span><span class="SyntaxCode">; }
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">R</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Color</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// 'Color' is now equivalent to 'RGB'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can then use the new name in place of the original type. This does not create a new type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Float32</span><span class="SyntaxCode"> = </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Float64</span><span class="SyntaxCode"> = </span><span class="SyntaxType">f64</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxConstant">Float32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// Same as 'f32'</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> y: </span><span class="SyntaxConstant">Float64</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// Same as 'f64'</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Float32</span><span class="SyntaxCode">) == </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Float64</span><span class="SyntaxCode">) == </span><span class="SyntaxType">f64</span><span class="SyntaxCode">
}</code>
</div>
<p>But to create a new type, a <code class="incode">typealias</code> can also be marked with the <code class="incode">Swag.Strict</code> attribute. In that case, all implicit casts won't be done. Explicit cast are still possible. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Strict]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">typealias</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyType</span><span class="SyntaxCode"> = </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyType</span><span class="SyntaxCode">) != </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxConstant">MyType</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">MyType</span><span class="SyntaxCode">) </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
}</code>
</div>
<h3>Name alias </h3>
<p>You can alias a function name with <code class="incode">namealias</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">thisIsABigFunctionName</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) =&gt; x * x
    </span><span class="SyntaxKeyword">namealias</span><span class="SyntaxCode"> myFunc = thisIsABigFunctionName
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">16</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can also alias variables and namespaces. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> myLongVariableName: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">namealias</span><span class="SyntaxCode"> short = myLongVariableName
    short += </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(myLongVariableName == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
}</code>
</div>

<h2 id="170_error_management">Error management</h2><p>Swag contains a <b>very</b> simple error system used to deal with function returning errors. </p>
<p>It will probably be changed/improved at some point. </p>
<blockquote>
<p>These are <b>not</b> exceptions ! </p>
</blockquote>
<p>A function that can return an error must be marked with <code class="incode">throw</code>. It can then raise an error with the <code class="incode">throw</code> keyword, passing an error message. </p>
<blockquote>
<p>When an error is raised by a function, the return value is always equal to the <b>default value</b>, depending on the return type. </p>
</blockquote>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
{
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> name == </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxComment">// This function will return 0 in case of an error, because this is the default</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// value for 'u64'.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"> </span><span class="SyntaxString">"null pointer"</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(name)
}</code>
</div>
<p>The caller will then have to deal with the error in some way. It can <code class="incode">catch</code> it, and test (or not) its value with the <code class="incode">@errmsg</code> intrinsic. The execution will continue at the call site. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">()
{
    </span><span class="SyntaxComment">// Dismiss the eventual error with 'catch' and continue execution</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxKeyword">catch</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"fileName"</span><span class="SyntaxCode">)

    </span><span class="SyntaxComment">// And test it with @errmsg, which returns the 'throw' corresponding string</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@errmsg</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
        </span><span class="SyntaxIntrinsic">@print</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@errmsg</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxComment">// You can also use 'trycatch', which will exit the current function in case</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// an error has been raised (returning the default value if necessary)</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt1 = </span><span class="SyntaxKeyword">trycatch</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"fileName"</span><span class="SyntaxCode">)
}</code>
</div>
<p>The caller can stop the execution with <code class="incode">try</code>, and return to its own caller with the same error raised. The function must then also be marked with <code class="incode">throw</code>. </p>
<p>Here, the caller of <code class="incode">myFunc1</code> will have to deal with the error at its turn. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunc1</span><span class="SyntaxCode">() </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// If 'count()' raises an error, 'myFunc1' will return with the same error</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxKeyword">try</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"filename"</span><span class="SyntaxCode">)
}</code>
</div>
<p>The caller can also panic if an error is raised, with <code class="incode">assume</code>. </p>
<blockquote>
<p>This can be disabled in release builds (in that case the behaviour is undefined). </p>
</blockquote>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunc2</span><span class="SyntaxCode">()
{
    </span><span class="SyntaxComment">// Here the program will stop with a panic message if 'count()' throws an error</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxKeyword">assume</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"filename"</span><span class="SyntaxCode">)
}</code>
</div>
<p>Note that you can use a block instead of one single statement (this does not create a scope). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunc3</span><span class="SyntaxCode">() </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// This is not really necessary, see below, but this is just to show 'try' with a block</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// instead of one single call</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">try</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt0 = </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"filename"</span><span class="SyntaxCode">)
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt1 = </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"other filename"</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxKeyword">assume</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt2 = </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"filename"</span><span class="SyntaxCode">)
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt3 = </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"other filename"</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxComment">// Works also for 'catch' if you do not want to deal with the error message.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// '@errmsg' in that case is not really relevant.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">catch</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt4 = </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"filename"</span><span class="SyntaxCode">)
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt5 = </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"other filename"</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxComment">// Works also for 'trycatch' if you do not want to deal with the error message</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// and you want to return as soon as an error is raised.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">trycatch</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt6 = </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"filename"</span><span class="SyntaxCode">)
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt7 = </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxString">"other filename"</span><span class="SyntaxCode">)
    }
}</code>
</div>
<p>When a function is marked with <code class="incode">throw</code>, the <code class="incode">try</code> for a function call is automatic if not specified. That means that most of the time it's not necessary to specify it. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">mySubFunc2</span><span class="SyntaxCode">() </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"> </span><span class="SyntaxString">"error"</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">mySubFunc1</span><span class="SyntaxCode">() </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxComment">// In fact there's no need to add a 'try' before the call because 'mySubFunc1' is</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// marked with 'throw'</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// This is less verbose when you do not want to do something special in case</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// of errors (with 'assume', 'catch' or 'trycatch')</span><span class="SyntaxCode">
        </span><span class="SyntaxFunction">mySubFunc2</span><span class="SyntaxCode">()
    }

    </span><span class="SyntaxKeyword">catch</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">mySubFunc1</span><span class="SyntaxCode">()
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@errmsg</span><span class="SyntaxCode"> == </span><span class="SyntaxString">"error"</span><span class="SyntaxCode">)
}</code>
</div>
<h3>defer </h3>
<p><code class="incode">defer</code> can have parameters like <code class="incode">defer(err)</code> or <code class="incode">defer(noerr)</code> to control if it should be executed depending on the error status. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> g_Defer = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">raiseError</span><span class="SyntaxCode">() </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"> </span><span class="SyntaxString">"error"</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">testDefer</span><span class="SyntaxCode">(err: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
{
    </span><span class="SyntaxLogic">defer</span><span class="SyntaxCode">(err)      g_Defer += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// This will be called in case an error is raised, before exiting</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">defer</span><span class="SyntaxCode">(noerr)    g_Defer += </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// This will only be called in case an error is not raised</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">defer</span><span class="SyntaxCode">           g_Defer += </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// This will be called in both cases</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> err
        </span><span class="SyntaxFunction">raiseError</span><span class="SyntaxCode">()
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    g_Defer = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">catch</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">testDefer</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(g_Defer == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)   </span><span class="SyntaxComment">// Will call only defer(err) and the normal defer</span><span class="SyntaxCode">

    g_Defer = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">catch</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">testDefer</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(g_Defer == </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">)   </span><span class="SyntaxComment">// Will call only defer(noerr) and the normal defer</span><span class="SyntaxCode">
}</code>
</div>

<h2 id="175_safety">Safety</h2><p>Swag comes with a bunch of safety checks which can be activated by module, function or even instruction with the <code class="incode">#<a href="swag.runtime.html#Swag_Safety">Swag.Safety</a></code> attribute. </p>
<p>Safety checks can also be changed for a specific build configuration (<code class="incode">--cfg:&lt;config&gt;</code>) with <code class="incode">buildCfg.safetyGuards</code>. </p>
<blockquote>
<p>Swag comes with four predefined configurations : <code class="incode">debug</code>, <code class="incode">fast-debug</code>, <code class="incode">fast-compile</code> and <code class="incode">release</code>. Safety checks are disabled in <code class="incode">fast-compile</code> and <code class="incode">release</code>. </p>
</blockquote>
<h3>overflow </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Safety("overflow", true)]</span><span class="SyntaxCode"></code>
</div>
<p>Swag will panic if some operators overflow and if we lose some bits during an integer conversion. </p>
<p>Operators that can overflow are : <code class="incode">+ - * &lt;&lt; &gt;&gt;</code> and their equivalent <code class="incode">+= -= <i>= &lt;&lt;= &gt;&gt;=</code>.</i> </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">255</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// x += 1      // This would overflow, and panic, because we lose informations</span><span class="SyntaxCode">
}</code>
</div>
<p>But if you know what your are doing, you can use a special version of those operators, which will not panic. Add the <code class="incode">,over</code> modifier after the operation. This will disable safety checks. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">255</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode">
    x +=,over </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// Overflow is expected, so this will wrap around</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can also use <code class="incode">#<a href="swag.runtime.html#Swag_Overflow(true)">Swag.Overflow(true)</a></code> to authorize overflow on a more global scale. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Overflow(true)]</span><span class="SyntaxCode">
</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">255</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode">
    x += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// No need for operator modifier ',over'</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)
}</code>
</div>
<p>For 8 or 16 bits, you can promote an operation to 32 bits by using <code class="incode">,up</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">255</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode"> +,up </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">256</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(x) == </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)
}</code>
</div>
<p>Swag will also check that a cast does not lose information. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x1 = </span><span class="SyntaxNumber">255</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">//var y0 = cast(s8) x1     // This would lose information and panic, as 255 cannot be encoded in 's8'</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@print(y0)</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y1 = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">,</span><span class="SyntaxFunction">over</span><span class="SyntaxCode">(</span><span class="SyntaxType">s8</span><span class="SyntaxCode">) x1  </span><span class="SyntaxComment">// But ',over' can be used on the cast operation too</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y1 == -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x2 = -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">s8</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//var y2 = cast(u8) x2     // This cast also is not possible, because 'x2' is negative and 'y' is 'u8'</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@print(y)</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y2 = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">,</span><span class="SyntaxFunction">over</span><span class="SyntaxCode">(</span><span class="SyntaxType">u8</span><span class="SyntaxCode">) x2
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(y2 == </span><span class="SyntaxNumber">255</span><span class="SyntaxCode">)
}</code>
</div>
<p>Rember that you can disable these safety checks with the corresponding attribute. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Overflow(true)]</span><span class="SyntaxCode">
</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">255</span><span class="SyntaxCode">'</span><span class="SyntaxType">u8</span><span class="SyntaxCode">
    x += </span><span class="SyntaxNumber">255</span><span class="SyntaxCode">    </span><span class="SyntaxComment">// 254</span><span class="SyntaxCode">
    x += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">      </span><span class="SyntaxComment">// 255</span><span class="SyntaxCode">
    x &gt;&gt;= </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// 127</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">127</span><span class="SyntaxCode">)
}</code>
</div>
<h3>any </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Safety("any", true)]</span><span class="SyntaxCode"></code>
</div>
<p>Swag will panic if a bad cast from <code class="incode">any</code> is performed. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x: </span><span class="SyntaxType">any</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"1"</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> y = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">string</span><span class="SyntaxCode">) x     </span><span class="SyntaxComment">// This is valid, because this is the correct underlying type</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//var z = cast(s32) x      // This is not valid, and will panic</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@assert(z == 0)</span><span class="SyntaxCode">
}</code>
</div>
<h3>boundcheck </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Safety("boundcheck", true)]</span><span class="SyntaxCode"></code>
</div>
<p>Swag will panic if an index is out of range when dereferencing a sized value like an array, a slice, a string... </p>
<p>Safety for fixed size arrays. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = [</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> idx = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@assert(x[idx] == 1)     // '10' is out of range, will panic</span><span class="SyntaxCode">
}</code>
</div>
<p>Safety when indexing a slice. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> x: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> idx = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x[idx] == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)        </span><span class="SyntaxComment">// '1' is in range, ok</span><span class="SyntaxCode">
    idx += </span><span class="SyntaxNumber">9</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@assert(x[idx] == 1)      // '10' is out of range, will panic</span><span class="SyntaxCode">
}</code>
</div>
<p>Safety when slicing a sized value. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = [</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">]
    </span><span class="SyntaxComment">//var slice = x[1..4]              // '4' is out of range, will panic</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@assert(slice[0] == 1)</span><span class="SyntaxCode">
}

</span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> idx = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//var slice = x[0..idx]            // '10' is out of range, will panic</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@assert(slice[0] == `s`)</span><span class="SyntaxCode">
}</code>
</div>
<h3>math </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Safety("math", true)]</span><span class="SyntaxCode"></code>
</div>
<p>Swag will panic if some math operations are invalid. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> y = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//var z = x / y        // Division by zero, panic</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@print(z)</span><span class="SyntaxCode">
}</code>
</div>
<p>Swag will also check for invalid arguments on some math intrinsics. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// All of this will panic if the arguments are unsupported.</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">//@abs(-128)</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@log(-2'f32)</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@log2(-2'f32)</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@log10(2'f64)</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@sqrt(-2'f32)</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@asin(-2'f32)</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//@acos(2'f32)</span><span class="SyntaxCode">
}</code>
</div>
<h3>switch </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Safety("switch", true)]</span><span class="SyntaxCode"></code>
</div>
<p>Swag will panic if a switch is marked with <code class="incode">#<a href="swag.runtime.html#Swag_Complete">Swag.Complete</a></code>, but the value is not covered by a <code class="incode">case</code>. </p>
<h3>bool </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Safety("bool", true)]</span><span class="SyntaxCode"></code>
</div>
<p>Swag will panic if a boolean value is not <code class="incode">true</code> (1) or <code class="incode">false</code> (0). </p>
<h3>nan </h3>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Safety("nan", true)]</span><span class="SyntaxCode"></code>
</div>
<p>Swag will panic if a floating point <code class="incode">NaN</code> is used in an operation. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
}</code>
</div>

<h2 id="180_compiler_declarations">Compiler declarations</h2>
<h3 id="181_001_compile_time_evaluation">Compile time evaluation</h3><p><code class="incode">#assert</code> is a static assert (at compile time). </p>
<div class="precode"><code></span><span class="SyntaxCode">#assert </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode"></code>
</div>
<p><code class="incode">@defined(SYMBOL)</code> returns true, at compile time, if the given symbol exists in the current context. </p>
<div class="precode"><code></span><span class="SyntaxCode">#assert !</span><span class="SyntaxIntrinsic">@defined</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">DOES_NOT_EXISTS</span><span class="SyntaxCode">)
#assert </span><span class="SyntaxIntrinsic">@defined</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Global</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Global</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"></code>
</div>
<p>A static <code class="incode">#if/#elif/#else</code>, with an expression that can be evaluated at compile time. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DEBUG</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RELEASE</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
#if </span><span class="SyntaxConstant">DEBUG</span><span class="SyntaxCode">
{
}
#elif </span><span class="SyntaxConstant">RELEASE</span><span class="SyntaxCode">
{
}
#else
{
}</code>
</div>
<p><code class="incode">#error</code> to raise a compile time error, and <code class="incode">#warning</code> to raise a compile time warning. </p>
<div class="precode"><code></span><span class="SyntaxCode">#if </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
{
    #error   </span><span class="SyntaxString">"this is an error"</span><span class="SyntaxCode">
    #warning </span><span class="SyntaxString">"this is a warning"</span><span class="SyntaxCode">
}

</span><span class="SyntaxComment">// 'isThisDebug' is marked with 'Swag.ConstExpr', so it can be automatically evaluated</span><span class="SyntaxCode">
</span><span class="SyntaxComment">// at compile time</span><span class="SyntaxCode">
</span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isThisDebug</span><span class="SyntaxCode">() =&gt; </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">

</span><span class="SyntaxComment">// This call is valid</span><span class="SyntaxCode">
#if </span><span class="SyntaxFunction">isThisDebug</span><span class="SyntaxCode">() == </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
{
    #error </span><span class="SyntaxString">"this should not be called !"</span><span class="SyntaxCode">
}

</span><span class="SyntaxComment">// This time 'isThisRelease' is not marked with 'Swag.ConstExpr'</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isThisRelease</span><span class="SyntaxCode">() =&gt; </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">

</span><span class="SyntaxComment">// But this call is still valid because we force the compile time execution with '#run'</span><span class="SyntaxCode">
#if </span><span class="SyntaxFunction">#run</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isThisRelease</span><span class="SyntaxCode">() == </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">
{
    #error </span><span class="SyntaxString">"this should not be called !"</span><span class="SyntaxCode">
}</code>
</div>
<p>A more complicated <code class="incode">#assert</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">factorial</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
{
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> x * </span><span class="SyntaxFunction">factorial</span><span class="SyntaxCode">(x - </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
}

#assert </span><span class="SyntaxFunction">factorial</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">) == </span><span class="SyntaxNumber">24</span><span class="SyntaxCode"> </span><span class="SyntaxComment">// Evaluated at compile time</span><span class="SyntaxCode"></code>
</div>

<h3 id="182_002_special_functions">Special functions</h3><div class="precode"><code></span><span class="SyntaxCode">#global skip</code>
</div>
<h4>#test </h4>
<p><code class="incode">#test</code> is a special function than can be used in the <code class="incode">tests/</code> folder of the workspace. All <code class="incode">#test</code> will be executed only if swag is running in test mode. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
}</code>
</div>
<h4>#main </h4>
<p><code class="incode">#main</code> is the program entry point. It can only be defined <b>once</b> per module, and has meaning only for an executable. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#main</span><span class="SyntaxCode">
{
}</code>
</div>
<p>Unlike the C function <code class="incode">main()</code>, there's no argument, but you can use the intrinsic <code class="incode">@args</code> to get a slice of all the parameters. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#main</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> myArgs = </span><span class="SyntaxIntrinsic">@args</span><span class="SyntaxCode">()
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> count = </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(myArgs)
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> myArgs[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxString">"fullscreen"</span><span class="SyntaxCode">
    {
        ...
    }
}</code>
</div>
<h4>#init </h4>
<p><code class="incode">#init</code> will be called at runtime, during the module initialization. You can have as many <code class="incode">#init</code> as you want, but the execution order in the same module is undefined. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#init</span><span class="SyntaxCode">
{
}</code>
</div>
<h4>#drop </h4>
<p><code class="incode">#drop</code> will be called at runtime, when module is unloaded. You can have as many <code class="incode">#drop</code> as you want. The execution order in the same module is undefined, but is always the inverse order of <code class="incode">#init</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#drop</span><span class="SyntaxCode">
{
}</code>
</div>
<h4>#premain </h4>
<p><code class="incode">#premain</code> will be called after all the modules have done their <code class="incode">#init</code> code, but before the <code class="incode">#main</code> function is called. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#premain</span><span class="SyntaxCode">
{
}</code>
</div>

<h3 id="183_003_run">Run</h3><p><code class="incode">#run</code> is a special function that will be called at <b>compile time</b>. </p>
<p>It can be used to precompute some global values for example. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G</span><span class="SyntaxCode">: [</span><span class="SyntaxNumber">5</span><span class="SyntaxCode">] </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">undefined</span><span class="SyntaxCode"></code>
</div>
<p>Initialize <code class="incode">G</code> with <code class="incode">[1,2,4,8,16]</code> at compile time. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#run</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> value = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> i: </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">G</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxConstant">G</span><span class="SyntaxCode">[i] = value
        value *= </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    }
}</code>
</div>
<p><code class="incode">#test</code> are executed after <code class="incode">#run</code>, even at compile time (during testing). </p>
<p>So we can test the values of <code class="incode">G</code> here. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">G</span><span class="SyntaxCode">[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">G</span><span class="SyntaxCode">[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">G</span><span class="SyntaxCode">[</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">G</span><span class="SyntaxCode">[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">8</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">G</span><span class="SyntaxCode">[</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] == </span><span class="SyntaxNumber">16</span><span class="SyntaxCode">)
}</code>
</div>
<p><code class="incode">#run</code> can also be used as an expression, to call for example a function not marked with <code class="incode">#<a href="swag.runtime.html#Swag_ConstExpr">Swag.ConstExpr</a></code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SumValue</span><span class="SyntaxCode"> = </span><span class="SyntaxFunction">#run</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">, </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">) + </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
#assert </span><span class="SyntaxConstant">SumValue</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">20</span><span class="SyntaxCode">

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sum</span><span class="SyntaxCode">(values: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">...)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> v: values
        result += v
    </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> result
}</code>
</div>
<p><code class="incode">#run</code> can also be used as an expression block. </p>
<p>The return type is deduced from the <code class="incode">return</code> statement. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode"> = </span><span class="SyntaxFunction">#run</span><span class="SyntaxCode"> {
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> result: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">loop</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">
        result += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> result   </span><span class="SyntaxComment">// 'Value' will be of type 'f32'</span><span class="SyntaxCode">
}
#assert </span><span class="SyntaxConstant">Value</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">10.0</span><span class="SyntaxCode"></code>
</div>

<h3 id="184_004_global">Global</h3><p>A bunch of <code class="incode">#global</code> can start a source file. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxComment">// Skip the content of the file, like this one (but must be a valid swag file)</span><span class="SyntaxCode">
#global skip

</span><span class="SyntaxComment">// All symbols in the file will be public/internal</span><span class="SyntaxCode">
#global </span><span class="SyntaxKeyword">public</span><span class="SyntaxCode">
#global </span><span class="SyntaxKeyword">internal</span><span class="SyntaxCode">

</span><span class="SyntaxComment">// All symbols in the file will go in the namespace 'Toto'</span><span class="SyntaxCode">
#global </span><span class="SyntaxKeyword">namespace</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Toto</span><span class="SyntaxCode">

</span><span class="SyntaxComment">// A #if for the whole file</span><span class="SyntaxCode">
#global </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DEBUG</span><span class="SyntaxCode"> == </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">

</span><span class="SyntaxComment">// Some attributes can be assigned to the full file</span><span class="SyntaxCode">
#global </span><span class="SyntaxAttribute">#[Swag.Safety("", true)]</span><span class="SyntaxCode">

</span><span class="SyntaxComment">// The file will be exported for external usage</span><span class="SyntaxCode">
</span><span class="SyntaxComment">// It's like putting everything in public, except that the file will</span><span class="SyntaxCode">
</span><span class="SyntaxComment">// be copied in its totality in the public folder</span><span class="SyntaxCode">
#global export

</span><span class="SyntaxComment">// Link with a given external library</span><span class="SyntaxCode">
#foreignlib </span><span class="SyntaxString">"windows.lib"</span><span class="SyntaxCode"></code>
</div>

<h3 id="185_005_var">Var</h3><p>A global variable can be tagged with <code class="incode">#<a href="swag.runtime.html#Swag_Tls">Swag.Tls</a></code> to store it in the thread local storage (one copy per thread). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Tls]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"></code>
</div>
<p>A local variable can be tagged with <code class="incode">#<a href="swag.runtime.html#Swag_Global">Swag.Global</a></code> to make it global (aka <code class="incode">static</code> in C/C++). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxAttribute">#[Swag.Global]</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G1</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

        </span><span class="SyntaxConstant">G1</span><span class="SyntaxCode"> += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G1</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">toto</span><span class="SyntaxCode">() == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p>A global variable can also be marked as <code class="incode">#<a href="swag.runtime.html#Swag_Compiler">Swag.Compiler</a></code>. That kind of variable will not be exported to the runtime and can only be used in compile time code. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Compiler]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G2</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

</span><span class="SyntaxFunction">#run</span><span class="SyntaxCode">
{
    </span><span class="SyntaxConstant">G2</span><span class="SyntaxCode"> += </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
}</code>
</div>

<h2 id="190_attributes">Attributes</h2><p>Attributes are tags associated with functions, structures etc... </p>

<h3 id="191_001_user_attributes">User attributes</h3><p>User attributes are declared like functions, but with the <code class="incode">attr</code> keyword before instead of <code class="incode">func</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AttributeA</span><span class="SyntaxCode">()</code>
</div>
<p>Like functions, attributes can have parameters. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AttributeB</span><span class="SyntaxCode">(x, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, z: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</div>
<p>So attributes can also have default values. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AttributeBA</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxString">"string"</span><span class="SyntaxCode">)</code>
</div>
<p>You can define a usage before the attribute definition to restrict its usage. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AttributeC</span><span class="SyntaxCode">()</code>
</div>
<p>To use an attribute, the syntax is <code class="incode">#[attribute, attribute...]</code>. It should be placed <b>before</b> the thing you want to tag. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[AttributeA, AttributeB(0, 0, "string")]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">function1</span><span class="SyntaxCode">()
{
}</code>
</div>
<p>You can declare multiple usages. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AttributeD</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">);

</span><span class="SyntaxAttribute">#[AttributeD(6)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">function2</span><span class="SyntaxCode">()
{
}

</span><span class="SyntaxAttribute">#[AttributeD(150)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> struct1
{
}</code>
</div>
<p>Finaly, attributes can be retrieved at runtime thanks to <b>type reflection</b>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> type = </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(function2)                  </span><span class="SyntaxComment">// Get the type of the function</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(type.attributes) == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)     </span><span class="SyntaxComment">// Check that the function has one attribute associated with it</span><span class="SyntaxCode">
}</code>
</div>

<h3 id="192_002_predefined_attributes">Predefined attributes</h3><p>This is the list of predefined attributes. </p>
<p>All are located in the reserved <code class="incode">Swag</code> namespace. </p>
<div class="precode"><code></span><span class="SyntaxCode">#global skip

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ConstExpr</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.File)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PrintBc</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function|AttributeUsage.GlobalVariable)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Compiler</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Inline</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Macro</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Mixin</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Test</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Implicit</span><span class="SyntaxCode">()

</span><span class="SyntaxComment">// Hardcoded also for switch</span><span class="SyntaxCode">
</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Complete</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CalleeReturn</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Foreign</span><span class="SyntaxCode">(module: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, function: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxString">""</span><span class="SyntaxCode">);

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Callback</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function|AttributeUsage.Variable)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Discardable</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.Enum|AttributeUsage.EnumValue)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Deprecated</span><span class="SyntaxCode">(msg: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)

</span><span class="SyntaxComment">// Hardcoded for type typealias</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Strict</span><span class="SyntaxCode">()

</span><span class="SyntaxComment">// Hardcoded for local variables</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Global</span><span class="SyntaxCode">()

</span><span class="SyntaxComment">// Hardcoded for struct and variables</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Align</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Struct)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Pack</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Struct)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">NoCopy</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.StructVariable)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Offset</span><span class="SyntaxCode">(name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Enum)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumFlags</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Enum)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumIndex</span><span class="SyntaxCode">()

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.All|AttributeUsage.File)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Safety</span><span class="SyntaxCode">(what: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.All)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SelectIf</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)

</span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function|AttributeUsage.File)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Optim</span><span class="SyntaxCode">(what: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</div>

<h2 id="200_type_reflection">Type reflection</h2><p>In Swag, types are also values that can be inspected at compile time or at runtime. The two main intrinsics for this are <code class="incode">@typeof</code> and <code class="incode">@kindof</code>. </p>
<p>You can get the type of an expression with <code class="incode">@typeof</code>, or just with the type itself (<b>types are also values</b>). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr1 = </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxType">s8</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr1.name == </span><span class="SyntaxString">"s8"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr1 == </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr2 = </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxType">s16</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr2.name == </span><span class="SyntaxString">"s16"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr2 == </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr3 = </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr3.name == </span><span class="SyntaxString">"s32"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr3 == </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">))

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr4 = </span><span class="SyntaxType">s64</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr4.name == </span><span class="SyntaxString">"s64"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr4 == </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)
}</code>
</div>
<p>The return result of <code class="incode">@typeof</code> is a const pointer to a <code class="incode">Swag.TypeInfo</code> kind of structure. This is an typealias for the <code class="incode">typeinfo</code> type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr = </span><span class="SyntaxType">bool</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(ptr) == </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">TypeInfoNative</span><span class="SyntaxCode">))

    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr1 = </span><span class="SyntaxType">#type</span><span class="SyntaxCode"> [</span><span class="SyntaxNumber">2</span><span class="SyntaxCode">] </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(ptr1) == </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">TypeInfoArray</span><span class="SyntaxCode">))
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr1.name == </span><span class="SyntaxString">"[2] s32"</span><span class="SyntaxCode">)
}</code>
</div>
<p>The <code class="incode">TypeInfo</code> structure contains a different enum value for each type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> ptr = </span><span class="SyntaxType">f64</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(ptr.kind).fullname == </span><span class="SyntaxString">"Swag.TypeInfoKind"</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(ptr.sizeof == </span><span class="SyntaxIntrinsic">@sizeof</span><span class="SyntaxCode">(</span><span class="SyntaxType">f64</span><span class="SyntaxCode">))
}</code>
</div>
<h3>@decltype </h3>
<p><code class="incode">@decltype</code> can be used to transform a <code class="incode">typeinfo</code> to a real compiler type. This is the opposite of <code class="incode">@typeof</code> or <code class="incode">@kindof</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxIntrinsic">@decltype</span><span class="SyntaxCode">(</span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxType">s32</span><span class="SyntaxCode">))
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(x) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
}</code>
</div>
<p><code class="incode">@decltype</code> can evaluate a constexpr expression that returns a <code class="incode">typeinfo</code> to determine the real type. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.ConstExpr]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getType</span><span class="SyntaxCode">(needAString: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">typeinfo</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> needAString
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxType">string</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">else</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x: </span><span class="SyntaxIntrinsic">@decltype</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">getType</span><span class="SyntaxCode">(needAString = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">))
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(x) == </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    x = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> x1: </span><span class="SyntaxIntrinsic">@decltype</span><span class="SyntaxCode">(</span><span class="SyntaxFunction">getType</span><span class="SyntaxCode">(needAString = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">))
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(x1) == </span><span class="SyntaxType">string</span><span class="SyntaxCode">
    x1 = </span><span class="SyntaxString">"0"</span><span class="SyntaxCode">
}</code>
</div>

<h2 id="210_code_inspection">Code inspection</h2><p><code class="incode">#message</code> is a special function that will be called by the compiler when something specific occurs during the build. The parameter of <code class="incode">#message</code> is a mask that tells the compiler when to call the function. </p>
<p>With the <code class="incode">Swag.CompilerMsgMask.SemFunctions</code> flag, for example, <code class="incode">#message</code> will be called each time a function of the module <b>has been typed</b>. You can then use <code class="incode">getMessage()</code> in the <code class="incode">@compiler()</code> interface to retrieve some informations about the reason of the call. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#message</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">CompilerMsgMask</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SemFunctions</span><span class="SyntaxCode">)
{
    </span><span class="SyntaxComment">// Get the interface to communicate with the compiler</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> itf = </span><span class="SyntaxIntrinsic">@compiler</span><span class="SyntaxCode">()

    </span><span class="SyntaxComment">// Get the current message</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> msg = itf.</span><span class="SyntaxFunction">getMessage</span><span class="SyntaxCode">()

    </span><span class="SyntaxComment">// We know that the type in the message is a function because of the '#message' mask.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// So we can safely cast.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> typeFunc = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">TypeInfoFunc</span><span class="SyntaxCode">) msg.type

    </span><span class="SyntaxComment">// The message name, for `Swag.CompilerMsgMask.SemFunctions`, is the name of the</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// function being compiled.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> nameFunc = msg.name

    </span><span class="SyntaxComment">// As an example, we count that name if it starts with "XX".</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// But we could use custom function attributes instead...</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(nameFunc) &gt; </span><span class="SyntaxNumber">2</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> nameFunc[</span><span class="SyntaxNumber">0</span><span class="SyntaxCode">] == </span><span class="SyntaxString">`X`</span><span class="SyntaxCode"> </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> nameFunc[</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">] == </span><span class="SyntaxString">`X`</span><span class="SyntaxCode">
        </span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
}

</span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">

</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">XXTestFunc1</span><span class="SyntaxCode">() {}
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">XXTestFunc2</span><span class="SyntaxCode">() {}
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">XXTestFunc3</span><span class="SyntaxCode">() {}</code>
</div>
<p>The compiler will call the following function after the semantic pass. So after <b>all the functions</b> of the module have been parsed. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#message</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">CompilerMsgMask</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">PassAfterSemantic</span><span class="SyntaxCode">)
{
    </span><span class="SyntaxComment">// We should have found 3 functions starting with "XX"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">G</span><span class="SyntaxCode"> == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<p>This will be called for every global variables of the module. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#message</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">CompilerMsgMask</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SemGlobals</span><span class="SyntaxCode">)
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> itf = </span><span class="SyntaxIntrinsic">@compiler</span><span class="SyntaxCode">()
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> msg = itf.</span><span class="SyntaxFunction">getMessage</span><span class="SyntaxCode">()
}</code>
</div>
<p>This will be called for every global types of the module (structs, enums, interfaces...). </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#message</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">CompilerMsgMask</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SemTypes</span><span class="SyntaxCode">)
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> itf = </span><span class="SyntaxIntrinsic">@compiler</span><span class="SyntaxCode">()
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> msg = itf.</span><span class="SyntaxFunction">getMessage</span><span class="SyntaxCode">()
}</code>
</div>

<h2 id="220_meta_programmation">Meta programmation</h2><p>In Swag you can construct some source code at compile time, which will then be compiled. The source code you provide in the form of a <b>string</b> must be a valid Swag program. </p>

<h3 id="221_001_ast">Ast</h3><p>The most simple way to produce a string which contains the Swag code to compile is with an <code class="incode">#ast</code> block. An <code class="incode">#ast</code> block is executed at compile time and the string it returns will be compiled <b>inplace</b>. </p>
<p>The <code class="incode">#ast</code> can be a simple expression with the string to compile. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxFunction">#ast</span><span class="SyntaxCode"> </span><span class="SyntaxString">"var x = 666"</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(x == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
}</code>
</div>
<p>Or it can be a block, with an explicit <code class="incode">return</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> cpt = </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">
    </span><span class="SyntaxFunction">#ast</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">INC</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">5</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxString">"cpt += "</span><span class="SyntaxCode"> ++ </span><span class="SyntaxConstant">INC</span><span class="SyntaxCode">   </span><span class="SyntaxComment">// Equivalent to 'cpt += 5'</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(cpt == </span><span class="SyntaxNumber">7</span><span class="SyntaxCode">)
}</code>
</div>
<h4>Struct and enums </h4>
<p><code class="incode">#ast</code> can for example be used to generate the content of a <code class="incode">struct</code> or <code class="incode">enum</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxFunction">#ast</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxString">"x, y: s32 = 666"</span><span class="SyntaxCode">
        }
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.x == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">)
}</code>
</div>
<p>It works with generics too, and can be mixed with static declarations. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxFunction">#ast</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxString">"x, y: "</span><span class="SyntaxCode"> ++ </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">).name
        }

        z: </span><span class="SyntaxType">string</span><span class="SyntaxCode">
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">'</span><span class="SyntaxType">bool</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(v.x) == </span><span class="SyntaxType">bool</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(v.y) == </span><span class="SyntaxType">bool</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(v.z) == </span><span class="SyntaxType">string</span><span class="SyntaxCode">

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v1: </span><span class="SyntaxConstant">MyStruct</span><span class="SyntaxCode">'</span><span class="SyntaxType">f64</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(v1.x) == </span><span class="SyntaxType">f64</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(v1.y) == </span><span class="SyntaxType">f64</span><span class="SyntaxCode">
    #assert </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(v1.z) == </span><span class="SyntaxType">string</span><span class="SyntaxCode">
}</code>
</div>
<p><code class="incode">#ast</code> needs to return a <i>string like</i> value, which can of course be dynamically constructed. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxAttribute">#[Swag.Compiler]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">append</span><span class="SyntaxCode">(buf: ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, val: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> len = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">while</span><span class="SyntaxCode"> buf[len] len += </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">
        </span><span class="SyntaxIntrinsic">@memcpy</span><span class="SyntaxCode">(buf + len, </span><span class="SyntaxIntrinsic">@dataof</span><span class="SyntaxCode">(val), </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">u64</span><span class="SyntaxCode">) </span><span class="SyntaxIntrinsic">@countof</span><span class="SyntaxCode">(val) + </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    }

    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxFunction">#ast</span><span class="SyntaxCode">
        {
            </span><span class="SyntaxComment">// We construct the code to compile in this local array</span><span class="SyntaxCode">
            </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> buf: [</span><span class="SyntaxNumber">256</span><span class="SyntaxCode">] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">
            </span><span class="SyntaxFunction">append</span><span class="SyntaxCode">(buf, </span><span class="SyntaxString">"x: f32 = 1\n"</span><span class="SyntaxCode">)
            </span><span class="SyntaxFunction">append</span><span class="SyntaxCode">(buf, </span><span class="SyntaxString">"y: f32 = 2\n"</span><span class="SyntaxCode">)
            </span><span class="SyntaxFunction">append</span><span class="SyntaxCode">(buf, </span><span class="SyntaxString">"z: f32 = 3\n"</span><span class="SyntaxCode">)

            </span><span class="SyntaxComment">// And returns to the compiler the corresponding *code*</span><span class="SyntaxCode">
            </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxType">string</span><span class="SyntaxCode">) buf
        }
    }

    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> v: </span><span class="SyntaxConstant">Vector3</span><span class="SyntaxCode">
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.x == </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.y == </span><span class="SyntaxNumber">2</span><span class="SyntaxCode">)
    </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(v.z == </span><span class="SyntaxNumber">3</span><span class="SyntaxCode">)
}</code>
</div>
<h4>For example </h4>
<p>This is a real life example of an <code class="incode">#ast</code> usage from the <code class="incode">Std.Core</code> module. Here we generate a structure which contains all the fields of an original other structure, but where the types are forced to be <code class="incode">bool</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxConstant">IsSet</span><span class="SyntaxCode">
{
    </span><span class="SyntaxFunction">#ast</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxComment">// A `StringBuilder` is used to manipulate dynamic strings.</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> str = </span><span class="SyntaxConstant">StrConv</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">StringBuilder</span><span class="SyntaxCode">{}

        </span><span class="SyntaxComment">// We get the type of the generic parameter 'T'</span><span class="SyntaxCode">
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> typeof = </span><span class="SyntaxIntrinsic">@typeof</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">)

        </span><span class="SyntaxComment">// Then we visit all the fields, assuming the type is a struct (or this will not compile).</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// For each original field, we create one with the same name, but with a `bool` type.</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> f: typeof.fields
            str.</span><span class="SyntaxFunction">appendFormat</span><span class="SyntaxCode">(</span><span class="SyntaxString">"%: bool\n"</span><span class="SyntaxCode">, f.name)

        </span><span class="SyntaxComment">// Then we return the constructed source code.</span><span class="SyntaxCode">
        </span><span class="SyntaxComment">// It will be used by the compiler to generate the content of the `IsSet` struct.</span><span class="SyntaxCode">
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> str.</span><span class="SyntaxFunction">toString</span><span class="SyntaxCode">()
    }
}</code>
</div>
<h4>At global scope </h4>
<p><code class="incode">#ast</code> can also be called at the global scope. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#ast</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> value = </span><span class="SyntaxNumber">666</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">return</span><span class="SyntaxCode"> </span><span class="SyntaxString">"const myGeneratedConst = "</span><span class="SyntaxCode"> ++ value
}</code>
</div>
<p>But be aware that you must use <code class="incode">#placeholder</code> in case you are generating global symbols that can be used by something else in the code. This will tell Swag that <i>this symbol</i> will exist at some point, so please wait for it to <i>exist</i> before complaining. </p>
<div class="precode"><code></span><span class="SyntaxCode">#placeholder myGeneratedConst   </span><span class="SyntaxComment">// Symbol `myGeneratedConst` will be generated</span><span class="SyntaxCode"></code>
</div>
<p>Here for example, thanks to the <code class="incode">#placeholder</code>, the <code class="incode">#assert</code> will wait for the symbol <code class="incode">myGeneratedConst</code> to be replaced with its real content. </p>
<div class="precode"><code></span><span class="SyntaxCode">#assert myGeneratedConst == </span><span class="SyntaxNumber">666</span><span class="SyntaxCode"></code>
</div>

<h3 id="222_002_compiler_interface">Compiler interface</h3><p>The other method to compile generated code is to use the function <code class="incode">compileString()</code> in the <code class="incode">@compiler()</code> interface. Of course this should be called at compile time, and mostly during a <code class="incode">#message</code> call. </p>
<p>Here is a real life example from the <code class="incode">Std.Ogl</code> module (opengl wrapper), which uses <code class="incode">#message</code> to track functions marked with a specific <b>user attribute</b> <code class="incode">Ogl.Extension</code>, and generates some code for each function that has been found. </p>
<p>First we declare a new specific attribute, which can then be associated with a function. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[AttrUsage(AttributeUsage.Function)]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Extension</span><span class="SyntaxCode">()

</span><span class="SyntaxComment">// Here is an example of usage of that attribute.</span><span class="SyntaxCode">
</span><span class="SyntaxAttribute">#[Extension]</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">glUniformMatrix2x3fv</span><span class="SyntaxCode">(location: </span><span class="SyntaxConstant">GLint</span><span class="SyntaxCode">, count: </span><span class="SyntaxConstant">GLsizei</span><span class="SyntaxCode">, transpose: </span><span class="SyntaxConstant">GLboolean</span><span class="SyntaxCode">, value: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">GLfloat</span><span class="SyntaxCode">);
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">glUniformMatrix2x4fv</span><span class="SyntaxCode">(location: </span><span class="SyntaxConstant">GLint</span><span class="SyntaxCode">, count: </span><span class="SyntaxConstant">GLsizei</span><span class="SyntaxCode">, transpose: </span><span class="SyntaxConstant">GLboolean</span><span class="SyntaxCode">, value: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">GLfloat</span><span class="SyntaxCode">);
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">glUniformMatrix3x2fv</span><span class="SyntaxCode">(location: </span><span class="SyntaxConstant">GLint</span><span class="SyntaxCode">, count: </span><span class="SyntaxConstant">GLsizei</span><span class="SyntaxCode">, transpose: </span><span class="SyntaxConstant">GLboolean</span><span class="SyntaxCode">, value: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">GLfloat</span><span class="SyntaxCode">);
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">glUniformMatrix3x4fv</span><span class="SyntaxCode">(location: </span><span class="SyntaxConstant">GLint</span><span class="SyntaxCode">, count: </span><span class="SyntaxConstant">GLsizei</span><span class="SyntaxCode">, transpose: </span><span class="SyntaxConstant">GLboolean</span><span class="SyntaxCode">, value: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">GLfloat</span><span class="SyntaxCode">);
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">glUniformMatrix4x2fv</span><span class="SyntaxCode">(location: </span><span class="SyntaxConstant">GLint</span><span class="SyntaxCode">, count: </span><span class="SyntaxConstant">GLsizei</span><span class="SyntaxCode">, transpose: </span><span class="SyntaxConstant">GLboolean</span><span class="SyntaxCode">, value: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">GLfloat</span><span class="SyntaxCode">);
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">glUniformMatrix4x3fv</span><span class="SyntaxCode">(location: </span><span class="SyntaxConstant">GLint</span><span class="SyntaxCode">, count: </span><span class="SyntaxConstant">GLsizei</span><span class="SyntaxCode">, transpose: </span><span class="SyntaxConstant">GLboolean</span><span class="SyntaxCode">, value: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">GLfloat</span><span class="SyntaxCode">);
}</code>
</div>
<p>The following will be used to track the functions with that specific attribute. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OneFunc</span><span class="SyntaxCode">
{
    type: </span><span class="SyntaxType">typeinfo</span><span class="SyntaxCode">
    name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">
}

</span><span class="SyntaxAttribute">#[Compiler]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> g_Functions: </span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'</span><span class="SyntaxConstant">OneFunc</span><span class="SyntaxCode"></code>
</div>
<p>This <code class="incode">#message</code> will be called for each function of the <code class="incode">Ogl</code> module. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#message</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">CompilerMsgMask</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SemFunctions</span><span class="SyntaxCode">)
{
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> itf = </span><span class="SyntaxIntrinsic">@compiler</span><span class="SyntaxCode">()
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> msg = itf.</span><span class="SyntaxFunction">getMessage</span><span class="SyntaxCode">()

    </span><span class="SyntaxComment">// If the function does not have our attribute, forget it</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> !</span><span class="SyntaxConstant">Reflection</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">hasAttribute</span><span class="SyntaxCode">(msg.type, </span><span class="SyntaxConstant">Extension</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">return</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// We just track all the functions with the given attribute</span><span class="SyntaxCode">
    g_Functions.</span><span class="SyntaxFunction">add</span><span class="SyntaxCode">({msg.type, msg.name})
}</code>
</div>
<p>We will generate a <code class="incode">glInitExtensions</code> global function, so we register it as a place holder. </p>
<div class="precode"><code></span><span class="SyntaxCode">#placeholder glInitExtensions</code>
</div>
<p>This is called once all functions of the module have been typed, and this is the main code generation. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#message</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">CompilerMsgMask</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">PassAfterSemantic</span><span class="SyntaxCode">)
{
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> builderVars: </span><span class="SyntaxConstant">StringBuilder</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> builderInit: </span><span class="SyntaxConstant">StringBuilder</span><span class="SyntaxCode">

    </span><span class="SyntaxComment">// Generate the `glInitExtensions` function</span><span class="SyntaxCode">
    builderInit.</span><span class="SyntaxFunction">appendString</span><span class="SyntaxCode">(</span><span class="SyntaxString">"public func glInitExtensions()\n{\n"</span><span class="SyntaxCode">);

    </span><span class="SyntaxComment">// Visit all functions we have registered, i.e. all functions with the `Ogl.Extension` attribute.</span><span class="SyntaxCode">
    </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> e: g_Functions
    {
        </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> typeFunc = </span><span class="SyntaxKeyword">cast</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">TypeInfoFunc</span><span class="SyntaxCode">) e.type

        </span><span class="SyntaxComment">// Declare a lambda variable for that extension</span><span class="SyntaxCode">
        builderVars.</span><span class="SyntaxFunction">appendFormat</span><span class="SyntaxCode">(</span><span class="SyntaxString">"var ext_%: %\n"</span><span class="SyntaxCode">, e.name, typeFunc.name)

        </span><span class="SyntaxComment">// Make a wrapper function</span><span class="SyntaxCode">
        builderVars.</span><span class="SyntaxFunction">appendFormat</span><span class="SyntaxCode">(</span><span class="SyntaxString">"public func %("</span><span class="SyntaxCode">, e.name)
        </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> p, i: typeFunc.parameters
        {
            </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> i != </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"> builderVars.</span><span class="SyntaxFunction">appendString</span><span class="SyntaxCode">(</span><span class="SyntaxString">", "</span><span class="SyntaxCode">)
            builderVars.</span><span class="SyntaxFunction">appendFormat</span><span class="SyntaxCode">(</span><span class="SyntaxString">"p%: %"</span><span class="SyntaxCode">, i, p.pointedType.name)
        }

        </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> typeFunc.returnType == </span><span class="SyntaxType">void</span><span class="SyntaxCode">
            builderVars.</span><span class="SyntaxFunction">appendFormat</span><span class="SyntaxCode">(</span><span class="SyntaxString">")\n{\n"</span><span class="SyntaxCode">)
        </span><span class="SyntaxLogic">else</span><span class="SyntaxCode">
            builderVars.</span><span class="SyntaxFunction">appendFormat</span><span class="SyntaxCode">(</span><span class="SyntaxString">")-&gt;%\n{\n"</span><span class="SyntaxCode">, typeFunc.returnType.name)
        builderVars.</span><span class="SyntaxFunction">appendFormat</span><span class="SyntaxCode">(</span><span class="SyntaxString">"\treturn ext_%("</span><span class="SyntaxCode">, e.name)
        </span><span class="SyntaxLogic">visit</span><span class="SyntaxCode"> p, i: typeFunc.parameters
        {
            </span><span class="SyntaxLogic">if</span><span class="SyntaxCode"> i != </span><span class="SyntaxNumber">0</span><span class="SyntaxCode"> builderVars.</span><span class="SyntaxFunction">appendString</span><span class="SyntaxCode">(</span><span class="SyntaxString">", "</span><span class="SyntaxCode">)
            builderVars.</span><span class="SyntaxFunction">appendFormat</span><span class="SyntaxCode">(</span><span class="SyntaxString">"p%"</span><span class="SyntaxCode">, i)
        }

        builderVars.</span><span class="SyntaxFunction">appendString</span><span class="SyntaxCode">(</span><span class="SyntaxString">");\n}\n\n"</span><span class="SyntaxCode">)

        </span><span class="SyntaxComment">// Initialize the variable with the getExtensionAddress</span><span class="SyntaxCode">
        builderInit.</span><span class="SyntaxFunction">appendFormat</span><span class="SyntaxCode">(</span><span class="SyntaxString">"\text_% = cast(%) getExtensionAddress(@dataof(\"%\"))\n"</span><span class="SyntaxCode">, e.name, typeFunc.name, e.name);
    }

    </span><span class="SyntaxComment">// Compile !!</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> itf = </span><span class="SyntaxIntrinsic">@compiler</span><span class="SyntaxCode">()
    </span><span class="SyntaxKeyword">var</span><span class="SyntaxCode"> str = builderVars.</span><span class="SyntaxFunction">toString</span><span class="SyntaxCode">()
    itf.</span><span class="SyntaxFunction">compileString</span><span class="SyntaxCode">(str.</span><span class="SyntaxFunction">toString</span><span class="SyntaxCode">())

    builderInit.</span><span class="SyntaxFunction">appendString</span><span class="SyntaxCode">(</span><span class="SyntaxString">"}\n"</span><span class="SyntaxCode">);
    str = builderInit.</span><span class="SyntaxFunction">toString</span><span class="SyntaxCode">()
    itf.</span><span class="SyntaxFunction">compileString</span><span class="SyntaxCode">(str.</span><span class="SyntaxFunction">toString</span><span class="SyntaxCode">())
}</code>
</div>

<h2 id="230_documentation">Documentation</h2><p>The Swag compiler can generate documentation for all the modules of a given workspace. </p>
<div class="precode"><code></span><span class="SyntaxCode">swag doc -w:myWorkspaceFolder</code>
</div>
<p>The main module documentation should be placed at the top of the corresponding <code class="incode">module.swg</code> file. The rest is placed in various source files. </p>
<p>The documentation comment needs to be placed just before a function, struct or enum. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// Everything between empty lines is considered to be a simple paragraph. Which</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// means that if you put several comments on several lines like this, they all</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// will be part of the same paragraph.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// This is another paragraph because there's an empty line before.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// This is yet another paragraph.</span><span class="SyntaxCode">
}</code>
</div>
<p>The first paragraph is considered to be the 'short description' which can appear on specific parts of the documentation. So make it short. </p>
<p>If the first line ends with a dot <code class="incode">.</code>, then this marks the end of the paragraph, i.e. the end of the short description. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// This is the short description.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// As the previous first line ends with '.', this is another paragraph, so this should be</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// the long description. No need for an empty line before.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">test</span><span class="SyntaxCode">()
    {
    }
}</code>
</div>
<p>A paragraph that starts with <code class="incode">---</code> is a paragraph where every blanks and end of lines are respected. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// ---</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// Even...</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// ...empty lines are preserved.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// You end that kind of paragraph with another '---' alone on its line.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// ---</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">test</span><span class="SyntaxCode">()
    {
    }
}</code>
</div>
<p>You can create a list of bullet points with <code class="incode"><i></code>.</i> </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
 {
    </span><span class="SyntaxComment">// * This is a bullet point</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// * This is a bullet point</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// * This is a bullet point</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
    {
        r, g, b: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    }
}</code>
</div>
<p>You can create a quote with <code class="incode">&gt;</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// This is the short description.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// &gt; This is a block quote on multiple</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// &gt; lines.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// &gt;</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// &gt; End of the quote.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
    {
        r, g, b: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    }
}</code>
</div>
<p>You can create a table with <code class="incode">|</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// A table with 4 lines of 2 columns:</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// | 'boundcheck'   | Check out of bound access</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// | 'overflow'     | Check type conversion lost of bits or precision</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// | 'math'         | Various math checks (like a negative '@sqrt')</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// | 'switch'       | Check an invalid case in a '#[Swag.Complete]' switch</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">myFunc</span><span class="SyntaxCode">()
    {
    }
}</code>
</div>
<p>You can create a code paragraph with three backticks. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// For example:</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// ```</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// if a == true</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">//   @print("true")</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// ```</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">test</span><span class="SyntaxCode">()
    {
    }
}</code>
</div>
<p>For constants or enum values, the document comment is the one declared at the end of the line. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">A</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">     </span><span class="SyntaxComment">// This is a documentation comment</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">enum</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">
    {
        </span><span class="SyntaxConstant">Red</span><span class="SyntaxCode">         </span><span class="SyntaxComment">// This is a documentation comment</span><span class="SyntaxCode">
    }
}</code>
</div>
<p>Some other markdown markers (or kind of) are also supported inside paragraphs. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// `this is code` (backtick) for 'inline' code.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// 'single_word'  (tick) for 'inline' code.</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// *italic*</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// **bold**</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// # Title</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// ## Title</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">
    {
        r, g, b: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
    }
}</code>
</div>
<p>You can create a reference to something in the current module with <code class="incode">[name]</code> or <code class="incode">[name1.name2 etc.]</code>. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// This is a function with a 'value' parameter.</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">one</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)
    {
    }

    </span><span class="SyntaxComment">// This is a reference to [one]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">two</span><span class="SyntaxCode">()
    {
    }
}</code>
</div>
<p>The attribute <code class="incode">#<a href="swag.runtime.html#Swag_NoDoc">Swag.NoDoc</a></code> can be used to avoid a given element to appear in the documentation. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#test</span><span class="SyntaxCode">
{
    </span><span class="SyntaxComment">// This function will be ignored when generating documentation.</span><span class="SyntaxCode">
    </span><span class="SyntaxAttribute">#[Swag.NoDoc]</span><span class="SyntaxCode">
    </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">one</span><span class="SyntaxCode">()
    {
    }
}</code>
</div>
</div>
</div>
</div>
</body>
</html>
