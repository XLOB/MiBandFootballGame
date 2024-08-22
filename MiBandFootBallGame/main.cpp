#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <time.h>


//深色5条，浅色4条。5*36，4*39。足球34px 在1/4屏位置。发球圈R=44球。球门宽为5*34px.

/*  一些小记录罢了
    1.在这个项目中，碰撞检测函数中的变量option是活雷锋（私人变量）
 */

double PI = 3.14159265358979323846; // 圆周率


// 实体碰撞通用函数
template<typename T1, typename T2>
bool checkCollision(const T1& a, const T2& b) {
    sf::FloatRect box1 = a.getGlobalBounds();
    sf::FloatRect box2 = b.getGlobalBounds();
    return box1.intersects(box2);
}
// 已知点&&实体的碰撞通用函数
template<typename Tbox>
bool checkCollision(const sf::Vector2f a, const Tbox& b, bool option) {
    sf::FloatRect box = b.getGlobalBounds();
    return box.contains(a);
}
template<typename Tbox>
bool checkCollision(const sf::Vector2i a, const Tbox& b, bool option) {
    sf::FloatRect box = b.getGlobalBounds();
    return box.contains((sf::Vector2f)a);
}
// 实体转换到点&&实体的碰撞通用函数
template<typename Tbox>
bool checkCollision(const Tbox& a, const Tbox& b, double option) {
    sf::Vector2f point = a.getPosition();
    sf::FloatRect box = b.getGlobalBounds();
    return box.contains(point);
}



// 计算角度通用函数
template<typename T1, typename T2>
float angleCalculate(const T1& x, const T2& y) {
    return (float)atan2(y, x) * 180 / PI;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {

    // 初始值初始化
    srand((unsigned)time(NULL));    // 随机数种子
    bool isMousePressed = false;
    bool hadMousePressed = false;
    bool scoreAdded = false;
    bool isGamePause = false;
    bool isTimed = false;
    int clickTimes = 0;
    float timeSetted = 0.f;
    sf::Vector2f latestMousePosition;
    sf::Vector2f intersection; // 由圆心发射出的射线与圆的 焦点
    sf::Vector2f moveDirection; // 移动方向
    sf::Clock timeClock; // 计时器对象创建 && 开始计时
    float theta = 0; // θ(希腊字母) 鼠标与发球点的偏移角
    float rate = 1; // 小球的移速倍率
    int score = 0; // 分数
    int life = 3; // 生命


    // 4级抗锯齿
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    // 创建窗口
    sf::RenderWindow Window(sf::VideoMode(336, 480), "MiBandGame", sf::Style::Titlebar | sf::Style::Close);


    // 导入纹理
    sf::Texture txBall;
    if (!txBall.loadFromFile("./img/Football.png"))
    {
        //error...
        std::cout << "Load file error." << std::endl;
    }
    txBall.setSmooth(true);

    sf::Texture txPlayGroung;
    if (!txPlayGroung.loadFromFile("./img/PlayGround.png"))
    {
        //error...
        std::cout << "Load file error." << std::endl;
    }

    // 导入字体
    sf::Font font;
    if (!font.loadFromFile("./img/MiSans-Normal.ttf"))
    {
        // error...
    }

    // 创建对象
    sf::RectangleShape playGround(sf::Vector2f(336.f, 480.f));
    playGround.setTexture(&txPlayGroung);

    sf::RectangleShape invisibleColliders(sf::Vector2f(268.f, 412.f));    // 隐形碰撞体
    invisibleColliders.setPosition(34.f, 34.f);

    sf::Sprite ball;
    ball.setTexture(txBall);
    ball.setOrigin(17.f, 17.f); // 设置球的位置
    ball.setPosition(playGround.getSize().x / 2, playGround.getSize().y * 3 / 4);

    sf::RectangleShape greyVFX(sf::Vector2f(336.f, 480.f)); // 灰色特效图
    greyVFX.setFillColor(sf::Color(0, 0, 0, 140));

    sf::CircleShape startPoint(44.f);
    startPoint.setFillColor(sf::Color(0, 0, 0, 0));
    startPoint.setOutlineThickness(3.f);
    startPoint.setOutlineColor(sf::Color::White);
    startPoint.setOrigin(44.f, 44.f);   // 设置发球点的位置
    startPoint.setPosition(playGround.getSize().x / 2, playGround.getSize().y * 3 / 4);

    sf::Text tTip;
    tTip.setFont(font);
    tTip.setString(L"得分");
    tTip.setCharacterSize(24);
    tTip.setOrigin(24.f, 0.f);
    tTip.setPosition(playGround.getSize().x / 2, playGround.getSize().y / 8);
    tTip.setStyle(sf::Text::Bold);
    tTip.setOutlineColor(sf::Color::Black);
    tTip.setOutlineThickness(1.f);

    sf::Text tBigTip;
    tBigTip.setFont(font);
    tBigTip.setCharacterSize(32);
    tBigTip.setStyle(sf::Text::Bold);
    tBigTip.setOrigin(40.f, 16.f);
    tBigTip.setPosition(playGround.getSize().x / 2, playGround.getSize().y / 2);
    tBigTip.setFillColor(sf::Color(210, 210, 0));

     sf::Text tScore;
    tScore.setFont(font);
    tScore.setString(sf::String::String(std::to_string(score)));
    tScore.setCharacterSize(32);
    tScore.setOrigin(16.f, 0.f);
    tScore.setPosition(playGround.getSize().x / 2, playGround.getSize().y / 5);
    tScore.setStyle(sf::Text::Bold);
    tScore.setFillColor(sf::Color(210, 210, 0));

    // sf::RectangleShape goal(sf::Vector2f(3.f, 4*34.f));
    sf::ConvexShape goal;
    goal.setPointCount(8);
    goal.setPoint(0, sf::Vector2f(0.f, 0.f));
    goal.setPoint(1, sf::Vector2f(5.f, 0.f));
    goal.setPoint(2, sf::Vector2f(5.f, 2.5));
    goal.setPoint(3, sf::Vector2f(2.5, 2.5));
    goal.setPoint(4, sf::Vector2f(2.5, 117.5));
    goal.setPoint(5, sf::Vector2f(5.f, 117.5));
    goal.setPoint(6, sf::Vector2f(5.f, 120.f));
    goal.setPoint(7, sf::Vector2f(0.f, 120.f));
    if(rand() / double(RAND_MAX) > 0.5)
    {   // 右
        goal.setScale(-1.f, 1.f);
        goal.setPosition(336.f, rand() % (240 + 1));
    }
    else
    {   // 左
        goal.setScale(1.f, 1.f);
        goal.setPosition(0.f, rand() % (240 + 1));
    }

    sf::Sprite life1;
    life1.setTexture(txBall);
    life1.setOrigin(17.f, 17.f); // 设置球的位置
    life1.setPosition(playGround.getSize().x * 1 / 4, playGround.getSize().y * 10 / 11);
    life1.setScale(0.5, 0.5);

    sf::Sprite life2;
    life2.setTexture(txBall);
    life2.setOrigin(17.f, 17.f); // 设置球的位置
    life2.setPosition(playGround.getSize().x / 2, playGround.getSize().y * 10 / 11);
    life2.setScale(0.5, 0.5);

    sf::Sprite life3;
    life3.setTexture(txBall);
    life3.setOrigin(17.f, 17.f); // 设置球的位置
    life3.setPosition(playGround.getSize().x * 3 / 4, playGround.getSize().y * 10 / 11);
    life3.setScale(0.5, 0.5);

    // #####创建测试用线(足球与发球点连线)
    /*
    sf::Vertex testLine[] =
    {
        sf::Vertex(sf::Vector2f((float)playGround.getSize().x / 2, (float)playGround.getSize().y * 3 / 4)),
        sf::Vertex(sf::Vector2f((float)playGround.getSize().x / 2, (float)playGround.getSize().y * 3 / 4))
    };
    */

    // Start the game loop
    while (Window.isOpen())
    {
        // Process events
        sf::Event event;
        while (Window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                Window.close();
         
            // 检测鼠标左键是否被按下
            if (event.type == sf::Event::MouseButtonPressed)
            {
                // 游戏暂停阶段等待用户操作
                if (isGamePause)
                {
                    // 球门随机化
                    if (rand() / double(RAND_MAX) > 0.5)
                    {   // 右
                        goal.setScale(-1.f, 1.f);
                        goal.setPosition(336.f, rand() % (240 + 1));
                    }
                    else
                    {   // 左
                        goal.setScale(1.f, 1.f);
                        goal.setPosition(0.f, rand() % (240 + 1));
                    }

                    ball.setPosition(playGround.getSize().x / 2, playGround.getSize().y * 3 / 4);
                    scoreAdded = false;
                    clickTimes = 0;
                    isGamePause = false;
                    hadMousePressed = false;
                    if (isTimed)
                    {
                        isTimed = false;
                    }
                    if (life == 0)
                    {
                        score = 0;
                        tScore.setOrigin(sf::Vector2f(8.f, 0.f));

                        tScore.setString(std::to_string(score));
                        life = 3;
                        life1.setColor(sf::Color::White);
                        life2.setColor(sf::Color::White);
                        life3.setColor(sf::Color::White);
                    }
                    continue;
                }

                // 鼠标点到足球则进入蓄力阶段
                if (event.mouseButton.button == sf::Mouse::Left \
                    && checkCollision(sf::Mouse::getPosition(Window), ball, true) \
                    && (hadMousePressed == 0)) // <- 加这一条是因为有一个罕见的Bug，足球发射出去后可被“抓”回来
                {
                // 设置鼠标左键按下的bool值
                    isMousePressed = true;
                    scoreAdded = false;
                    clickTimes++;
                }
            }

            // 检测鼠标左键是否被松开
            if (event.type == sf::Event::MouseButtonReleased)
            {
                // 设置鼠标左键松开的bool值
                isMousePressed = false;
            }

        }

        // 根据bool值判断鼠标左键行为
        if ((isMousePressed == 0) && (hadMousePressed == 1)) // 鼠标松开 且 点过鼠标  
        {
            if (!isTimed)
            {
                timeSetted = timeClock.getElapsedTime().asSeconds();
                isTimed = true;
            }
            // 生命处理
            switch (life)
            {
            case 1:
                life2.setColor(sf::Color(140.f, 140.f, 140.f));
            case 2:
                life3.setColor(sf::Color(140.f, 140.f, 140.f));
            case 3:
                break;
            case 0:
                life1.setColor(sf::Color(140.f, 140.f, 140.f));
                isGamePause = true;
                // 游戏暂停 && 等待用户操作
                {  // 渲染操作
                    Window.clear();
                    Window.draw(playGround);
                    Window.draw(startPoint);
                    Window.draw(goal);
                    Window.draw(ball);

                    Window.draw(life1);
                    Window.draw(life2);
                    Window.draw(life3);
                    Window.draw(greyVFX);
                    tBigTip.setString(L"Over!");
                    Window.draw(tBigTip);
                    Window.draw(tTip);
                    Window.draw(tScore);
                    Window.display();
                }
                clickTimes = 0;
                continue;
            default:
                break;
            }
            // 超时处理
            if ((timeClock.getElapsedTime().asSeconds() - timeSetted) >= 4.5f)
            {
                // 游戏暂停 && 等待用户操作
                {   // 渲染操作
                    Window.clear();
                    Window.draw(playGround);
                    Window.draw(startPoint);
                    Window.draw(tTip);
                    Window.draw(tScore);
                    Window.draw(goal);
                    Window.draw(life1);
                    Window.draw(life2);
                    Window.draw(life3);

                    Window.draw(ball);

                    Window.draw(greyVFX);
                    tBigTip.setString(L"Again!");
                    Window.draw(tBigTip);
                    Window.display();
                }
                // 超时扣生命
                if (!isGamePause)
                {
                    life--;
                }
                isGamePause = true;
                clickTimes = 0;
                continue;
            }
            // 球门碰撞处理
            if (checkCollision(ball, goal))
            {
                if (scoreAdded == false)
                {
                    // 分数增加
                    ++score;

                    int count = 1 + log10(score);   // 自动对齐
                    tScore.setOrigin(sf::Vector2f(count * 8, 0.f));

                    tScore.setString(std::to_string(score));

                    scoreAdded = true;
                    isGamePause = true;
                }
                
                /*
                // 初始值初始化
                bool isMousePressed = false;
                bool hadMousePressed = false;
                bool scoreAdded = false;
                int clickTimes = 0;
                float theta = 0; // θ(希腊字母) 鼠标与发球点的偏移角
                */

                // 游戏暂停 && 等待用户操作
                {  // 渲染操作
                    Window.clear();
                    Window.draw(playGround);
                    Window.draw(startPoint);
                    Window.draw(tTip);
                    Window.draw(tScore);
                    Window.draw(goal);
                    Window.draw(life1);
                    Window.draw(life2);
                    Window.draw(life3);

                    Window.draw(ball);

                    Window.draw(greyVFX);
                    tBigTip.setString(L"Goal!");
                    Window.draw(tBigTip);
                    Window.display();
                }
                clickTimes = 0;
                continue;
            }
            // 墙壁碰撞处理
            if (!checkCollision(ball, invisibleColliders))  
            {
                if (ball.getPosition().x < 18 || ball.getPosition().x > 318)
                    moveDirection.x = -moveDirection.x;
                else
                    moveDirection.y = -moveDirection.y;
            }
            
            ball.move(moveDirection);
        }
        else if (isMousePressed)
        {
            hadMousePressed = true;

            // 检测光标(有时是球)与发球框的碰撞 
            if (checkCollision(sf::Mouse::getPosition(Window), startPoint, true))
            {
                // tScore.setString("Yes");  // 测试用提示语句

                 // 计算球的偏移角(弧度)
                latestMousePosition = (sf::Vector2f)sf::Mouse::getPosition(Window);
                theta = angleCalculate(latestMousePosition.x - 168.f, latestMousePosition.y - 360.f);
                intersection.x = (playGround.getSize().x / 2) + 44.f * cos(theta / 58);         // 这里不知道为什么足球移动的角度与鼠标角度倍率很高
                intersection.y = (playGround.getSize().y * 3 / 4) + 44.f * sin(theta / 58);      // 发现58符合倍率关系，就先将就着用了

                // 计算角度
                moveDirection = sf::Vector2f(-rate * cos(theta / 58), -rate * sin(theta / 58));

                // 检测鼠标位置并移动足球
                ball.setPosition((sf::Vector2f)sf::Mouse::getPosition(Window));
            }
            else
            {
                // tScore.setString("No");  // 测试用提示语句

                // 计算球的偏移角(弧度)
                latestMousePosition = (sf::Vector2f)sf::Mouse::getPosition(Window);
                theta = angleCalculate(latestMousePosition.x - 168.f, latestMousePosition.y - 360.f);
                intersection.x = (playGround.getSize().x / 2) + 44.f * cos(theta / 58);         // 这里不知道为什么足球移动的角度与鼠标角度倍率很高
                intersection.y = (playGround.getSize().y * 3 / 4) + 44.f * sin(theta / 58);      // 发现58符合倍率关系，就先将就着用了

                // 计算角度
                moveDirection = sf::Vector2f(-rate * cos(theta / 58), -rate * sin(theta / 58));

                // 移动足球
                ball.setPosition(intersection);
            }

            // std::log(sf::Mouse::getPosition(Window).x+ sf::Mouse::getPosition(Window).y);

            // #####设置测试线端点
            // testLine[1] = sf::Vertex((sf::Vector2f)sf::Mouse::getPosition(Window));

        }

        // Clear screen
        Window.clear();

        Window.draw(playGround);
        Window.draw(startPoint);
        Window.draw(tTip);
        Window.draw(tScore);
        Window.draw(goal);
        Window.draw(life1);
        Window.draw(life2);
        Window.draw(life3);

        Window.draw(ball);

        // 这是足球与发球点的辅助连线
        // Window.draw(testLine, 2, sf::Lines);

        // Update the window
        Window.display();
    }

	return 0;
}
