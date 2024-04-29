#include <SFML/Graphics.hpp>
#include <iostream>
#include "TextureManager.h"
#include "Random.h"
#include <fstream>

using namespace std;

static int n = 32;

int* ReadConfig();
void MineMap(vector<vector<int>>& tiles);
void PrintCounter(sf::RenderWindow& window, int num);
void CountMines(sf::RenderWindow& window, vector<vector<int>> tiles, vector<vector<bool>>& isRevealed, vector<vector<int>>& isFlagged);
void PrintBoard(sf::RenderWindow& window, vector<vector<int>> tiles, bool &isLost, bool &won, vector<vector<sf::Sprite>> &boardS, unordered_map<string, sf::Sprite> &spriteMap, int &o);
void Reset(sf::RenderWindow& window, vector<vector<int>>& tiles, bool& isLost, bool& won, vector<vector<sf::Sprite>>& boardS, vector<vector<bool>>& isRevealed, vector<vector<int>>& isFlagged, unordered_map<string, sf::Sprite>& spriteMap, int &o);
void Debug(sf::RenderWindow& window, vector<vector<int>>& tiles, unordered_map<string, sf::Sprite>& spriteMap, vector<vector<int>>& isFlagged);
void ReadBoard(vector<vector<int>>& tiles, int num, int &o);
void CheckWin(sf::RenderWindow& window, vector<vector<int>>& tiles, vector<vector<bool>>& isRevealed, bool& won, unordered_map<string, sf::Sprite>& spriteMap, vector<vector<int>>& isFlagged, int o);
void OpenMultipleTiles(sf::RenderWindow& window, vector<vector<bool>>& isRevealed, vector<vector<int>>& tiles, vector<vector<int>> &isFlagged, unordered_map<string, sf::Sprite > & spriteMap);

int main()
{
    sf::RenderWindow window(sf::VideoMode(ReadConfig()[0] * 32, ReadConfig()[1] * 32 + 88), "Minesweeper!!!");

    sf::Sprite boardSprite(TextureManager::GetTexture("tile_hidden"));
    sf::Sprite smileySprite(TextureManager::GetTexture("face_happy"));
    sf::Sprite debug(TextureManager::GetTexture("debug"));
    sf::Sprite test1(TextureManager::GetTexture("test_1"));
    sf::Sprite test2(TextureManager::GetTexture("test_2"));
    sf::Sprite test3(TextureManager::GetTexture("test_3"));
    sf::Sprite digits(TextureManager::GetTexture("digits"));
    sf::Sprite flag(TextureManager::GetTexture("flag"));
    sf::Sprite mine(TextureManager::GetTexture("mine"));
    sf::RectangleShape shape(sf::Vector2f(ReadConfig()[0] * 32, ReadConfig()[1] * 32 + 88));
    sf::Sprite revealed(TextureManager::GetTexture("tile_revealed"));
    sf::Sprite lost(TextureManager::GetTexture("face_lose"));
    sf::Sprite victory(TextureManager::GetTexture("face_win"));
    unordered_map<string, sf::Sprite> spriteMap;
    spriteMap.emplace("lost", lost);
    spriteMap.emplace("test1", test1);
    spriteMap.emplace("test2", test2);
    spriteMap.emplace("test3", test3);
    spriteMap.emplace("flag", flag);
    spriteMap.emplace("mine", mine);
    spriteMap.emplace("debug", debug);
    spriteMap.emplace("boardSprite", boardSprite);
    spriteMap.emplace("smileySprite", smileySprite);
    spriteMap.emplace("victory", victory);
    spriteMap.emplace("revealed", revealed);

    bool isLost = false, won = false; //***(!isLost)!=won;
    bool temp = false;
    shape.setFillColor(sf::Color::White);

    int num = ReadConfig()[2];
    unsigned int row = ReadConfig()[1];
    unsigned int col = ReadConfig()[0];

    vector<vector<bool>> isEmpty(row, vector<bool>(col, false)); //not same as isRevealed (returns blank tiles)
    vector<vector<bool>> isRevealed(row, vector<bool>(col, false));
    vector<vector<int>> isFlagged(row, vector<int>(col, 0));
    vector<vector<sf::Sprite>> boardS(row, vector<sf::Sprite>(col, boardSprite)); 
    vector<vector<int>> tiles(row, vector<int>(col, 0)); 
    MineMap(tiles); 
     
    int a=num, b=0, c=0, o=0;
    while (window.isOpen())
    {
    int num = ReadConfig()[2]; 
    sf::Event event;
   
    PrintBoard(window, tiles, isLost, won, boardS, spriteMap, o);
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                for (int j = 0; j < ReadConfig()[1]; j++) {
                    for (int i = 0; i < ReadConfig()[0]; i++) {
                        if (event.mouseButton.button == sf::Mouse::Right && !isLost && !won) { //Adds Flags
                            auto bounds = boardS[j][i].getGlobalBounds();
                            if (bounds.contains(mousePos.x, mousePos.y) && !isRevealed[j][i]) {
                                isFlagged[j][i]++;
                                if (isFlagged[j][i] % 2 == 1) 
                                    a--;
                                else
                                    b++;
                            }
                        }
                        if (event.mouseButton.button == sf::Mouse::Left && !isLost && !won) { //Reveals tiles
                            auto bounds = boardS[j][i].getGlobalBounds();
                            if (bounds.contains(mousePos.x, mousePos.y) && isFlagged[j][i] % 2 == 0) {
                                isRevealed[j][i] = true;
                                //OpenMultipleTiles(window, isRevealed, tiles, isFlagged, spriteMap);
                            }
                        }
                    }
                }
            if (event.type == sf::Event::MouseButtonPressed) { //Resets boards using smileys
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    auto bounds = spriteMap["smileySprite"].getGlobalBounds();
                    if (bounds.contains(mousePos.x, mousePos.y)) {
                        won = false;
                        Reset(window, tiles, isLost, won, boardS, isRevealed, isFlagged, spriteMap, o);
                        a = num;
                        b = 0;
                        o = ReadConfig()[2];
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) { //Debug stuff...
                if (event.mouseButton.button == sf::Mouse::Left) {
                    auto bounds = spriteMap["debug"].getGlobalBounds();
                    if (bounds.contains(mousePos.x, mousePos.y)) {
                        temp = !temp;
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) { //Test1 (loads test board 1)
                if (event.mouseButton.button == sf::Mouse::Left) {
                    auto bounds = spriteMap["test1"].getGlobalBounds();
                    if (bounds.contains(mousePos.x, mousePos.y)) {
                        Reset(window, tiles, isLost, won, boardS, isRevealed, isFlagged, spriteMap, o);
                        ReadBoard(tiles, 1, o);
                        a = o;
                        b = 0;
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) { //Test2
                if (event.mouseButton.button == sf::Mouse::Left) {
                    auto bounds = spriteMap["test2"].getGlobalBounds();
                    if (bounds.contains(mousePos.x, mousePos.y)) {
                        Reset(window, tiles, isLost, won, boardS, isRevealed, isFlagged, spriteMap, o);
                        ReadBoard(tiles, 2, o);
                        a = o;
                        b = 0;
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) { //Test3 
                if (event.mouseButton.button == sf::Mouse::Left) {
                    auto bounds = spriteMap["test3"].getGlobalBounds();
                    if (bounds.contains(mousePos.x, mousePos.y)) {
                        Reset(window, tiles, isLost, won, boardS, isRevealed, isFlagged, spriteMap, o);
                        ReadBoard(tiles, 3, o);
                        a = o;
                        b = 0;
                    }
                }
            }
        }
    }
    OpenMultipleTiles(window, isRevealed, tiles, isFlagged, spriteMap); //Sets empty neighbours to revealed
    CheckWin(window, tiles, isRevealed, won, spriteMap, isFlagged, o); //func to check win
    if (won) {
        PrintCounter(window, 0);
    }
    else
        PrintCounter(window, a + b);
    if (isRevealed[0][0]) {
        window.draw(spriteMap["revealed"]);
    }
    for (int j = 0; j < ReadConfig()[1]; j++) { //Prints revealed tiles
        for (int i = 0; i < ReadConfig()[0]; i++) {
            if (isFlagged[j][i] % 2 == 0 && isRevealed[j][i]) {
                spriteMap["revealed"].setPosition(i * n, j * n);
                window.draw(spriteMap["revealed"]);
            }
        }
    }
    for (int j = 0; j < ReadConfig()[1]; j++) { //Prints Flag
        for (int i = 0; i < ReadConfig()[0]; i++) {
            if (isFlagged[j][i] % 2 == 1) {
                spriteMap["flag"].setPosition(32 * i, 32 * j);
                window.draw(spriteMap["flag"]);
            }
        }
    }  
    for (int j = 0; j < ReadConfig()[1]; j++) { //Defeat Condition
        for (int i = 0; i < ReadConfig()[0]; i++) {
            if (tiles[j][i] == 1 && isRevealed[j][i]) {
                spriteMap["mine"].setPosition(32 * i, 32 * j);
                window.draw(spriteMap["mine"]);
                isLost = true;
            }
            if (isLost && tiles[j][i] == 1) {
                spriteMap["revealed"].setPosition(32 * i, 32 * j);
                window.draw(spriteMap["revealed"]);
                if (isFlagged[j][i] % 2 == 1) {
                    spriteMap["flag"].setPosition(32 * i, 32 * j);
                    window.draw(spriteMap["flag"]);
                }
                spriteMap["mine"].setPosition(32 * i, 32 * j);
                window.draw(spriteMap["mine"]);
            }
        }
    }
    if (temp) { //Draws/Erases mines (Debug)
        Debug(window, tiles, spriteMap, isFlagged);
    }
    CountMines(window, tiles, isRevealed, isFlagged);
    window.display();
}
    TextureManager::Clear(); 
    return 0;
} 

int* ReadConfig() {
    int arr[3];
    int col = 0, row = 0, mines = 0;
    ifstream FileName("boards/config.cfg");
    if (FileName.is_open()) {
        FileName >> col;
        arr[0] = col;
        FileName >> row;
        arr[1] = row;
        FileName >> mines;
        arr[2] = mines;
    }
    else cout << "fail";
    return arr;
}
void MineMap(vector<vector<int>>& tiles) {
    //cout << ReadConfig()[2] << endl;
    unsigned int row = ReadConfig()[1];
    unsigned int col = ReadConfig()[0];
    for (int i = 0; i < ReadConfig()[2]; i++) {
        int x = Random::Int(0, row - 1);
        int y = Random::Int(0, col - 1);
        if (tiles[x][y] == 0) {
            tiles[x][y] = 1;
        }
        else {
            while (tiles[x][y] != 0) {
                x = Random::Int(0, row - 1);
                y = Random::Int(0, col - 1);
            }
            tiles[x][y] = 1;
        }
    }
}
void PrintCounter(sf::RenderWindow& window, int num) {
    sf::Sprite digits(TextureManager::GetTexture("digits"));
    bool isNegative = false;
    if (num < 0) {
        num *= -1;
        isNegative = true;
    }
    int a, b, c;
    a = num % 10;
    num /= 10;
    b = num % 10;
    num /= 10;
    c = num % 10;
    if (isNegative) {
        digits.setPosition(0, n * ReadConfig()[1]);
        digits.setTextureRect(sf::IntRect(21 * 10, 0, 21, 32));
        window.draw(digits);
    }
    digits.setPosition(21, n * ReadConfig()[1]);
    digits.setTextureRect(sf::IntRect(21 * c, 0, 21, 32));
    window.draw(digits);
    digits.setPosition(21 * 2, n * ReadConfig()[1]);
    digits.setTextureRect(sf::IntRect(21 * b, 0, 21, 32));
    window.draw(digits);
    digits.setPosition(21 * 3, n * ReadConfig()[1]);
    digits.setTextureRect(sf::IntRect(21 * a, 0, 21, 32));
    window.draw(digits);
}
void CountMines(sf::RenderWindow& window, vector<vector<int>> tiles, vector<vector<bool>> &isRevealed, vector<vector<int>>& isFlagged) {
    int count = 0;
    sf::Sprite revealed(TextureManager::GetTexture("tile_revealed"));
    sf::Sprite numbers[8];
    for (int i = 0; i < 8; i++) {
        numbers[i].setTexture(TextureManager::GetTexture("number_" + to_string(i + 1)));
    }
    for (int j = 0; j < ReadConfig()[1]; j++) {
        for (int i = 0; i < ReadConfig()[0]; i++) {
            count = 0;
            if (isRevealed[j][i]) {
                if (tiles[j][i] == 1)
                    continue;
                if ((j + 1 < ReadConfig()[1]) && tiles[j + 1][i] == 1)
                    count++;
                if ((j - 1 >= 0) && tiles[j - 1][i] == 1)
                    count++;
                if ((i + 1 < ReadConfig()[0]) && tiles[j][i + 1] == 1)
                    count++;
                if ((i - 1 >= 0) && tiles[j][i - 1] == 1)
                    count++;
                if ((j + 1 < ReadConfig()[1]) && (i + 1 < ReadConfig()[0]) && tiles[j + 1][i + 1] == 1)
                    count++;
                if ((i - 1 >= 0) && (j - 1 >= 0) && tiles[j - 1][i - 1] == 1)
                    count++;
                if ((j - 1 >= 0) && (i + 1 < ReadConfig()[0]) && tiles[j - 1][i + 1] == 1)
                    count++;
                if ((j + 1 < ReadConfig()[1]) && (i - 1 >= 0) && tiles[j + 1][i - 1] == 1)
                    count++;
                if (count != 0 && isFlagged[j][i] % 2 == 0) {
                    numbers[count - 1].setPosition(i * 32, j * 32);
                    window.draw(numbers[count - 1]);
                    count = 0;
                }
            }
        }
    }
}
void PrintBoard(sf::RenderWindow& window, vector<vector<int>> tiles, bool &isLost, bool &won, vector<vector<sf::Sprite>>& boardS, unordered_map<string, sf::Sprite>& spriteMap, int &o) {
    
    sf::Sprite digits(TextureManager::GetTexture("digits")); 
    sf::RectangleShape shape(sf::Vector2f(ReadConfig()[0] * 32, ReadConfig()[1] * 32 + 88));

        int num = ReadConfig()[2];
        sf::Event event;
        window.draw(shape);
        if (o == 0) {
            o = num;
        }
        for (int j = 0; j < ReadConfig()[1]; j++) {
            for (int i = 0; i < ReadConfig()[0]; i++) {
                boardS[j][i].setPosition(i * n, j * n);
                window.draw(boardS[j][i]);
            }
        }
        spriteMap["smileySprite"].setPosition(n * ((float)ReadConfig()[0] / 2 - 1), n * ReadConfig()[1]);
        spriteMap["lost"].setPosition(n * ((float)ReadConfig()[0] / 2 - 1), n * ReadConfig()[1]);
        spriteMap["victory"].setPosition(n * ((float)ReadConfig()[0] / 2 - 1), n * ReadConfig()[1]);
        spriteMap["debug"].setPosition((n * ((float)ReadConfig()[0] / 2 - 1)) + n * 4, n * ReadConfig()[1]);
        spriteMap["test1"].setPosition((n * ((float)ReadConfig()[0] / 2 - 1)) + n * 6, n * ReadConfig()[1]);
        window.draw(spriteMap["test1"]);
        spriteMap["test2"].setPosition((n * ((float)ReadConfig()[0] / 2 - 1)) + n * 8, n * ReadConfig()[1]);
        window.draw(spriteMap["test2"]);
        spriteMap["test3"].setPosition((n * ((float)ReadConfig()[0] / 2 - 1)) + n * 10, n * ReadConfig()[1]);
        window.draw(spriteMap["test3"]);
        if (won)
            window.draw(spriteMap["victory"]);
        else if (!isLost) 
            window.draw(spriteMap["smileySprite"]);
        else
            window.draw(spriteMap["lost"]);
        window.draw(spriteMap["debug"]);
        window.draw(spriteMap["boardSprite"]);
}
void Reset(sf::RenderWindow& window, vector<vector<int>> &tiles, bool& isLost, bool &won, vector<vector<sf::Sprite>>& boardS, vector<vector<bool>> &isRevealed, vector<vector<int>>& isFlagged, unordered_map<string, sf::Sprite>& spriteMap, int &o) {
    isLost = false;
    for (int j = 0; j < ReadConfig()[1]; j++) {
        for (int i = 0; i < ReadConfig()[0]; i++) {
            tiles[j][i] = 0;
        }
    }
    MineMap(tiles);
    for (int j = 0; j < ReadConfig()[1]; j++) {
        for (int i = 0; i < ReadConfig()[0]; i++) {
            boardS[j][i] = spriteMap["boardSprite"];
        }
    }
    for (int j = 0; j < ReadConfig()[1]; j++) {
        for (int i = 0; i < ReadConfig()[0]; i++) {
            isRevealed[j][i] = false;
        }
    }
    for (int j = 0; j < ReadConfig()[1]; j++) {
        for (int i = 0; i < ReadConfig()[0]; i++) {
            isFlagged[j][i] = 0;
        }
    }
    won = false;
    isLost = false;
                        
    PrintBoard(window, tiles, isLost, won, boardS, spriteMap, o);
}
void Debug(sf::RenderWindow& window, vector<vector<int>>& tiles, unordered_map<string, sf::Sprite>& spriteMap, vector<vector<int>>& isFlagged) {
    for (int j = 0; j < ReadConfig()[1]; j++) {
        for (int i = 0; i < ReadConfig()[0]; i++) {
            if (tiles[j][i] == 1) {
                if (isFlagged[j][i] % 2 == 1) {
                    spriteMap["flag"].setPosition(32 * i, 32 * j);
                    window.draw(spriteMap["flag"]);
                }
                spriteMap["mine"].setPosition(32 * i, 32 * j);
                window.draw(spriteMap["mine"]);
            }
        }
    }
}
void ReadBoard(vector<vector<int>>& tiles, int num, int &o) {
    ifstream inFile("boards/testboard"+to_string(num)+".brd");
    vector<vector<char>> chars(ReadConfig()[1] + 1, vector<char>(ReadConfig()[0] + 1, '0'));
    o = 0;
    if (inFile.is_open()) {
        for (int j = 0; j < ReadConfig()[1]; j++) {
            for (int i = 0; i < ReadConfig()[0]; i++) {
                inFile >> chars[j][i];
            }
        }
        for (int j = 0; j < ReadConfig()[1]; j++) {
            for (int i = 0; i < ReadConfig()[0]; i++) {
                tiles[j][i] = chars[j][i] - '0';
                if (tiles[j][i] == 1) {
                    o++;
                }
            }
        }
    }
    else cout << "Failed";
}
void CheckWin(sf::RenderWindow& window, vector<vector<int>>& tiles, vector<vector<bool>>& isRevealed, bool &won, unordered_map<string, sf::Sprite>& spriteMap, vector<vector<int>>& isFlagged, int o) {
    int c = 0;
        ReadConfig()[2] = o;
    for (int j = 0; j < ReadConfig()[1]; j++) { //Victory Condition //add won to print board to print victory smiley
        for (int i = 0; i < ReadConfig()[0]; i++) {
            if (isRevealed[j][i] && tiles[j][i] == 0) {
                c++;
            }
        }
    }
    for (int j = 0; j < ReadConfig()[1]; j++) { 
        for (int i = 0; i < ReadConfig()[0]; i++) {
            if (c == ReadConfig()[1] * ReadConfig()[0] - o && tiles[j][i] == 1) {
                spriteMap["boardSprite"].setPosition(32 * i, 32 * j);
                window.draw(spriteMap["boardSprite"]);
                spriteMap["flag"].setPosition(32 * i, 32 * j);
                window.draw(spriteMap["flag"]);
                won = true;
            }
        }
    }
}
void OpenMultipleTiles(sf::RenderWindow& window, vector<vector<bool>>& isRevealed, vector<vector<int>>& tiles, vector<vector<int>>& isFlagged, unordered_map<string, sf::Sprite >& spriteMap) {
    int huh = 0;
    for (int j = 0; j < ReadConfig()[1]; j++) {
        for (int i = 0; i < ReadConfig()[0]; i++) {
            if (isRevealed[j][i]) {
                if ((j + 1 < ReadConfig()[1]) && tiles[j + 1][i] == 0 && isFlagged[j+1][i]%2==0) {
                    huh++;
                }
                if ((j - 1 >= 0) && tiles[j - 1][i] == 0 && isFlagged[j - 1][i] % 2 == 0) {
                    huh++;
                }
                if ((i + 1 < ReadConfig()[0]) && tiles[j][i + 1] == 0 && isFlagged[j][i+1] % 2 == 0) {
                    huh++;
                }
                if ((i - 1 >= 0) && tiles[j][i - 1] == 0 && isFlagged[j][i-1] % 2 == 0) {
                    huh++;
                }
                if ((j + 1 < ReadConfig()[1]) && (i + 1 < ReadConfig()[0]) && tiles[j + 1][i + 1] == 0 && isFlagged[j + 1][i+1] % 2 == 0) {
                    huh++;
                }
                if ((i - 1 >= 0) && (j - 1 >= 0) && tiles[j - 1][i - 1] == 0 && isFlagged[j - 1][i-1] % 2 == 0) {
                    huh++;
                }
                if ((j - 1 >= 0) && (i + 1 < ReadConfig()[0]) && tiles[j - 1][i + 1] == 0 && isFlagged[j - 1][i+1] % 2 == 0) {
                    huh++;
                }
                if ((j + 1 < ReadConfig()[1]) && (i - 1 >= 0) && tiles[j + 1][i - 1] == 0 && isFlagged[j + 1][i - 1] % 2 == 0) {
                    huh++;
                }
                if (isRevealed[j][i] && ((huh == 8) || (((i == 0 || j == 0||j == ReadConfig()[1]-1 || i == ReadConfig()[0]-1)) && huh == 5)||(((i == 0 && j == 0)|| (i == ReadConfig()[0] - 1 && j == 0) || (i == 0 && j == ReadConfig()[1] - 1) || (i == ReadConfig()[0] - 1 && j == ReadConfig()[1] - 1)) && huh == 3))) {
                    if ((j + 1 < ReadConfig()[1]) && tiles[j + 1][i] == 0 && isFlagged[j + 1][i] % 2 == 0)
                        isRevealed[j + 1][i] = true;
                    if ((j - 1 >= 0) && tiles[j - 1][i] == 0 && isFlagged[j - 1][i] % 2 == 0)
                        isRevealed[j - 1][i] = true;
                    if ((i + 1 < ReadConfig()[0]) && tiles[j][i + 1] == 0 && isFlagged[j][i + 1] % 2 == 0)
                        isRevealed[j][i + 1] = true;
                    if ((i - 1 >= 0) && tiles[j][i - 1] == 0 && isFlagged[j][i - 1] % 2 == 0)
                        isRevealed[j][i - 1] = true;
                    if ((j + 1 < ReadConfig()[1]) && (i + 1 < ReadConfig()[0]) && tiles[j + 1][i + 1] == 0 && isFlagged[j + 1][i + 1] % 2 == 0)
                        isRevealed[j + 1][i + 1] = true;
                    if ((i - 1 >= 0) && (j - 1 >= 0) && tiles[j - 1][i - 1] == 0 && isFlagged[j - 1][i - 1] % 2 == 0)
                        isRevealed[j - 1][i - 1] = true;
                    if ((j - 1 >= 0) && (i + 1 < ReadConfig()[0]) && tiles[j - 1][i + 1] == 0 && isFlagged[j - 1][i + 1] % 2 == 0)
                        isRevealed[j - 1][i + 1] = true;
                    if ((j + 1 < ReadConfig()[1]) && (i - 1 >= 0) && tiles[j + 1][i - 1] == 0 && isFlagged[j + 1][i - 1] % 2 == 0)
                        isRevealed[j + 1][i - 1] = true;
                }
                huh = 0;
            }
        }
    }    
    for (int j = 0; j < ReadConfig()[1]; j++) { 
        for (int i = 0; i < ReadConfig()[0]; i++) {
            if (isRevealed[j][i]) {
                spriteMap["revealed"].setPosition(32 * i, 32 * j);
                window.draw(spriteMap["revealed"]);
            }
        }
    }
}