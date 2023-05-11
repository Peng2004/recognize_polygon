#include "recognize_polygon.h"

using namespace cv;

void RecognizePolygon::find_low()
{
    vector<vector<Point>> polygonsBegin;
    Mat ker = getStructuringElement(0, Size(3, 3));
    Mat img_copy;
    img.copyTo(img_copy);
    // 将图像灰度化，检测边缘，闭运算
    cvtColor(img_copy, img_copy, COLOR_BGR2GRAY);
    Canny(img_copy, img_copy, 20, 50);
    morphologyEx(img_copy, img_copy, MORPH_CLOSE, ker);
    // 识别轮廓
    findContours(img_copy, polygonsBegin, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
    // 拟合多边形
    for (int j = 0; j < polygonsBegin.size(); j++)
    {
        vector<Point> result;
        double peri = arcLength(polygonsBegin[j], true);
        approxPolyDP(polygonsBegin[j], result, 0.03 * peri, true);
        results.push_back(result);
    }
}

void RecognizePolygon::find_middle()
{
    vector<vector<Point>> polygonsBegin;
    Mat img_copy;
    img.copyTo(img_copy);
    cv::Point seedPoint(1, 1);

    // 设置填充的颜色和阈值
    cv::Scalar newVal(255, 255, 255);
    int loDiff = 20;
    int upDiff = 20;
    // 对图像进行填充
    cv::floodFill(img_copy, seedPoint, newVal, 0, cv::Scalar::all(loDiff), cv::Scalar::all(upDiff));
    Mat ker = getStructuringElement(0, Size(3, 3));
    // 将图像灰度化，检测边缘，闭运算
    cvtColor(img_copy, img_copy, COLOR_BGR2GRAY);
    Canny(img_copy, img_copy, 20, 50);
    morphologyEx(img_copy, img_copy, MORPH_CLOSE, ker);
    // 识别轮廓
    findContours(img_copy, polygonsBegin, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
    // 拟合多边形
    for (int j = 0; j < polygonsBegin.size(); j++)
    {
        vector<Point> result;
        double peri = arcLength(polygonsBegin[j], true);
        approxPolyDP(polygonsBegin[j], result, 0.03 * peri, true);
        results.push_back(result);
    }
}

// 定义变量用于追踪条的控制
struct TrackParameters
{

    Mat img1;           // 用于显示的原图像
    Mat img2;           // 最终图像，在其中识别出多边形
    int thresholdValue; // 追踪条对应的阈值
    int val[10];
    vector<vector<Point>> polygons; // 存储识别出的轮廓
};

TrackParameters track; // 定义追踪条参数

// 追踪条回调函数
void trackThreshold(int val, void *)
{
    // 定义变量
    vector<vector<Point>> polygonsBegin;
    Mat dst, mask;
    vector<vector<Point>> conPoly;
    // 阈值化处理
    threshold(track.img1, mask, track.thresholdValue, 255, THRESH_BINARY_INV);
    track.img1.copyTo(dst);
    bitwise_not(mask, mask);
    dst.setTo(Scalar(0, 0, 0), mask);
    // imshow("setto", dst);
    //  执行floodFill，填充结果保存在dst中
    Point seedPoint(1, 1);
    Scalar newVal(255, 255, 255);
    int loDiff = 20;
    int upDiff = 20;
    floodFill(dst, seedPoint, newVal, 0, Scalar::all(loDiff), Scalar::all(upDiff));
    // 图像预处理
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
    Mat element2 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    cvtColor(dst, dst, COLOR_BGR2GRAY);
    morphologyEx(dst, dst, MORPH_CLOSE, element);
    inRange(dst, Scalar(0, 0, 0), Scalar(254, 254, 254), dst);
    Canny(dst, dst, 75, 150);
    dilate(dst, track.img2, element2);
    // 查找轮廓并进行多边形拟合
    findContours(track.img2, polygonsBegin, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
    track.polygons.clear(); // 清空多边形容器
    for (int i = 0; i < polygonsBegin.size(); i++)
    {
        Rect rect = boundingRect(polygonsBegin[i]); // 获取位置信息
        int area = contourArea(polygonsBegin[i]);   // 计算轮廓面积
        if (area > 600)                             // 筛除过小轮廓
        {
            vector<Point> result;
            if (area > 500 && area < 1000)
                approxPolyDP(polygonsBegin[i], result, track.val[0], true);
            else if (area > 1000 && area < 1500)
                approxPolyDP(polygonsBegin[i], result, track.val[1], true);
            else if (area > 1500 && area < 2000)
                approxPolyDP(polygonsBegin[i], result, track.val[2], true);
            else if (area > 2000 && area < 2500)
                approxPolyDP(polygonsBegin[i], result, track.val[3], true);
            else if (area > 2500 && area < 3000)
                approxPolyDP(polygonsBegin[i], result, track.val[4], true);
            else if (area > 3000 && area < 4000)
                approxPolyDP(polygonsBegin[i], result, track.val[5], true);
            else if (area > 4000 && area < 5000)
                approxPolyDP(polygonsBegin[i], result, track.val[6], true);
            else if (area > 5000 && area < 10000)
                approxPolyDP(polygonsBegin[i], result, track.val[7], true);
            else if (area > 10000 && area < 20000)
                approxPolyDP(polygonsBegin[i], result, track.val[8], true);
            else if (area > 20000)
                approxPolyDP(polygonsBegin[i], result, track.val[9], true);
            track.polygons.push_back(result); // 将拟合出的多边形加入容器中
            // 在处理图像中标出多边形的顶点个数和面积（仅用于调试）
            vector<Point> foll = {rect.tl(), {rect.tl().x + 50, rect.tl().y}, {rect.tl().x + 50, rect.tl().y - 30}, {rect.tl().x, rect.tl().y - 30}};
            fillConvexPoly(track.img2, foll, Scalar(0, 0, 0));
            putText(track.img2, "[" + to_string(area) + "]" + to_string(result.size()), rect.tl(), 2, 1, Scalar(255, 255, 255));
        }
    }

    // 显示处理结果
    imshow("Threshold Adjust", track.img2);
}

void RecognizePolygon::find_high()
{

    // 显示原图并创建追踪条
    namedWindow("Threshold Adjust", WINDOW_AUTOSIZE);
    namedWindow("ApproxPolyDP Adjust", WINDOW_AUTOSIZE);
    track.img1 = img;
    track.thresholdValue = 200;
    track.val[0] = 4;
    track.val[1] = 5;
    track.val[2] = 6;
    track.val[3] = 7;
    track.val[4] = 8;
    track.val[5] = 9;
    track.val[6] = 10;
    track.val[7] = 11;
    track.val[8] = 13;
    track.val[9] = 15;

    createTrackbar("Threshold:", "ApproxPolyDP Adjust", &track.thresholdValue, 255, trackThreshold);
    createTrackbar("<1000", "ApproxPolyDP Adjust", &track.val[0], 20, trackThreshold);
    createTrackbar("<1500", "ApproxPolyDP Adjust", &track.val[1], 20, trackThreshold);
    createTrackbar("<2000", "ApproxPolyDP Adjust", &track.val[2], 20, trackThreshold);
    createTrackbar("<2500", "ApproxPolyDP Adjust", &track.val[3], 20, trackThreshold);
    createTrackbar("<3000:", "ApproxPolyDP Adjust", &track.val[4], 20, trackThreshold);
    createTrackbar("<4000:", "ApproxPolyDP Adjust", &track.val[5], 20, trackThreshold);
    createTrackbar("<5000:", "ApproxPolyDP Adjust", &track.val[6], 20, trackThreshold);
    createTrackbar("<10000:", "ApproxPolyDP Adjust", &track.val[7], 20, trackThreshold);
    createTrackbar("<20000:", "ApproxPolyDP Adjust", &track.val[8], 20, trackThreshold);
    createTrackbar(">20000:", "ApproxPolyDP Adjust", &track.val[9], 20, trackThreshold);
    trackThreshold(0, NULL);
    waitKey(0);
    // 保存结果并在原图中显示
    results = track.polygons;
}

// 根据多边形边数获取多边形类型
string RecognizePolygon::getShapeType(int numSides)
{
    string type;
    switch (numSides)
    {
    case 3:
        type = "Poly-3";
        break;
    case 4:
        type = "Poly-4";
        break;
    case 5:
        type = "Poly-5";
        break;
    case 6:
        type = "Poly-6";
        break;
    case 7:
        type = "Poly-7";
        break;
    case 8:
        type = "Poly-8";
        break;
    default:
        type = "star";
        break;
    }
    return type;
}

void RecognizePolygon::show()
{
    int counts[7] = {0};
    // 显示识别结果并统计多边形类型数目
    for (int i = 0; i < results.size(); i++)
    {
        Rect rect = boundingRect(results[i]);
        rectangle(img, rect, Scalar(255, 255, 255), 2, 8, 0);
        string type = getShapeType(results[i].size());
        putText(img, type, rect.tl(), 2, 1, Scalar(255, 255, 255));
        if (results[i].size() == 6 || results[i].size() == 7 || results[i].size() == 8)
        {
            counts[results[i].size() - 2]++;
        }
        else
        {
            counts[0]++;
        }
    }

    // 显示每种类型的个数
    for (int j = 0; j < 7; j++)
    {
        if (counts[j] > 0)
        {
            string type = getShapeType(j + 2);
            cout << type << ": " << counts[j] << endl;
        }
    }
}