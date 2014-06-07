#include "Matching.h"

Matching::Matching(cv::Ptr<cv::DescriptorMatcher> matcher)
	: m_matcher(matcher)
{
}


int Matching::getMatches(const Pattern queryPattern)
{
	
	//マッチングしたペア
	std::vector<cv::DMatch> matches;
	// Get matches
	match( queryPattern.keypoints , queryPattern.descriptors, matches);

	std::vector<int> rankingList(dataSetSize);
	//全要素の初期化
	std::fill(rankingList.begin(), rankingList.end(), 0);
	int num;
	for(int i = 0; i < matches.size(); i++)
	{
		num = matches[i].imgIdx;
		rankingList[num] += 1;
	}

	int matchedNumber;
	int max = 0;
	for(int i = 0 ; i < rankingList.size();i++)
	{
		if(max < rankingList[i])
		{
			max = rankingList[i];
			matchedNumber = i;
		}
	}
	std::cout << max << std::endl;
	return matchedNumber;

}


void Matching::train(const std::vector<Pattern> trainPatterns )
{
	
	// API of cv::DescriptorMatcher is somewhat tricky
	// First we clear old train data:
	m_matcher->clear();

	dataSetSize = trainPatterns.size();
	std::vector<cv::Mat> descriptors( dataSetSize );

	for(int i = 0; i < trainPatterns.size(); i++)
	{
		// Then we add vector of descriptors (each descriptors matrix describe one image). 
		// This allows us to perform search across multiple images:

		descriptors[i]= trainPatterns[i].descriptors.clone();
	}

	m_matcher->add(descriptors);
	// After adding train data perform actual train:
	m_matcher->train();
	
}



void Matching::match(std::vector<cv::KeyPoint> queryKeypoints,cv::Mat queryDescriptors,
				std::vector<cv::DMatch>& matches)
{
	const float minRatio = 0.8f;
	matches.clear();

	//最近傍点の探索

	//knnマッチング
	std::vector< std::vector<cv::DMatch>>  knnMatches;

	// queryとmatcherに保存されている特徴量をknn構造体を用いて最近傍点を検索する.
	m_matcher->knnMatch(queryDescriptors, knnMatches, 2);

	//
	std::vector<cv::DMatch> correctMatches;

	//ratio test
	for(int j = 0; j < knnMatches.size(); j++)
	{
		if(knnMatches[j].empty() == false)
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
	bool passFlag = geometricConsistencyCheck(queryKeypoints, trainKeypoints[i], correctMatches);

	//幾何学的整合性チェックに通過したもののみ登録する
	if(passFlag == true){
		//要素の移し替え
		for(int k = 0; k < correctMatches.size(); k++)
		{
			matches.push_back(correctMatches[k]);
		}
	}

	//初期化
	knnMatches.clear();
	correctMatches.clear();
	*/
	

	
}

bool Matching::geometricConsistencyCheck(std::vector<cv::KeyPoint> queryKeypoints, std::vector<cv::KeyPoint> trainKeypoints, std::vector<cv::DMatch>& match)
{
	if(match.size() < 8)
		return false;

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