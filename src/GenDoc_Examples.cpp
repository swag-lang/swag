#include "pch.h"
#include "ErrorIds.h"
#include "GenDoc.h"
#include "Module.h"
#include "Report.h"

void GenDoc::addTitle(const Utf8& title, int level)
{
	helpContent += FMT("<h%d id=\"%s\">", level, title.c_str());
	helpContent += title;
	helpContent += FMT("</h%d>", level);

	helpToc += FMT("<li><a href=\"#%s\">%s</a></li>\n", title.c_str(), title.c_str());
}

bool GenDoc::processMarkDownFile(const Path& fileName, int titleLevel)
{
	ifstream ifs(fileName);

	if (!ifs)
	{
		Report::errorOS(FMT(Err(Err0096), fileName.c_str()));
		return false;
	}

	Vector<Utf8> lines;
	string       line;
	while (std::getline(ifs, line))
		lines.push_back(line);

	UserComment result;
	computeUserComments(result, lines, false);
	outputUserComment(result, titleLevel);

	return true;
}

bool GenDoc::processSourceFile(const Path& fileName, int titleLevel)
{
	ifstream ifs(fileName);

	if (!ifs)
	{
		Report::errorOS(FMT(Err(Err0096), fileName.c_str()));
		return false;
	}

	Vector<Utf8> lines;
	string       line;
	while (std::getline(ifs, line))
		lines.push_back(line);

	int i = 0;
	while (i < static_cast<int>(lines.size()))
	{
		Utf8 code;
		while (i < static_cast<int>(lines.size()))
		{
			auto& l        = lines[i++];
			Utf8  lineTrim = l;
			lineTrim.trim();
			if (lineTrim.startsWith("/**"))
				break;
			code += l;
			code += "\n";
		}

		outputCode(code, GENDOC_CODE_BLOCK | GENDOC_CODE_SYNTAX_COL);

		Vector<Utf8> linesUser;
		while (i < static_cast<int>(lines.size()))
		{
			auto& l        = lines[i++];
			Utf8  lineTrim = l;
			lineTrim.trim();
			if (lineTrim.startsWith("*/"))
				break;
			linesUser.push_back(l);
		}

		UserComment result;
		computeUserComments(result, linesUser, false);
		outputUserComment(result, titleLevel);
	}

	return true;
}

bool GenDoc::generateExamples()
{
	ranges::sort(module->files, [](const SourceFile* a, const SourceFile* b)
	{
		return strcmp(a->name.c_str(), b->name.c_str()) < 0;
	});

	helpToc += "<ul>\n";

	// Parse all files
	tocLastTitleLevel = 1;
	for (const auto file : module->files)
	{
		Path path = file->name;
		path.replace_extension("");

		Utf8 name  = path.string();
		Utf8 title = name;

		int titleLevel = 0;
		while (title.length() > 4 && SWAG_IS_DIGIT(title[0]) && SWAG_IS_DIGIT(title[1]) && SWAG_IS_DIGIT(title[2]) && title[3] == '_')
		{
			title.remove(0, 4);
			titleLevel++;
		}

		title.buffer[0] = static_cast<char>(toupper(title.buffer[0]));
		title.replace("_", " ");

		addTocTitle(name, title, titleLevel);

		helpContent += "\n";
		helpContent += FMT("<h%d id=\"%s\">", titleLevel + 1, getTocTitleRef().c_str());
		helpContent += title;
		helpContent += FMT("</h%d>", titleLevel + 1);

		if (file->hasFlag(FILE_MARK_DOWN))
		{
			if (!processMarkDownFile(file->path, titleLevel + 1))
				return false;
		}
		else
		{
			if (!processSourceFile(file->path, titleLevel + 1))
				return false;
		}
	}

	helpToc += "</ul>\n";
	return true;
}
