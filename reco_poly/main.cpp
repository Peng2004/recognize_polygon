#include "recognize_polygon.h"

void Run_high();
void Run_low();
void Run_middle();

int main()
{
    Run_high();
    //Run_low();
    //Run_middle();
    return 0;
}

void Run_high()
{
    // 读取图像并进行识别
    RecognizePolygon polygon;
    polygon.img = imread("/home/peng-ub/school_robo/high/4.jpg");
    imshow("SRC",polygon.img);
    polygon.find_high();
    polygon.show();
    destroyAllWindows();
    // 显示结果
    imshow("Result", polygon.img);
    waitKey(0);
    destroyAllWindows();
}

void Run_low()
{
    // 读取图像并进行识别
    RecognizePolygon polygon;
    polygon.img = imread("/home/peng-ub/school_robo/low/4.jpg");
    polygon.find_low();
    polygon.show();
    // 显示结果
    imshow("Result", polygon.img);
    waitKey(0);
    destroyAllWindows();
}

void Run_middle()
{
    // 读取图像并进行识别
    RecognizePolygon polygon;
    polygon.img = imread("/home/peng-ub/school_robo/middle/4.jpg");
    polygon.find_middle();
    polygon.show();
    // 显示结果
    imshow("Result", polygon.img);
    waitKey(0);
    destroyAllWindows();
}
