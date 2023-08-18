#include "pch.h"
#include "GenDoc.h"
#include "Module.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Workspace.h"
#include "Version.h"
#include "SyntaxColor.h"

void GenDoc::outputStyles()
{
    helpOutput += "<style>\n";
    helpOutput +=
        ".container {\n\
            display:        flex;\n\
            flex-wrap:      nowrap;\n\
            flex-direction: row;\n\
            height:         100%;\n\
            line-height:    1.3em;\n\
            font-family:    Segoe UI;\n\
        }\n\
        .left {\n\
            display:    block;\n\
            overflow-y: scroll;\n\
            width:      650;\n\
            height:     100%;\n\
        }\n\
        .right {\n\
            display:     block;\n\
            overflow-y:  scroll;\n\
            width:       100%;\n\
            line-height: 1.3em;\n\
            height:      100%;\n\
        }\n\
        .page {\n\
            width:  1000;\n\
            margin: 0 auto;\n\
        }\n\
        blockquote {\n\
            background-color:   LightYellow;\n\
            border-left:        6px solid Orange;\n\
            padding:            10px;\n\
            width:              90%;\n\
        }\n\
        a {\n\
            text-decoration: none;\n\
            color:           DoggerBlue;\n\
        }\n\
        a:hover {\n\
            text-decoration: underline;\n\
        }\n\
        a.src {\n\
            font-size:          90%;\n\
            color:              LightGrey;\n\
        }\n\
        table.enumeration {\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              100%;\n\
            font-size:          90%;\n\
        }\n\
        td.enumeration {\n\
            padding:            6px;\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              30%;\n\
        }\n\
        td.tdname {\n\
            padding:            6px;\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              20%;\n\
            background-color:   #f8f8f8;\n\
        }\n\
        td.tdtype {\n\
            padding:            6px;\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              auto;\n\
        }\n\
        td:last-child {\n\
            width:              auto;\n\
        }\n\
        .tocbullet {\n\
            list-style-type:    none;\n\
            margin-left:        -20px;\n\
        }\n\
        .titletype {\n\
            font-weight:        normal;\n\
            font-size:          80%;\n\
        }\n\
        .titlelight {\n\
            font-weight:        normal;\n\
        }\n\
        .titlestrong {\n\
            font-weight:        bold;\n\
            font-size:          100%;\n\
        }\n\
        h1.content {\n\
            margin-top:         50px;\n\
            margin-bottom:      50px;\n\
        }\n\
        h2.section {\n\
            background-color:   Black;\n\
            color:              White;\n\
            padding:            6px;\n\
        }\n\
        table.h3 {\n\
            background-color:   Black;\n\
            color:              White;\n\
            width:              100%;\n\
            margin-top:         70px;\n\
            margin-right:       0px;\n\
        }\n\
        h3.content {\n\
            margin-bottom:      2px;\n\
            margin-right:       0px;\n\
            width:              100%;\n\
        }\n\
        .srcref {\n\
            text-align:         right;\n\
        }\n\
        .incode {\n\
            background-color:   #eeeeee;\n\
            padding:            2px;\n\
            border: 1px dotted  #cccccc;\n\
        }\n\
        .tdname .incode {\n\
            background-color:   revert;\n\
            padding:            2px;\n\
            border:             revert;\n\
        }\n\
        .addinfos {\n\
            font-size:          90%;\n\
            white-space:        break-spaces;\n\
            overflow-wrap:      break-word;\n\
        }\n\
        .code {\n\
            background-color:   #eeeeee;\n\
            border:             1px solid LightGrey;\n\
            padding:            10px;\n\
            width:              94%;\n\
            margin-left:        20px;\n\
        }\n\
    ";

    float lum = module ? module->buildCfg.docSyntaxColorLum : 0.5f;
    helpOutput += Fmt(".SyntaxCode { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxCode, lum));
    helpOutput += Fmt(".SyntaxComment { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxComment, lum));
    helpOutput += Fmt(".SyntaxCompiler { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxCompiler, lum));
    helpOutput += Fmt(".SyntaxFunction { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxFunction, lum));
    helpOutput += Fmt(".SyntaxConstant { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxConstant, lum));
    helpOutput += Fmt(".SyntaxIntrinsic { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxIntrinsic, lum));
    helpOutput += Fmt(".SyntaxType { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxType, lum));
    helpOutput += Fmt(".SyntaxKeyword { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxKeyword, lum));
    helpOutput += Fmt(".SyntaxLogic { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxLogic, lum));
    helpOutput += Fmt(".SyntaxNumber { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxNumber, lum));
    helpOutput += Fmt(".SyntaxString { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxString, lum));
    helpOutput += Fmt(".SyntaxAttribute { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxAttribute, lum));

    helpOutput += "</style>\n";
}

void GenDoc::generate(Module* mdl, DocKind docKind)
{
    module = mdl;

    // Output filename
    auto filePath = g_Workspace->targetPath;
    if (!module)
    {
        filePath.append("swag.runtime.html");
    }
    else
    {
        filePath.append(g_Workspace->workspacePath.filename().string().c_str());
        filePath += ".";
        filePath += module->name.c_str();
        filePath += ".html";
    }

    fullFileName = filePath.string();
    fullFileName.makeLower();
    fileName = filePath.filename().string();

    // Write for output
    FILE* f = nullptr;
    if (fopen_s(&f, fullFileName.c_str(), "wb"))
    {
        Report::errorOS(Fmt(Err(Err0524), fullFileName.c_str()));
        return;
    }

    helpOutput += "<html>\n";
    helpOutput += "<body>\n";

    helpOutput += "<head>\n";
    helpOutput += "<meta charset=\"UTF-8\">\n";

    // Css
    bool userCss = false;
    if (module && module->buildCfg.docCss.buffer && module->buildCfg.docCss.count)
    {
        Utf8 css{(const char*) module->buildCfg.docCss.buffer, (uint32_t) module->buildCfg.docCss.count};
        helpOutput += Fmt("<link rel=\"stylesheet\" type=\"text/css\" href=\"/%s\">\n", css.c_str());
        userCss = true;
    }

    helpOutput += "</head>\n";

    // Embbeded styles
    if (!userCss)
        outputStyles();

    switch (docKind)
    {
    case DocKind::Api:
        generateApi();
        break;
    case DocKind::Examples:
        generateExamples();
        break;
    }

    // Main page (left and right parts, left is for table of content, right is for content)
    helpOutput += "<div class=\"container\">\n";
    helpOutput += "<div class=\"left\">\n";
    helpOutput += helpToc;
    helpOutput += "</div>\n";

    // Right page start
    helpOutput += "<div class=\"right\">\n";
    helpOutput += "<div class=\"page\">\n";
    helpOutput += helpContent;
    helpOutput += "</div>\n";
    helpOutput += "</div>\n";
    helpOutput += "</div>\n";

    helpOutput += "</body>\n";
    helpOutput += "</html>\n";

    // Write file
    fwrite(helpOutput.c_str(), 1, helpOutput.length(), f);
    fclose(f);
}
