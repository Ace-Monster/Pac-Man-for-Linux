# Pac-Man-for-Linux

FZU Linux Assignment

## 简介
Linux终端小游戏，当有4个客户端与服务端创建连接后游戏自动开始。<br>
玩家分为两种：1个怪物（Monster）个3个人类（Player），怪物的游戏目标是杀掉所有的人类，人类的目标是找到所有的豆子。<br>
游戏具有视野系统，每个玩家只能看见视野内的物体（包括豆子和其他玩家），地图信息随时可见。<br>

## 使用方法
在Linux下编译和运行 <br>
分别在Server文件夹和Client文件夹下打开终端，输入编译指令：

`
  $g++ *.cpp -o Server -lpthread
`
<br>
`
  $g++ *.cpp -o Client -lpthread
`

然后先运行服务端程序, Post是运行端口：
`
  $./Server Post
`

然后运行客户端程序
`
  $./Client Post
`

## 注意事项
1、游戏人数到达4人时自动开始<br>
2、游戏客户端接入后直到游戏结束都不可结束进程，否则会导致服务端程序崩溃<br>
3、游戏结束后，客户端会与服务端自动断开连接并结束进程，此时服务端可以继续等待新的游戏开始<br>
4、可以支持远程联机游戏，但该功能目前暂未实现<br>
