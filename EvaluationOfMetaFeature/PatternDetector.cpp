#include "PatternDetector\PatternDetector.h"


PatternDetector::PatternDetector(std::vector<Pattern>& trainPatterns)
{
	//訓練データを格納(このデータに対しマッチングされる)
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

	//処理用
	// 特徴量をPatternに保存
	extract.getFeatures(queryImage,queryPattern);

	// すべての画像同士をマッチングする
	m_matching.getMatches(queryPattern,ranking);
	
}