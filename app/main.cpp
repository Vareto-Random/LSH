#include <random>
#include <iostream>
#include <sstream>
using namespace std;

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;

#include "util.h"

int main(int argc, char *argv[])
{
    Mat dataset;
    vector<pair<string, float>> scores;

    /*
     * Feature descriptors
     */
    cout << argv[0] << endl;
    for (string &path : ls_files("data")) {
        cout << path << endl;
        Mat image, descriptor;
        image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
        //cv::imshow("image", image);
        //cv::waitKey(0);

        image.reshape(1, 1).convertTo(descriptor, CV_32F);

        scores.push_back({path, 0});
        dataset.push_back(descriptor);
    }

    /*
     * Generate hash functions
     */
    Mat_<float> hash(dataset.cols, 100);
    default_random_engine gen;
    normal_distribution<float> dist(0, 1);
    for (float &v : hash)
        v = dist(gen);

    /*
     * Index dataset samples
     */
    dataset = dataset * hash;
    for (float &v : (cv::Mat_<float>) dataset)
        v = v > 0 ? 1 : 0;
    dataset.convertTo(dataset, CV_8U);

    /*
     * Query
     */
    HTML html("result.html");
    for (string &path : ls_files("../data/query")) {

        Mat image, query;
        image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

        image.reshape(1, 1).convertTo(query, CV_32F);

        query = query * hash;
        for (float &v : (cv::Mat_<float>) query)
            v = v > 0 ? 1 : 0;
        query.convertTo(query, CV_8U);

        // search  for similar samples in the dataset
        for (int row = 0; row < dataset.rows; ++row)
            scores[row].second = norm(query, dataset.row(row), NORM_HAMMING);

        // make a copy of scores and rank them
        vector<pair<string, float>> rank;
        rank.assign(scores.begin(), scores.end());
        sort(rank.begin(), rank.end(), [](pair<string, float> &a, pair<string, float> &b) {
            return a.second < b.second;
        });

        html.write(path, rank);
    }


    return EXIT_SUCCESS;
}
