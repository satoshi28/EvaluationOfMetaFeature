#include "Matching.h"

Matching::Matching()
{
}


void Matching::getMatches(const Pattern queryPattern, std::vector<int>& matchingList)
{

	//マッチングしたペア
	std::vector< std::vector<cv::DMatch>> matches(dataSetSize);
	// Get matches

	for(int i = 0; i < dataSetSize; i++)
	{
		match( queryPattern.descriptors , m_trainPatterns[i].descriptors, matches[i]);
	}
	std::vector< std::pair<int, int> > imageRankingList(dataSetSize);	//各画像のランキング(rank, index)

	//評価
	for(int i = 0; i < dataSetSize; i++)
	{
		imageRankingList[i].first = matches[i].size();
		imageRankingList[i].second = i;
	}

	//画像のランキングに基づいて降順に並び替え
	std::sort(imageRankingList.begin(), imageRankingList.end(),std::greater<std::pair<int, int>>() );

	//3位まで返す
	for(int i = 0; i < 3; i++)
	{
		matchingList.push_back(imageRankingList[i].second);
	}

}


void Matching::train(const std::vector<Pattern> trainPatterns )
{
// API of cv::DescriptorMatcher is somewhat tricky
	

	dataSetSize = trainPatterns.size();

	m_trainPatterns = trainPatterns;

	
}



void Matching::match(cv::Mat queryDescriptors,cv::Mat dbDescriptors, std::vector<cv::DMatch>& matches)
{
	const float minRatio = 0.8f;
	matches.clear();

	//最近傍点の探索
	cv::Ptr<cv::DescriptorMatcher>   matcher   = cv::DescriptorMatcher::create(matcherName);
	matcher->clear();
	matcher->add(queryDescriptors);
	matcher->train();

	//knnマッチング
	std::vector< std::vector<cv::DMatch>>  knnMatches;

	// queryとmatcherに保存されている特徴量をknn構造体を用いて最近傍点を検索する.
	matcher->knnMatch(dbDescriptors, knnMatches, 2);

	//
	std::vector<cv::DMatch> correctMatches;

	//ratio test
	for(int j = 0; j < knnMatches.size(); j++)
	{
		if(knnMatches[j].empty() == false )
		{
			const cv::DMatch& bestMatch = knnMatches[j][0];
			const cv::DMatch& betterMatch = knnMatches[j][1];

			float distanceRatio = bestMatch.distance / betterMatch.distance;

			//距離の比が1.5以下の特徴だけ保存
			if(distanceRatio < minRatio)
			{
				matches.push_back(bestMatch);
			}
		}
	}
	/*
	//幾何学的整合性チェック
	bool passFlag = geometricConsistencyCheck(queryKeypoints, trainKeypoints, correctMatches);

	//幾何学的整合性チェックに通過したもののみ登録する
	if(passFlag == true){
		//要素の移し替え
		for(int k = 0; k < correctMatches.size(); k++)
		{
			matches.push_back(correctMatches[k]);
		}
	}
	*/
	
	

	
}

bool Matching::geometricConsistencyCheck(std::vector<cv::KeyPoint> queryKeypoints, std::vector<cv::KeyPoint> trainKeypoints, std::vector<cv::DMatch>& match)
{
	if(match.size() < 8)
	{
		match.clear();
		return false;
	}
	std::vector<cv::Point2f>  queryPoints, trainPoints; 
	for(int i = 0; i < match.size(); i++)
	{
		queryPoints.push_back(queryKeypoints[match[i].queryIdx].pt);
		trainPoints.push_back(trainKeypoints[match[i].trainIdx].pt);
	}

	//幾何学的整合性チェック
	std::vector<unsigned char> inliersMask(queryPoints.size() );

	//幾何学的整合性チェックによって当たり値を抽出
	cv::findHomography( queryPoints, trainPoints, CV_FM_RANSAC, 10, inliersMask);

	std::vector<cv::DMatch> inliers;
	for(size_t i =0 ; i < inliersMask.size(); i++)
	{
		if(inliersMask[i])
			inliers.push_back(match[i]);
	}

	match.swap(inliers);
	return true;
}