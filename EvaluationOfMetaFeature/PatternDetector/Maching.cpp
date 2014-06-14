#include "Matching.h"

Matching::Matching()
{
}


void Matching::getMatches(const Pattern queryPattern, std::vector<int>& matchingList)
{
	std::vector< std::pair<int, int> > imageRankingList(dataSetSize);	//�e�摜�̃����L���O(rank, index)

	for(int i = 0; i < dataSetSize; i++)
	{
		//�}�b�`���O�����y�A
		std::vector<cv::DMatch> matches;
		// Get matches
		match( queryPattern.keypoints , queryPattern.descriptors,m_matchers[i], matches);
		
		//�]��
		imageRankingList[i].first = matches.size();
		imageRankingList[i].second = i;
	}

	//�摜�̃����L���O�Ɋ�Â��č~���ɕ��ёւ�
	std::sort(imageRankingList.begin(), imageRankingList.end(),std::greater<std::pair<int, int>>() );

	//3�ʂ܂ŕԂ�
	for(int i = 0; i < 3; i++)
	{
		matchingList.push_back(imageRankingList[i].second);
	}

}


void Matching::train(const std::vector<Pattern> trainPatterns )
{

	dataSetSize = trainPatterns.size();

	for(int i= 0; i < trainPatterns.size(); i++)
	{
		cv::Ptr<cv::DescriptorMatcher>   matcher   = cv::DescriptorMatcher::create(matcherName);
		m_matchers.push_back(matcher);

		// API of cv::DescriptorMatcher is somewhat tricky
		// First we clear old train data:
		m_matchers[i]->clear();
	}

	std::vector<cv::Mat> descriptors(1);

	for(int i = 0; i < trainPatterns.size(); i++)
	{
		// Then we add vector of descriptors (each descriptors matrix describe one image). 
		// This allows us to perform search across multiple images:
		descriptors[0] = trainPatterns[i].descriptors.clone();
		m_matchers[i]->add(descriptors);

		// After adding train data perform actual train:
		m_matchers[i]->train();
	}

	
	
}



void Matching::match(std::vector<cv::KeyPoint> queryKeypoints,cv::Mat queryDescriptors, cv::Ptr<cv::DescriptorMatcher>& m_matcher,
				std::vector<cv::DMatch>& matches)
{
	const float minRatio = 0.8f;
	matches.clear();

	//�ŋߖT�_�̒T��

	//knn�}�b�`���O
	std::vector< std::vector<cv::DMatch>>  knnMatches;

	// query��matcher�ɕۑ�����Ă�������ʂ�knn�\���̂�p���čŋߖT�_����������.
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

			//�����̔䂪1.5�ȉ��̓��������ۑ�
			if(distanceRatio < minRatio)
			{
				matches.push_back(bestMatch);
			}
		}
	}
	/*
	//�􉽊w�I�������`�F�b�N
	bool passFlag = geometricConsistencyCheck(queryKeypoints, trainKeypoints[i], correctMatches);

	//�􉽊w�I�������`�F�b�N�ɒʉ߂������̂̂ݓo�^����
	if(passFlag == true){
		//�v�f�̈ڂ��ւ�
		for(int k = 0; k < correctMatches.size(); k++)
		{
			matches.push_back(correctMatches[k]);
		}
	}

	//������
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

	//�􉽊w�I�������`�F�b�N
	std::vector<unsigned char> inliersMask(queryPoints.size() );

	//�􉽊w�I�������`�F�b�N�ɂ���ē�����l�𒊏o
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