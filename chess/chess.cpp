#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "print.h"
#include <iostream>
#include <vector>
using namespace std;

struct v2 {


    float x;
    float y;

    float distance() {
        return sqrt(x * x + y * y);
    }

    v2(int a, int b) {
        x = a;
        y = b;
    }

    v2(float a, float b) {
        x = a;
        y = b;
    }

    v2() {

    }

    v2 makeunit() {
        v2 temp;
        float d = distance();
        temp.x = x / d;
        temp.y = y / d;
        return temp;
    }

    v2 operator -(v2 other) {
        v2 temp;
        temp.x = x - other.x;
        temp.y = y - other.y;
        return temp;
    }

    v2 operator +(v2 other) {
        v2 temp;
        temp.x = x + other.x;
        temp.y = y + other.y;
        return temp;
    }

    v2 operator /(float other) {
        v2 temp;
        temp.x = x / other;
        temp.y = y / other;
        return temp;
    }

    v2 operator *(float other) {
        v2 temp;
        temp.x = x * other;
        temp.y = y * other;
        return temp;
    }

    void operator +=(v2 other) {
        x += other.x;
        y += other.y;
    }

    void operator -=(v2 other) {
        x -= other.x;
        y -= other.y;
    }
};

ostream& operator << (ostream& os, v2 v) {
    os << v.x << " : " << v.y << endl;
    return os;
}

struct piece {
    olc::Pixel color;
    bool dama;
    v2 pos;
    piece(olc::Pixel c, v2 p, bool d) {
        dama = d;
        color = c;
        pos = p;
    }
};

struct tile {
    olc::Pixel color;
    v2 center;
    vector<int> gridpos;
    int occupied;
    tile(olc::Pixel c, v2 c2, int line, int column) {
        color = c;
        center = c2;
        gridpos = { line, column };
    }


    bool operator == (tile o) {

        return (condition);
    }
};

int movepiece(vector<piece>& pieces, v2 mousepos, olc::PixelGameEngine* engine, int turn) {
    cout << "change" << endl;
    for (auto i = pieces.begin(); i != pieces.end(); i++) {
        piece p = *i;
        bool ahead = (mousepos.x < p.pos.x + 25 and mousepos.x > p.pos.x - 25 and mousepos.y < p.pos.y + 25 and mousepos.y > p.pos.y - 25) ? true : false;
        if (ahead == true) {
            olc::Pixel c = p.color;
            if (c == olc::BLACK and turn % 2 == 0) { return 0; }
            else if (c == olc::WHITE and turn % 2 != 0) { return 0; }
            pieces.erase(i);
            return ((int)c.r == 255) ? 2 : 1;
           }
        }
    return 0;
   }

tile startingtile(v2 mousepos, vector<tile> tiles) {
    for (tile t : tiles) {
        if (mousepos.x < t.center.x + 25 and mousepos.x > t.center.x - 25 and mousepos.y < t.center.y + 25 and mousepos.y > t.center.y - 25) {
            return t;
        }
    }
}

void drawtable(vector<string> ltrs, vector<string> nmbs, vector<piece> pieces, olc::PixelGameEngine* engine) {
    int posy = 100;
    for (int i = 1; i <= 8; i++) {
        int posx = 100;
        for (int w = 1; w <= 4; w++) {
            if (i % 2 == 0) {
                engine->FillRect(posx, posy, 50, 50, olc::CYAN);
                engine->FillRect(posx + 50, posy, 50, 50, olc::DARK_CYAN);
            }
            else {
                if (i == 1) {
                    engine->DrawString(posx + 20, posy - 20, nmbs[(2 * w) - 2], olc::WHITE, 1);
                    engine->DrawString(posx + 70, posy - 20, nmbs[(2 * w) - 1], olc::WHITE, 1);
                }
                engine->FillRect(posx, posy, 50, 50, olc::DARK_CYAN);
                engine->FillRect(posx + 50, posy, 50, 50, olc::CYAN);
            }
            if (w == 1) { engine->DrawString(posx - 20, posy + 20, ltrs[i - 1], olc::WHITE, 1); }
            posx += 100;
        }
        posy += 50;
    }
    for (piece& p : pieces) {
        engine->FillCircle({ (int)p.pos.x, (int)p.pos.y }, 20, p.color);
    }
 }

bool isdiagonal(tile t1, tile t2) {
    vector<int> sum = { -1, 1 };
    int k1 = t2.gridpos[0] - t1.gridpos[0];
    int k2 = t2.gridpos[1] - t1.gridpos[1];
    if (find(sum.begin(), sum.end(), k1) != sum.end() and find(sum.begin(), sum.end(), k2) != sum.end()) {
        return true;
    }
    else { return false; }
}

vector<tile> possiblemoves(vector<tile> tiles, int haspiece, bool dama, tile actual) {
    olc::Pixel color = (haspiece == 1) ? olc::BLACK : olc::WHITE;
    vector<tile> possible;
    for (tile t : tiles) {
        if (isdiagonal(actual, t) and t.occupied == 0) {
            possible.push_back(t);
        }
    }
    return possible;
}

struct table : olc::PixelGameEngine {
    vector<string> ltrs = { "A", "B", "C", "D", "E", "F", "G", "H" };
    vector<string> nmbs = { "1", "2", "3", "4", "5", "6", "7", "8" };
    vector<piece> pieces;
    vector<tile> tiles;
    olc::Sprite* screen = new olc::Sprite(600, 600);
    bool OnUserCreate() {
        int posy = 100;
        for (int i = 1; i <= 8; i++) {
            int posx = 100;
            for (int w = 1; w <= 4; w++) {
                if (i % 2 == 0) {
                    FillRect(posx, posy , 50, 50, olc::CYAN);
                    tiles.push_back(tile(olc::CYAN, { posx + 25, posy + 25 }, w*2 - 1, i));
                    FillRect(posx + 50, posy , 50, 50, olc::DARK_CYAN);
                    tiles.push_back(tile(olc::DARK_CYAN, { posx + 75, posy + 25 }, w*2, i));
                    if (i <= 3) {
                        FillCircle({ posx + 75, posy + 25 }, 20, olc::WHITE);
                        pieces.push_back(piece(olc::WHITE, { posx + 75, posy + 25 }, false));
                    }
                    else if (i >= 6) {
                        FillCircle({ posx + 75, posy + 25 }, 20, olc::BLACK);
                        pieces.push_back(piece(olc::BLACK, { posx + 75, posy + 25 }, false));
                    }
                }
                else {
                    if (i == 1) {
                        DrawString(posx + 20, posy - 20, nmbs[w*2 - 2], olc::WHITE, 1);
                        DrawString(posx + 70, posy - 20, nmbs[w*2-1], olc::WHITE, 1);
                    }
                    FillRect(posx, posy, 50, 50, olc::DARK_CYAN);
                    tiles.push_back(tile(olc::DARK_CYAN, { posx + 25, posy + 25 }, w*2-1, i));
                    if (i <= 3) {
                        FillCircle({ posx + 25, posy + 25 }, 20, olc::WHITE);
                        pieces.push_back(piece(olc::WHITE, { posx + 25, posy + 25 }, false));
                    }
                    else if (i >= 6) { 
                        FillCircle({ posx + 25, posy + 25 }, 20, olc::BLACK);
                        pieces.push_back(piece(olc::BLACK, { posx + 25, posy + 25 }, false));
                    }
                    FillRect(posx + 50, posy, 50, 50, olc::CYAN);
                    tiles.push_back(tile(olc::CYAN, { posx + 75, posy + 25 }, w*2, i));
                }
                if (w == 1) { DrawString(posx - 20, posy + 20, ltrs[i - 1], olc::WHITE, 1); }
                posx += 100;
            }
            posy += 50;
        }

        return true;
    }
    int turn = 1;
    int haspiece = 0;
    tile actual = tile(olc::BLACK, { 10, 10 }, 1, 1);
    bool OnUserUpdate(float dt) {
        v2 mousepos = { GetMouseX(), GetMouseY() };
        for (auto i = pieces.begin(); i != pieces.end(); i++) {
            piece p = *i;
            for (auto w = tiles.begin(); w != tiles.end(); w++) {
                tile t = *w;
                if (p.pos.x == t.center.x and p.pos.y == t.center.y) {
                    t.occupied = (p.color == olc::BLACK) ? 1 : 2;
                    break;
                }
                else { t.occupied = 0; }
            }
        }
        Clear(olc::BLACK);
        drawtable(ltrs, nmbs, pieces, this);
        if (turn % 2 != 0) { DrawString(200, 530, "Black's turn", olc::WHITE, 2); }
        else { DrawString(200, 530, "White's turn", olc::WHITE, 2); }
        if (GetMouse(0).bPressed and haspiece == 0) {
            haspiece = movepiece(pieces, mousepos, this, turn);
            tile actual = startingtile(mousepos, tiles);
        }
        if (haspiece == 1) {
            FillCircle(mousepos.x, mousepos.y, 20, olc::BLACK);
            
        }
        else if (haspiece == 2) {
            FillCircle(mousepos.x, mousepos.y, 20, olc::WHITE);
        }
        if (GetMouse(0).bReleased and haspiece > 0) {
            v2 pos;
            for (auto i = tiles.begin(); i != tiles.end(); i++) {
                tile t = *i;
                if (mousepos.x < t.center.x + 25 and mousepos.x > t.center.x - 25 and mousepos.y < t.center.y + 25 and mousepos.y > t.center.y - 25) {
                    vector<tile> possible = possiblemoves(tiles, haspiece, false, actual);
                    
                    if (find(possible.begin(), possible.end(), t) != possible.end()){
                    pos = t.center;
                    break;
                    }
                }     
            }
            olc::Pixel c = (haspiece == 1) ? olc::BLACK : olc::WHITE;

            pieces.push_back(piece(c, pos, false));
            haspiece = 0;
        }
        if (GetKey(olc::C).bPressed) { turn += 1; }

        return true;
        


    }
};



int main()
{
    table checkers;
    checkers.Construct(600, 600, 1, 1);
    checkers.Start();
}
