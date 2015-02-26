#ifndef PATTERN_DETECTOR
#define PATTERN_DETECTOR

#include "ExtractFeatures.h"
#include "Matching.h"

class PatternDetector
{
public:
	/**
	* @brief 物体を認識しマッチング結果を渡す
	* @param[in] trainPatterns 認識する画像のパターン
	*/
	PatternDetector(std::vector<Pattern>& trainPatterns);
	~PatternDetector();

	/**
	* @brief 物体を認識しマッチング結果を渡す
	* @param[in] queryImage 入力画像
	* @param[out] ranking マッチング結果
	*/
	void findPattern(cv::Mat queryImage, std::vector<int>& ranking );

private:
	Matching m_matching;
};


#endif