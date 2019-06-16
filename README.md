# CThreadPoolPlus
Linux 高性能线程池

1. 简介

    CThreadPoolPlus为Linux下使用单例模式和cas_queue队列开发的线程池，cas_queue队列是本人另一个开源项目，本线程池项目支持每秒千万级别调度、
动态开辟线程、配置线程池数量功能。

2. 使用

    1）使用例子详见example。
    
    2）如何配置线程池数量：在可执行程序运行路径下创建tq.conf，第一行配置线程池数量、第二行配置线程调度队列数量。
