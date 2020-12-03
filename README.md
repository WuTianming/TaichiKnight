# 混元太极功法
（中国科学技术大学2020级计算机程序设计大作业）

# DEMO
[Link to demo, built with Emscripten.](https://wutianming.github.io/TaichiKnight/)

弹幕射击游戏。

## 内容设定

玩家、小怪、boss

玩家：显示为一个太极球加一个指向箭头。

移动：极坐标移动，鼠标瞄准

攻击：
- 基础武器。参数包含：{射击速度，子弹伤害，霰弹攻击张角}

闪躲、特殊操作：
- “化劲”：在某个音乐拍点响起前，角色会被虚线激光瞄准(visual hint, 打击感)，角色需要在该拍点响起时准确按下【功能键】来扛住激光攻击。根据准确程度，可以给角色的操作打分，并且给予一定的加成，如大招蓄力，反弹，clear
- “闪”：在音乐的一个长音响起时，角色周围会显示一个较大的判定圈。角色需要在长音结束之前迅速移动躲开判定圈

## 分模块编程

角色、怪物的各种信息全部用struct存起来
- main.cpp 主循环，处理按键事件
- audio.h audio.cpp 音响模块，向主循环发送event表示拍点来激活特殊操作
- render.h render.cpp 显示模块，负责将各种struct存储的信息转化为屏幕图像，还负责绘制环境
- player.h player.cpp 把玩家包装成struct
- mon.h mon.cpp 把怪物包装成struct
