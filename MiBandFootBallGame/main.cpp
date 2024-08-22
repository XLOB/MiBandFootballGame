#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <time.h>


//��ɫ5����ǳɫ4����5*36��4*39������34px ��1/4��λ�á�����ȦR=44�����ſ�Ϊ5*34px.

/*  һЩС��¼����
    1.�������Ŀ�У���ײ��⺯���еı���option�ǻ��׷棨˽�˱�����
 */

double PI = 3.14159265358979323846; // Բ����


// ʵ����ײͨ�ú���
template<typename T1, typename T2>
bool checkCollision(const T1& a, const T2& b) {
    sf::FloatRect box1 = a.getGlobalBounds();
    sf::FloatRect box2 = b.getGlobalBounds();
    return box1.intersects(box2);
}
// ��֪��&&ʵ�����ײͨ�ú���
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
// ʵ��ת������&&ʵ�����ײͨ�ú���
template<typename Tbox>
bool checkCollision(const Tbox& a, const Tbox& b, double option) {
    sf::Vector2f point = a.getPosition();
    sf::FloatRect box = b.getGlobalBounds();
    return box.contains(point);
}



// ����Ƕ�ͨ�ú���
template<typename T1, typename T2>
float angleCalculate(const T1& x, const T2& y) {
    return (float)atan2(y, x) * 180 / PI;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {

    // ��ʼֵ��ʼ��
    srand((unsigned)time(NULL));    // ���������
    bool isMousePressed = false;
    bool hadMousePressed = false;
    bool scoreAdded = false;
    bool isGamePause = false;
    bool isTimed = false;
    int clickTimes = 0;
    float timeSetted = 0.f;
    sf::Vector2f latestMousePosition;
    sf::Vector2f intersection; // ��Բ�ķ������������Բ�� ����
    sf::Vector2f moveDirection; // �ƶ�����
    sf::Clock timeClock; // ��ʱ�����󴴽� && ��ʼ��ʱ
    float theta = 0; // ��(ϣ����ĸ) ����뷢����ƫ�ƽ�
    float rate = 1; // С������ٱ���
    int score = 0; // ����
    int life = 3; // ����


    // 4�������
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    // ��������
    sf::RenderWindow Window(sf::VideoMode(336, 480), "MiBandGame", sf::Style::Titlebar | sf::Style::Close);


    // ��������
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

    // ��������
    sf::Font font;
    if (!font.loadFromFile("./img/MiSans-Normal.ttf"))
    {
        // error...
    }

    // ��������
    sf::RectangleShape playGround(sf::Vector2f(336.f, 480.f));
    playGround.setTexture(&txPlayGroung);

    sf::RectangleShape invisibleColliders(sf::Vector2f(268.f, 412.f));    // ������ײ��
    invisibleColliders.setPosition(34.f, 34.f);

    sf::Sprite ball;
    ball.setTexture(txBall);
    ball.setOrigin(17.f, 17.f); // �������λ��
    ball.setPosition(playGround.getSize().x / 2, playGround.getSize().y * 3 / 4);

    sf::RectangleShape greyVFX(sf::Vector2f(336.f, 480.f)); // ��ɫ��Чͼ
    greyVFX.setFillColor(sf::Color(0, 0, 0, 140));

    sf::CircleShape startPoint(44.f);
    startPoint.setFillColor(sf::Color(0, 0, 0, 0));
    startPoint.setOutlineThickness(3.f);
    startPoint.setOutlineColor(sf::Color::White);
    startPoint.setOrigin(44.f, 44.f);   // ���÷�����λ��
    startPoint.setPosition(playGround.getSize().x / 2, playGround.getSize().y * 3 / 4);

    sf::Text tTip;
    tTip.setFont(font);
    tTip.setString(L"�÷�");
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
    {   // ��
        goal.setScale(-1.f, 1.f);
        goal.setPosition(336.f, rand() % (240 + 1));
    }
    else
    {   // ��
        goal.setScale(1.f, 1.f);
        goal.setPosition(0.f, rand() % (240 + 1));
    }

    sf::Sprite life1;
    life1.setTexture(txBall);
    life1.setOrigin(17.f, 17.f); // �������λ��
    life1.setPosition(playGround.getSize().x * 1 / 4, playGround.getSize().y * 10 / 11);
    life1.setScale(0.5, 0.5);

    sf::Sprite life2;
    life2.setTexture(txBall);
    life2.setOrigin(17.f, 17.f); // �������λ��
    life2.setPosition(playGround.getSize().x / 2, playGround.getSize().y * 10 / 11);
    life2.setScale(0.5, 0.5);

    sf::Sprite life3;
    life3.setTexture(txBall);
    life3.setOrigin(17.f, 17.f); // �������λ��
    life3.setPosition(playGround.getSize().x * 3 / 4, playGround.getSize().y * 10 / 11);
    life3.setScale(0.5, 0.5);

    // #####������������(�����뷢�������)
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
         
            // ����������Ƿ񱻰���
            if (event.type == sf::Event::MouseButtonPressed)
            {
                // ��Ϸ��ͣ�׶εȴ��û�����
                if (isGamePause)
                {
                    // ���������
                    if (rand() / double(RAND_MAX) > 0.5)
                    {   // ��
                        goal.setScale(-1.f, 1.f);
                        goal.setPosition(336.f, rand() % (240 + 1));
                    }
                    else
                    {   // ��
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

                // ���㵽��������������׶�
                if (event.mouseButton.button == sf::Mouse::Left \
                    && checkCollision(sf::Mouse::getPosition(Window), ball, true) \
                    && (hadMousePressed == 0)) // <- ����һ������Ϊ��һ��������Bug���������ȥ��ɱ���ץ������
                {
                // �������������µ�boolֵ
                    isMousePressed = true;
                    scoreAdded = false;
                    clickTimes++;
                }
            }

            // ����������Ƿ��ɿ�
            if (event.type == sf::Event::MouseButtonReleased)
            {
                // �����������ɿ���boolֵ
                isMousePressed = false;
            }

        }

        // ����boolֵ�ж���������Ϊ
        if ((isMousePressed == 0) && (hadMousePressed == 1)) // ����ɿ� �� ������  
        {
            if (!isTimed)
            {
                timeSetted = timeClock.getElapsedTime().asSeconds();
                isTimed = true;
            }
            // ��������
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
                // ��Ϸ��ͣ && �ȴ��û�����
                {  // ��Ⱦ����
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
            // ��ʱ����
            if ((timeClock.getElapsedTime().asSeconds() - timeSetted) >= 4.5f)
            {
                // ��Ϸ��ͣ && �ȴ��û�����
                {   // ��Ⱦ����
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
                // ��ʱ������
                if (!isGamePause)
                {
                    life--;
                }
                isGamePause = true;
                clickTimes = 0;
                continue;
            }
            // ������ײ����
            if (checkCollision(ball, goal))
            {
                if (scoreAdded == false)
                {
                    // ��������
                    ++score;

                    int count = 1 + log10(score);   // �Զ�����
                    tScore.setOrigin(sf::Vector2f(count * 8, 0.f));

                    tScore.setString(std::to_string(score));

                    scoreAdded = true;
                    isGamePause = true;
                }
                
                /*
                // ��ʼֵ��ʼ��
                bool isMousePressed = false;
                bool hadMousePressed = false;
                bool scoreAdded = false;
                int clickTimes = 0;
                float theta = 0; // ��(ϣ����ĸ) ����뷢����ƫ�ƽ�
                */

                // ��Ϸ��ͣ && �ȴ��û�����
                {  // ��Ⱦ����
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
            // ǽ����ײ����
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

            // �����(��ʱ����)�뷢������ײ 
            if (checkCollision(sf::Mouse::getPosition(Window), startPoint, true))
            {
                // tScore.setString("Yes");  // ��������ʾ���

                 // �������ƫ�ƽ�(����)
                latestMousePosition = (sf::Vector2f)sf::Mouse::getPosition(Window);
                theta = angleCalculate(latestMousePosition.x - 168.f, latestMousePosition.y - 360.f);
                intersection.x = (playGround.getSize().x / 2) + 44.f * cos(theta / 58);         // ���ﲻ֪��Ϊʲô�����ƶ��ĽǶ������Ƕȱ��ʺܸ�
                intersection.y = (playGround.getSize().y * 3 / 4) + 44.f * sin(theta / 58);      // ����58���ϱ��ʹ�ϵ�����Ƚ���������

                // ����Ƕ�
                moveDirection = sf::Vector2f(-rate * cos(theta / 58), -rate * sin(theta / 58));

                // ������λ�ò��ƶ�����
                ball.setPosition((sf::Vector2f)sf::Mouse::getPosition(Window));
            }
            else
            {
                // tScore.setString("No");  // ��������ʾ���

                // �������ƫ�ƽ�(����)
                latestMousePosition = (sf::Vector2f)sf::Mouse::getPosition(Window);
                theta = angleCalculate(latestMousePosition.x - 168.f, latestMousePosition.y - 360.f);
                intersection.x = (playGround.getSize().x / 2) + 44.f * cos(theta / 58);         // ���ﲻ֪��Ϊʲô�����ƶ��ĽǶ������Ƕȱ��ʺܸ�
                intersection.y = (playGround.getSize().y * 3 / 4) + 44.f * sin(theta / 58);      // ����58���ϱ��ʹ�ϵ�����Ƚ���������

                // ����Ƕ�
                moveDirection = sf::Vector2f(-rate * cos(theta / 58), -rate * sin(theta / 58));

                // �ƶ�����
                ball.setPosition(intersection);
            }

            // std::log(sf::Mouse::getPosition(Window).x+ sf::Mouse::getPosition(Window).y);

            // #####���ò����߶˵�
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

        // ���������뷢���ĸ�������
        // Window.draw(testLine, 2, sf::Lines);

        // Update the window
        Window.display();
    }

	return 0;
}
