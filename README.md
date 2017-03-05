# dehaze
The implementations of dark channel prior haze removal and fast haze removal by using Qt and OpenCV.


## Introduction
Fog-degraded images are usually unclear and reduce the visual effects because the air contains a lot of particulate matter and makes the obtained images degrade. For solving the degraded problems including low contrast, weak color etc., I have made a simple program to demonstrate the process of dehaze by using dark channel prior algorithm and fast dehaze algorithm.


## Dehaze Algorithms
- DarkChannel
    
    > Kaiming He, Jian Sun, Xiaoou Tang, Single Image Haze Removal Using Dark Channel Prior, in Proc. IEEE Conf.Computer Vision and Pattern Recognition, 1956-1963, 2009d
    
- Fast Dehaze

    >  刘傅, 陈茂银, 周东华. 基于单幅图像的快速去雾算法. The 25th Chinese Contral and Decision Conference, 2013. 


## Addition
In my implementation, using the __sampling technique__ to faster the processing.


## Result
- ui

    ![image](https://github.com/lwvoid/dehaze/blob/master/screenshots/dehaze.jpg)

- before

    ![image](https://github.com/lwvoid/dehaze/blob/master/screenshots/dehaze_before.jpg)

- after

    ![image](https://github.com/lwvoid/dehaze/blob/master/screenshots/dehaze_after.jpg)
