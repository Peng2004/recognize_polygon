#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

// 声明识别多边形的类
class RecognizePolygon
{
public:
    Mat img;                           // 待处理的图像
    vector<vector<Point>> results;     // 存储识别出的轮廓
    void show();                       // 显示识别结果
    void find_high();
    void find_low(); 
    void find_middle();                        // 查找并识别多边形
    string getShapeType(int numSides); // 根据边数获取多边形类型
};