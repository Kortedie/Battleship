#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;

void gotoxy(int x, int y){
HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = y;
    SetConsoleCursorPosition(hcon, dwPos);
}

char ma[22][22] =   {
                        {' ',' ','A',' ','B',' ','C',' ','D',' ','E',' ','F',' ','G',' ','H',' ','I',' ','J',' '},
                        {' ',' ','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',' '},
                        {'0','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|'},
                        {' ','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|'},
                        {'1','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|'},
                        {' ','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|'},
                        {'2','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|'},
                        {' ','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|'},
                        {'3','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|'},
                        {' ','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|'},
                        {'4','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|'},
                        {' ','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|'},
                        {'5','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|'},
                        {' ','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|'},
                        {'6','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|'},
                        {' ','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|'},
                        {'7','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|'},
                        {' ','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|'},
                        {'8','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|'},
                        {' ','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|','-','|'},
                        {'9','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|'},
                        {' ',' ','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-',' '},
                    };

struct coord{
    int x, y;
};

struct pieza{
    coord original;
    coord periferico[3];
    char C;
    coord posicion(int n);
};

coord pieza::posicion(int n){
    coord retorno = {original.x, original.y};
    if(n != 0){
        retorno.x += periferico[n-1].x;
        retorno.y += periferico[n-1].y;
    };
    return retorno;
}

coord peris[4][3] = {{{2,0},{4,0},{6,0}},
                     {{2,0},{4,0},{   }},
                     {{2,0},{4,0},{   }},
                     {{   },{   },{   }}
};

char barcos[4] = {'P','S','D','F'};

class enemigo{
private :
    public:
        void pintarmapa();
        void mover(int &, int &);
        void pintar(int, int);
        void borrar(int, int);
};

void enemigo::pintarmapa(){
    for(int i=0;i<22;i++){
        cout<<endl;
        for(int j=0;j<22;j++){
            cout<<ma[i][j];
        }
    }
}

void enemigo::mover(int &x, int &y){
    if(kbhit()){
        enemigo::borrar(x,y);
        char tecla = getch();
        if(tecla == 'a' && x>2)
            x-=2;
        if(tecla == 'd' && x<20)
            x+=2;
        if(tecla == 's' && y<21)
            y+=2;
        if(tecla == 'w' && y>3)
            y-=2;
        enemigo::pintar(x,y);
    }
}

void enemigo::pintar(int x, int y){
    gotoxy(x,y);
    cout<<"O";
}

void enemigo::borrar(int x,int y){
    gotoxy(x,y);
    cout<<" ";
}

class jugador{
    public:
        void pintar(pieza &);
        void rotar(pieza &);
        coord rotar(coord &);
        void seleccionar(pieza &, int);
};

void jugador::pintar(pieza &P){
    for(int i=0; i<4; i++){
        coord c = P.posicion(i);
        ma[c.x][c.y] = P.C;
    };
}

coord jugador::rotar(coord &c)
{
    coord ret = {c.y,c.x};
    return ret;
}
void jugador::rotar(pieza &P){
    for(int i=0; i<3; i++){
        P.periferico[i] = jugador::rotar(P.periferico[i]);
    };
}

void jugador::seleccionar(pieza &P, int r){
    P.original.x = 2;
    P.original.y = 2;
    P.C = barcos[r];
    for(int i=0; i<3; i++){
        P.periferico[i] = peris[r][i];
    };
}

int main(){
    enemigo a;
    jugador b;
    pieza S;
    int r = 0;

    b.seleccionar(S,r);
    //a.pintarmapa();
    b.pintar(S);
    gotoxy(0, 25);
    a.pintarmapa();

    system("pause");
    return 0;
}