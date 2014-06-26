#include "PatternDetector\PatternDetector.h"


PatternDetector::PatternDetector(std::vector<Pattern>& trainPatterns)
{
	//�P���f�[�^���i�[(���̃f�[�^�ɑ΂��}�b�`���O�����)
	m_matching.train(trainPatterns);

}

PatternDetector
::~PatternDetector
()
{
}

void PatternDetector::findPattern(cv::Mat queryImage, std::vector<int>& ranking )
{
	
	ExtractFeatures extract;
	int matchedNumber = 0;
	Pattern queryPattern;

	//�����p
	// �����ʂ�Pattern�ɕۑ�
	extract.getFeatures(queryImage,queryPattern);

	// ���ׂẲ摜���m���}�b�`���O����
	m_matching.getMatches(queryPattern,ranking);
	
}