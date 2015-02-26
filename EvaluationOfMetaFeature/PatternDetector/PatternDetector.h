#ifndef PATTERN_DETECTOR
#define PATTERN_DETECTOR

#include "ExtractFeatures.h"
#include "Matching.h"

class PatternDetector
{
public:
	/**
	* @brief ���̂�F�����}�b�`���O���ʂ�n��
	* @param[in] trainPatterns �F������摜�̃p�^�[��
	*/
	PatternDetector(std::vector<Pattern>& trainPatterns);
	~PatternDetector();

	/**
	* @brief ���̂�F�����}�b�`���O���ʂ�n��
	* @param[in] queryImage ���͉摜
	* @param[out] ranking �}�b�`���O����
	*/
	void findPattern(cv::Mat queryImage, std::vector<int>& ranking );

private:
	Matching m_matching;
};


#endif