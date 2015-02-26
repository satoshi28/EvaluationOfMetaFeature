#ifndef PATTERN_DETECTOR
#define PATTERN_DETECTOR

#include "ExtractFeatures.h"
#include "Matching.h"

class PatternDetector
{
public:
	PatternDetector(std::vector<Pattern>& trainPatterns);
	~PatternDetector();

	/* ���̌��o */
	void findPattern(cv::Mat queryImage, std::vector<int>& ranking );

private:
	Matching m_matching;
};


#endif