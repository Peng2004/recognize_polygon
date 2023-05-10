#include "recognize_polygon.h"

void Run()
{
    // 读取图像并进行识别
    RecognizePolygon polygon;
    polygon.img = imread("/home/peng-ub/4.jpg");
    imshow("SRC",polygon.img);
    polygon.find();
    polygon.show();
    destroyAllWindows;
    // 显示结果
    imshow("Result", polygon.img);
    waitKey(0);
    destroyAllWindows();
}

int main()
{
    Run();
    return 0;
}