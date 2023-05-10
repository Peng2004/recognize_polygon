# recognize_polygon(校内赛视觉项目报告)

#### Zealous Team 彭绮钰

+++

### 目录

#### 一、文件说明

#### 二、代码讲解

#### 三、编写思路

+++



### 一、文件说明

 1、代码部分共有三个文件，分别为include/recognize_polygon.h，为头文件；src/recognize_polygon.cpp，为大部分主要的源文件；main.cpp，为main函数的主文件。

2、.vscode文件里放置环境配置的相关文件，此外还有一个CMakeLists.txt。

### 二、代码讲解

#### 1、设置类

首先设置了一个`RecognizePolygon`类用于封装代码

#### 2、定义变量用于追踪条的控制

定义了一个`TrackParameters`结构体，用于后面的追踪条

~~~c++
struct TrackParameters
{
    Mat img1;          
    Mat img2;          
    int thresholdValue; 
    int val[10];
    vector<vector<Point>> polygons;
};
TrackParameters track;
~~~

其中`img1`与`img2`分别传入的图片与最后得出的图片，`thresholdValue`与`val[10]`均为需要调的阈值（下文会一一介绍），`polygons`用于储存边数信息的容器。

#### 3、阈值化处理

主要用到的函数

~~~c++
    threshold(track.img1, mask, track.thresholdValue, 255, THRESH_BINARY_INV);
    track.img1.copyTo(dst);
    bitwise_not(mask, mask);
    dst.setTo(Scalar(0, 0, 0), mask);
~~~

其中track.thresholdValue即需要滑动条调整的数据，首先将 图像进行阈值处理，存储在 `mask` 变量中，然后按位取反函数 `bitwise_not` 对 `mask` 进行取反最后使用 `setTo` 将 `dst` 中，`mask` 对应的位置像素赋值为全黑色。

##### 该步的目的主要为将原图像中亮度较高，从而凸显出物体的轮廓。

#### 4、执行floodFill

~~~c++
 Point seedPoint(1, 1);
    Scalar newVal(255, 255, 255);
    int loDiff = 20;
    int upDiff = 20;
    floodFill(dst, seedPoint, newVal, 0, Scalar::all(loDiff), Scalar::all(upDiff));
~~~

首先定义 `seedPoint`，为需填充的像素值，然后定义 `newVal`，表示填充白色，接着定义loDiff` 和 `upDiff`，是阈值的下限和上限，最后使用floodFill` 函数，对图像 进行填充。

##### 该步目的是将背景颜色填充为白色，减少因背景颜色相近，而`cvtColor`时出现误差。

#### 5、得到轮廓信息

~~~c++
    cvtColor(dst, dst, COLOR_BGR2GRAY);
    morphologyEx(dst, dst, MORPH_CLOSE, element);
    inRange(dst, Scalar(0, 0, 0), Scalar(254, 254, 254), dst);
    Canny(dst, dst, 75, 150);
    dilate(dst, track.img2, element2);
~~~

首先将图像 `dst` 转换为灰度图像，然后进行闭运算操作，用于去除噪点，接着用 `inRange` 函数将像素值在 0~254 之间的像素设为白色(255)，即排除灰度深浅不一带来的影响，将白色背景以外的所有点变为白色，背景改为黑色，然后用`Canny` 进行边缘检测，最后膨胀，增强轮廓。

##### 该步目的是得到轮廓信息，用于识别。

#### 6、筛选轮廓

~~~c++
findContours(track.img2, polygonsBegin, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
    track.polygons.clear(); 
    for (int i = 0; i < polygonsBegin.size(); i++)
    {
        Rect rect = boundingRect(polygonsBegin[i]); 
        int area = contourArea(polygonsBegin[i]);   
        if (area > 600)                             
        {
            vector<Point> result;
            if (area > 500 && area < 1000)
                approxPolyDP(polygonsBegin[i], result, track.val[0], true);
            else if (area > 1000 && area < 1500)
                approxPolyDP(polygonsBegin[i], result, track.val[1], true);
				…………
			else if (area > 20000)
                approxPolyDP(polygonsBegin[i], result, track.val[9], true);
            track.polygons.push_back(result); 
            vector<Point> foll = {rect.tl(), {rect.tl().x + 50, rect.tl().y}, {rect.tl().x + 50, rect.tl().y - 30}, {rect.tl().x, rect.tl().y - 30}};
            fillConvexPoly(track.img2, foll, Scalar(0, 0, 0));
            putText(track.img2, "[" + to_string(area) + "]" + to_string(result.size()), rect.tl(), 2, 1, Scalar(255, 255, 255));
        }
    }

    // 显示处理结果
    imshow("Threshold Adjust", track.img2);
~~~

首先进行轮廓检测，并存储在 `polygonsBegin` 中，然后获取轮廓的面积，去除面积过小的轮廓，然后利用 `approxPolyDP` 对每个轮廓进行多边形拟合，得到多边形轮廓信息，并加入到 `track.polygons` 容器中，在拟合多边形的过程中，使用不同的精度参数对不同大小的轮廓进行拟合，以获得更加准确的结果。

##### val[]即需要滑动条调整的数据。

最后，使用 `fillConvexPoly` 和 `putText` 在处理图像中标出多边形的顶点个数和面积，便于调试。

##### 该步的目的是对目标进行拟合，并将拟合的多边形轮廓信息存储在 `track.polygons` 容器中，以便进行后续的目标跟踪和识别工作。同时，在处理图像中标出多边形顶点和面积信息，方便调试。最后，通过 `imshow` 函数显示处理结果。

#### 7、第3～6步均存在于追踪条回调函数`void trackThreshold(int val, void *)`中

#### 8、调用滑动条识别和拟合多边形

~~~c++
void RecognizePolygon::find()
{
    namedWindow("Threshold Adjust", WINDOW_AUTOSIZE);
    namedWindow("ApproxPolyDP Adjust", WINDOW_AUTOSIZE);
    track.img1 = img;
    track.thresholdValue = 200;
    track.val[0] = 4;
    track.val[1] = 5;
   	…………
    track.val[9] = 15;
    createTrackbar("Threshold:", "ApproxPolyDP Adjust", &track.thresholdValue, 255, trackThreshold);
    createTrackbar("<1000", "ApproxPolyDP Adjust", &track.val[0], 20, trackThreshold);
    createTrackbar("<1500", "ApproxPolyDP Adjust", &track.val[1], 20, trackThreshold);
   	…………
    createTrackbar(">20000:", "ApproxPolyDP Adjust", &track.val[9], 20, trackThreshold);
    trackThreshold(0, NULL);
    waitKey(0);
    results = track.polygons;
}

~~~

主要是用 `createTrackbar`创建用于调整拟合参数的滑块条，以便实时调整二值化阈值和拟合精度参数。将结果保存到 `results` 中。

#### 9、据多边形的边数确定多边形的形状类型

~~~c++
string RecognizePolygon::getShapeType(int numSides)
{
    string type;
    switch (numSides)
    {
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
~~~

#### 10、识别结果展示

~~~c++
int counts[4] = {0};
    for (int i = 0; i < results.size(); i++)
    {
        Rect rect = boundingRect(results[i]);
        rectangle(img, rect, Scalar(255, 255, 255), 2, 8, 0);
        string type = getShapeType(results[i].size());
        putText(img, type, rect.tl(), 2, 1, Scalar(255, 255, 255));
        if (results[i].size() == 6 || results[i].size() == 7 || results[i].size() == 8)
        {
            counts[results[i].size() - 6]++;
        }
        else
        {
            counts[3]++;
        }
    }
~~~

定义长为 4 的数组统计多边形类型的数目，并用`rectangle`和`putText`将结果打印到图片上，接着循环判断当前多边形的类型，记录在数组中。

#### 11、统计输出

~~~c++
 for (int j = 0; j < 4; j++)
    {
        if (counts[j] > 0)
        {
            string type = getShapeType(j + 6);
            cout << type << ": " << counts[j] << endl;
        }
    }
~~~

#### 12、剩余部分则为对函数的调用以及运行，不再详细写



### 三、编写思路

1、本人试过很多种方法对轮廓进行预处理，包括中值滤波；`split`成三个通道分别识别再合成，以及二值化等等，最后上述图像预处理的方法，是在本人尝试过的方法中轮廓最清晰的一种。

2、即使是用上述方法，依旧会毁坏部分轮廓信息，于是想出了在预处理以外的地方下功夫，即调整拟合多边形的精度，但是一个精度并不足以拟合所有多边形，考虑到多边形大小不一，所以根据面积划分了多个精度，并一一调式，初见成效。

3、但是这也并不能保证完全识别成功，于是想出了运用滑动条，实时根据识别效果调整精度，并且可以根据多边形的面积对应调整，这样基本能保证完全识别成功。

4、优点：在预处理的情况下，需要调整精度的多边形一般只有一两个，大大提升效率；又有滑动条保证其能调整到比较合适的精度，大大提高正确率。

5、缺点：我认为在需要调参数这一点还是会拖慢速度，但是本人至今没有想到如何用一套参数快速地识别得到结果。
