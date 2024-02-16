#pragma once
#include "Job.h"
#include "Semantic.h"

struct SourceFile;
struct AstNode;

struct SemanticJob final : Job
{
	void release() override;

	bool                spawnJob();
	JobResult           execute() override;
	static SemanticJob* newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* rootNode, bool run);

	SemanticContext context;
};
