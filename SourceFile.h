#pragma once
enum ErrorIO
{
	Ok,
	BadFormat,
};

enum TextFormat
{
	Ascii,
	UTF8,
};

struct SourceLocation
{
	int line;
	int column;
	int seek;
};

class SourceFile
{
	friend class LoadingThread;

public:
	SourceFile(const fs::path& path, int bufferSize = 4096);
	~SourceFile();

	unsigned getChar();

private:
	void open();
	void seekTo(long seek);
	long readTo(char* buffer);
	void notifyLoad();
	void close();
	void waitRequest(int reqNum);
	void validateRequest(int reqNum);
	void buildRequest(int reqNum);
	char getPrivateChar();

private:
	ErrorIO m_errorIO = ErrorIO::Ok;
	TextFormat m_textFormat = TextFormat::Ascii;
	int m_bufferSize;
	fs::path m_path;
	FILE* m_file;
	long m_fileSeek = 0;
	long m_bufferCurSeek = 0;
	int  m_bufferCurIndex = 0;
	char* m_buffers[2];
	struct LoadingThreadRequest* m_requests[2];
	long m_buffersSize[2];
	mutex m_mutexNotify;
	bool m_doneLoading = false;
	int m_totalRead = 0;
	condition_variable m_Cv;
};

