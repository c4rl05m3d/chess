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
    piece() {}

};

struct tile {
    olc::Pixel color;
    v2 center;
    vector<int> gridpos;
    int occupied;
    tile(olc::Pixel c, v2 c2, int line, int column, int o) {
        color = c;
        center = c2;
        occupied = o;
        gridpos = { line, column };
    }

    tile() {}

    bool operator == (tile o) {
        return gridpos[0] == o.gridpos[0] and gridpos[1] == o.gridpos[1] and color.r == o.color.r;

    }
};

void checkoccupied(vector<tile>& tiles, vector<piece>& pieces) {
    for (piece &p : pieces) {
        v2 ppos = p.pos;
        int occ = (p.color == olc::BLACK) ? 1 : 2;
        for (tile &t : tiles) {
            v2 tpos = t.center;
            if (tpos.x == ppos.x and tpos.y == ppos.y) { t.occupied = occ; }
            else { t.occupied = 0; }
        }
    }
}

int movepiece(vector<piece>& pieces, v2 mousepos, olc::PixelGameEngine* engine, int turn) {
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

piece startingpiece(v2 mousepos, vector<piece> pieces) {
    for (piece p : pieces) {
        cout << "sup" << endl;
        if (mousepos.x < p.pos.x + 20 and mousepos.x > p.pos.x - 20 and mousepos.y < p.pos.y + 20 and mousepos.y > p.pos.y - 20) {
            return p;
        }
    }
}

v2 poswithgrid(vector<int> gridpos) {
    int x = gridpos[0] * 50 + 125;
    int y = gridpos[1] * 50 + 125;
    return { x, y };

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
    if (t2.center.x - t1.center.x == t2.center.y - t1.center.y || (t2.center.x - t1.center.x)*-1 == t2.center.y - t1.center.y) {
        return true;
    }
    else { return false; }
}

void killavailable(tile t, piece p, tile actual, vector<tile> alltiles, vector<tile>& diagonals) {
    v2 pos;
    vector<int> grid;
    if (p.dama == false) {
            if (t.occupied != actual.occupied and t.occupied != 0) {
                grid = t.gridpos;
                if (t.gridpos[0] > actual.gridpos[0]) { grid[0] += 1; }
                else { grid[0] -= 1; }
                if (p.color == olc::BLACK) {
                    grid[1] -= 1;
                }
                else {
                    grid[1] += 1;
                }
                pos = poswithgrid(grid);
                for (tile tt : alltiles) {
                    if (tt.gridpos == grid) {
                        if (tt.occupied == 0) {
                            diagonals.push_back(tile(olc::DARK_CYAN, pos, grid[0], grid[1], 0)); }
                    }
                }
            }
        }
}

void kill(vector<piece>& pieces, tile start, tile end) {
    float medx = (start.center.x + end.center.x) / 2;
    float medy = (start.center.y + end.center.y) / 2;
    for (piece p : pieces) {
        if (p.pos.x  == medx and p.pos.y == medy) {
            p.pos = { 0,0 };
            p.color = olc::BLACK;
        }
    }

}

vector<tile> possiblemoves(vector<tile>& tiles, int haspiece, bool dama, tile actual, piece p) {
    olc::Pixel color = (haspiece == 1) ? olc::BLACK : olc::WHITE;
    vector<int> null = { 0,0 };
    vector<tile> possible;
    int fifty = (color == olc::BLACK) ? -50 : 50;
    for (tile t : tiles) {
        if (isdiagonal(actual, t) and t.center.y - actual.center.y == fifty) {
            if (t.occupied == 0) { possible.push_back(t); }
            else { killavailable(t, p, actual, tiles, possible); }     
            }
        }
    for (tile t : possible) {
        cout << t.gridpos << " possible" << endl;
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
                    FillRect(posx, posy, 50, 50, olc::CYAN);
                    tiles.push_back(tile(olc::CYAN, { posx + 25, posy + 25 }, i, w * 2 - 1, 1));
                    FillRect(posx + 50, posy, 50, 50, olc::DARK_CYAN);
                    tiles.push_back(tile(olc::DARK_CYAN, { posx + 75, posy + 25 }, i, w * 2,  1));
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
                        DrawString(posx + 20, posy - 20, nmbs[w * 2 - 2], olc::WHITE, 1);
                        DrawString(posx + 70, posy - 20, nmbs[w * 2 - 1], olc::WHITE, 1);
                    }
                    FillRect(posx, posy, 50, 50, olc::DARK_CYAN);
                    tiles.push_back(tile(olc::DARK_CYAN, { posx + 25, posy + 25 }, i, w * 2 - 1, 1));
                    if (i <= 3) {
                        FillCircle({ posx + 25, posy + 25 }, 20, olc::WHITE);
                        pieces.push_back(piece(olc::WHITE, { posx + 25, posy + 25 }, false));
                    }
                    else if (i >= 6) {
                        FillCircle({ posx + 25, posy + 25 }, 20, olc::BLACK);
                        pieces.push_back(piece(olc::BLACK, { posx + 25, posy + 25 }, false));
                    }
                    FillRect(posx + 50, posy, 50, 50, olc::CYAN);
                    tiles.push_back(tile(olc::CYAN, { posx + 75, posy + 25 }, i, w * 2,  1));
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
    tile actualt = tile();
    piece actualp = piece();
    bool OnUserUpdate(float dt) {
        v2 mousepos = { GetMouseX(), GetMouseY() };
        checkoccupied(tiles, pieces);
        Clear(olc::BLACK);
        drawtable(ltrs, nmbs, pieces, this);
        if (turn % 2 != 0) { DrawString(200, 530, "Black's turn", olc::WHITE, 2); }
        else { DrawString(200, 530, "White's turn", olc::WHITE, 2); }
        if (GetMouse(0).bPressed and haspiece == 0) {
            haspiece = movepiece(pieces, mousepos, this, turn);
            actualt = startingtile(mousepos, tiles);
            actualp = startingpiece(mousepos, pieces);

        }
        if (haspiece == 1) {
            FillCircle(mousepos.x, mousepos.y, 20, olc::BLACK);

        }
        else if (haspiece == 2) {
            FillCircle(mousepos.x, mousepos.y, 20, olc::WHITE);
        }
        if (GetMouse(0).bReleased and haspiece > 0) {
            v2 pos;
            if (mousepos.x < 100 || mousepos.x > 500 || mousepos.y < 100 || mousepos.y > 500) { pos = actualt.center; }
            for (auto i = tiles.begin(); i != tiles.end(); i++) {
                tile t = *i;
                if (mousepos.x < t.center.x + 25 and mousepos.x > t.center.x - 25 and mousepos.y < t.center.y + 25 and mousepos.y > t.center.y - 25) {
                    vector<tile> possible = possiblemoves(tiles, haspiece, false, actualt, actualp);
                    if (find(possible.begin(), possible.end(), t) != possible.end()) {
                        int diagonaldistance = t.gridpos[0] - actualt.gridpos[0];
                        if (diagonaldistance < 0) {diagonaldistance *= -1; }
                        pos = t.center;
                        if (diagonaldistance > 1) { kill(pieces, actualt, t); }
                        cout << actualt.gridpos << " to " << t.gridpos << endl;
                        break;
                    }
                    else { pos = actualt.center; }
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