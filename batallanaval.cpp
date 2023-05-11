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
        void pintar_portraaviones(int, int, int);
        void borrar_portaaviones(int, int, int);
        void mover_portaaviones(int &, int &, int);
};

void jugador::pintar_portraaviones(int x, int y, int d){
    if(d == 0){
        gotoxy(x,y);
        cout<<"P";
        gotoxy(x,y+2);
        cout<<"P";
        gotoxy(x,y+4);
        cout<<"P";
        gotoxy(x,y+6);
        cout<<"P";
    } else{
        gotoxy(x,y);
        cout<<"P";
        gotoxy(x+2,y);
        cout<<"P";
        gotoxy(x+4,y);
        cout<<"P";
        gotoxy(x+6,y);
        cout<<"P";
    }
}

void jugador::borrar_portaaviones(int x, int y, int d){
    if(d == 0){
        gotoxy(x,y);
        cout<<" ";
        gotoxy(x,y+2);
        cout<<" ";
        gotoxy(x,y+4);
        cout<<" ";
        gotoxy(x,y+6);
        cout<<" ";
    } else{
        gotoxy(x,y);
        cout<<" ";
        gotoxy(x+2,y);
        cout<<" ";
        gotoxy(x+4,y);
        cout<<" ";
        gotoxy(x+6,y);
        cout<<" ";
    }
}

void jugador::mover_portaaviones(int &x, int &y, int d){
    if(kbhit()){
        jugador::borrar_portaaviones(x,y,d);
        char tecla = getch();
        if(tecla == 'a' && x>2)
            x-=2;
        if(tecla == 'd' && x<20)
            x+=2;
        if(tecla == 's' && y<46)
            y+=2;
        if(tecla == 'w' && y>28)
            y-=2;
        jugador::pintar_portraaviones(x,y,d);
    }
}

int main(){
    enemigo a;
    jugador b;
    int q1=2,w1=3;
    int q2=2,w2=28;

    gotoxy(0, 0);
    a.pintarmapa();
    gotoxy(0, 25);
    a.pintarmapa();

    while(true){
        b.mover_portaaviones(q2,w2, 0);
    }

}