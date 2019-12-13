#pragma once
struct Utf8;
struct Concat;
struct Scope;
struct AstNode;
struct TypeInfoParam;

namespace DocHtmlHelper
{
    Utf8 markdown(const Utf8& msg);
    void htmlStart(Concat& outFile);
    void htmlEnd(Concat& outFile);
    void title(Concat& outFile, const Utf8& msg);
    void summary(Concat& outFile, const Utf8& msg);
    void sectionTitle1(Concat& outFile, const Utf8& title);
    void sectionTitle2(Concat& outFile, const Utf8& msg); 
    void sectionTitle3(Concat& outFile, const Utf8& msg);
    void startTable(Concat& outFile);
    void endTable(Concat& outFile);
    void startTableRow(Concat& outFile);
    void endTableRow(Concat& outFile);
    void tableNameCell(Concat& outFile, const Utf8& href, const Utf8& msg);
    void tableDescCell(Concat& outFile, const Utf8& msg);
    void table(Concat& outFile, Scope* scope, const set<AstNode*>& nodes);
    void table(Concat& outFile, Scope* scope, const vector<TypeInfoParam*>& params, bool specificRefFile = true);
    void origin(Concat& outFile, Scope* scope);

}; // namespace DocHtmlHelper