#include "Matching.h"

Matching::Matching(cv::Ptr<cv::DescriptorMatcher> matcher)
	: m_matcher(matcher)
{
}


void Matching::getMatches(Pattern& queryPattern, std::vector<int>& matchingList)
{

	//�}�b�`���O�����y�A
	std::vector<cv::DMatch> matches;
	// Get matches
	match( queryPattern.keypoints , queryPattern.descriptors,m_matcher, matches);

	std::vector< std::pair<int, int> > imageRankingList(dataSetSize);	//�e�摜�̃����L���O(rank, index)

	for(int i = 0; i < dataSetSize; i++)
	{
		imageRankingList[i].first = 0;
		imageRankingList[i].second = i;
	}
	
	//�]��
	int num;
	for(int i = 0; i < matches.size(); i++)
	{
		num = matches[i].imgIdx;
		imageRankingList[num].first += 1;
	}


	//�摜�̃����L���O�Ɋ�Â��č~���ɕ��ёւ�
	std::sort(imageRankingList.begin(), imageRankingList.end(),std::greater<std::pair<int, int>>() );

	//3�ʂ܂ŕԂ�
	for(int i = 0; i < 3; i++)
	{
		matchingList.push_back(imageRankingList[i].second);
	}

	//���͉摜�Ƀ}�b�`���O�_��`��
	for(int i =0; i < matches.size(); i++)
	{
		if(matches[i].imgIdx == matchingList[0])
		{
			cv::Point2d point =queryPattern.keypoints[matches[i].queryIdx].pt;
			cv::circle(queryPattern.image, cv::Point(point.x,point.y),
				2, cv::Scalar(20,165,255),-1,CV_AA);
			
		}
	}

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
		if(knnMatches[j].empty() == false )
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