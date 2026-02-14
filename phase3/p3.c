#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// تابع برای چاپ جدول
void printMap(int m, int n, char map[m][n], int hwall[m][n], int vwall[m][n]) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if(map[i][j]) printf("%c", map[i][j]);
            else printf(".");
            if (j < n-1) {
                if(vwall[i][j]) printf("|");
                else printf(" ");
            }
        }
        printf("\n");
        if (i < m-1) {
            for (int j = 0; j < n; j++) {
                if(hwall[i][j]) printf("-");
                else printf(" ");
                if (j < n-1) printf(" ");
            }
            printf("\n");
        }
    }
}

// بررسی اتصال تمام خانه‌ها با DFS
int isConnected(int m, int n, int hwall[m][n], int vwall[m][n], int startX, int startY) {
    int visited[m][n];
    for(int i=0;i<m;i++)
        for(int j=0;j<n;j++)
            visited[i][j] = 0;

    int stack[m*n][2], top = 0;
    stack[top][0] = startX;
    stack[top][1] = startY;
    top++;
    visited[startX][startY] = 1;

    while(top > 0) {
        int x = stack[top-1][0];
        int y = stack[top-1][1];
        top--;

        if(x>0 && !visited[x-1][y] && !hwall[x-1][y]) { visited[x-1][y]=1; stack[top][0]=x-1; stack[top][1]=y; top++; }
        if(x<m-1 && !visited[x+1][y] && !hwall[x][y]) { visited[x+1][y]=1; stack[top][0]=x+1; stack[top][1]=y; top++; }
        if(y>0 && !visited[x][y-1] && !vwall[x][y-1]) { visited[x][y-1]=1; stack[top][0]=x; stack[top][1]=y-1; top++; }
        if(y<n-1 && !visited[x][y+1] && !vwall[x][y]) { visited[x][y+1]=1; stack[top][0]=x; stack[top][1]=y+1; top++; }
    }

    for(int i=0;i<m;i++)
        for(int j=0;j<n;j++)
            if(!visited[i][j]) return 0;
    return 1;
}

int main() {
    int m, n, walls;

    printf("Enter number of rows (m): ");
    scanf("%d", &m);

    printf("Enter number of columns (n): ");
    scanf("%d", &n);

    printf("Enter number of walls: ");
    scanf("%d", &walls);

    char map[m][n];
    int hwall[m][n];
    int vwall[m][n];

    srand(time(NULL));

    int hx, hy, rx, ry, sx, sy;

    // قرار دادن هسته نور
    hx = rand() % m;
    hy = rand() % n;

    // قرار دادن نقشه بردار
    do { rx = rand() % m; ry = rand() % n; }
    while (rx == hx && ry == hy);

    // قرار دادن سایه نگر
    do { sx = rand() % m; sy = rand() % n; }
    while ((sx == hx && sy == hy) || (sx == rx && sy == ry));

    map[hx][hy] = 'H';
    map[rx][ry] = 'R';
    map[sx][sy] = 'S';

    // قرار دادن دیوارها و بررسی اتصال
    int validMap = 0;
    while(!validMap) {
        // مقداردهی اولیه دیوارها
        for(int i=0;i<m;i++)
            for(int j=0;j<n;j++)
                hwall[i][j]=vwall[i][j]=0;

        int placedWalls = 0;
        while (placedWalls < walls) {
            int t = rand() % 2, i = rand() % m, j = rand() % n;
            if (t == 0 && i < m-1 && !hwall[i][j]) { hwall[i][j] = 1; placedWalls++; }
            if (t == 1 && j < n-1 && !vwall[i][j]) { vwall[i][j] = 1; placedWalls++; }
        }

        // اگر مسیر وجود نداشت، دوباره رندم می‌کنیم
        if(isConnected(m, n, hwall, vwall, 0, 0)) validMap = 1;
        else printf("Invalid random wall placement! Generating a new map...\n");
    }

    // خانه‌های غیر H,R,S را '.' می‌کنیم
    for(int i=0;i<m;i++)
        for(int j=0;j<n;j++)
            if(map[i][j]!='H' && map[i][j]!='R' && map[i][j]!='S') map[i][j]='.';

    char move;
    int gameOver = 0;

    while (!gameOver) {
        system("clear");
        printMap(m, n, map, hwall, vwall);

        printf("\nplease choose one of the directions to move R (W/A/S/D): ");
        scanf(" %c", &move);

        int nr = rx, nc = ry;
        if ((move == 'W' || move == 'w') && rx > 0 && !hwall[rx-1][ry]) nr--;
        if ((move == 'S' || move == 's') && rx < m-1 && !hwall[rx][ry]) nr++;
        if ((move == 'A' || move == 'a') && ry > 0 && !vwall[rx][ry-1]) nc--;
        if ((move == 'D' || move == 'd') && ry < n-1 && !vwall[rx][ry]) nc++;

        if (nr != rx || nc != ry) {
            map[rx][ry] = '.';
            rx = nr; ry = nc;
            map[rx][ry] = 'R';
        }

        if (rx == hx && ry == hy) {
            printf("\n YOU WIN!\n");
            break;
        }

        // حرکت S
        for (int step = 0; step < 2; step++) {
            map[sx][sy] = '.';
            int moved = 0;
            if (sy < ry && sy < n-1 && !vwall[sx][sy]) { sy++; moved = 1; }
            else if (sy > ry && sy > 0 && !vwall[sx][sy-1]) { sy--; moved = 1; }
            if (!moved) {
                if (sx < rx && sx < m-1 && !hwall[sx][sy]) sx++;
                else if (sx > rx && sx > 0 && !hwall[sx-1][sy]) sx--;
            }
        }
        map[sx][sy] = 'S';

        if (sx == rx && sy == ry) {
            system("clear");
            printMap(m, n, map, hwall, vwall);
            printf("\n GAME OVER!\n");
            break;
        }
    }

    return 0;
}
