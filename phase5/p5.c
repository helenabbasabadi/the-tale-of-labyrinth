#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

int magnitude(int x) { return x < 0 ? -x : x; }

/* ---------- نمایش صفحه ---------- */
void showBoard(int rows, int cols, char field[rows][cols],
               int hWall[rows][cols], int vWall[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c", field[i][j]);
            if (j < cols - 1) printf(vWall[i][j] ? "|" : " ");
        }
        printf("\n");
        if (i < rows - 1) {
            for (int j = 0; j < cols; j++) {
                printf(hWall[i][j] ? "-" : " ");
                if (j < cols - 1) printf(" ");
            }
            printf("\n");
        }
    }
}

/* ---------- بررسی اتصال ---------- */
int checkConnectivity(int rows, int cols, int hWall[rows][cols], int vWall[rows][cols]) {
    int vis[rows][cols];
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            vis[i][j] = 0;
    
    int st[rows * cols][2], sp = 0;
    st[sp][0] = 0; st[sp][1] = 0; vis[0][0] = 1; sp++;
    
    while (sp) {
        int x = st[--sp][0], y = st[sp][1];
        if (x > 0 && !vis[x - 1][y] && !hWall[x - 1][y])
            vis[x - 1][y] = 1, st[sp][0] = x - 1, st[sp++][1] = y;
        if (x < rows - 1 && !vis[x + 1][y] && !hWall[x][y])
            vis[x + 1][y] = 1, st[sp][0] = x + 1, st[sp++][1] = y;
        if (y > 0 && !vis[x][y - 1] && !vWall[x][y - 1])
            vis[x][y - 1] = 1, st[sp][0] = x, st[sp++][1] = y - 1;
        if (y < cols - 1 && !vis[x][y + 1] && !vWall[x][y])
            vis[x][y + 1] = 1, st[sp][0] = x, st[sp++][1] = y + 1;
    }
    
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (!vis[i][j]) return 0;
    return 1;
}

/* ---------- نزدیک‌ترین رانر ---------- */
int closestRunner(int sx, int sy, int rX[], int rY[], int n) {
    int idx = 0;
    int min = magnitude(sx - rX[0]) + magnitude(sy - rY[0]);
    for (int i = 1; i < n; i++) {
        int d = magnitude(sx - rX[i]) + magnitude(sy - rY[i]);
        if (d < min) { min = d; idx = i; }
    }
    return idx;
}

/* ---------- رویداد شانسی ---------- */
void applyChance(int *extraTurn, int *wallNum, int row, int col,
                int rX[], int rY[], int R,
                int sX[], int sY[], int S,
                int hWall[row][col], int vWall[row][col],
                int *skipShadowMovement,
                int assigned[]) {
    int e = rand() % 4;
    if (e == 0) {
        printf("Chance: Extra Turn!\n");
        *extraTurn = 1;
    } else if (e == 1) {
        printf("Chance: +2 Walls!\n");
        int placed = 0;
        while (placed < 2) {
            int t = rand() % 2;
            int r = rand() % row;
            int c = rand() % col;
            if (t == 0 && r < row - 1 && !hWall[r][c]) {
                hWall[r][c] = 1;
                placed++;
            } else if (t == 1 && c < col - 1 && !vWall[r][c]) {
                vWall[r][c] = 1;
                placed++;
            }
        }
    } else if (e == 2) {
        printf("Chance: Earthquake!\n");
        for (int i = 0; i < R; i++) {
            int d = rand() % 4;
            if (d == 0 && rX[i] > 0 && !hWall[rX[i] - 1][rY[i]]) rX[i]--;
            else if (d == 1 && rX[i] < row - 1 && !hWall[rX[i]][rY[i]]) rX[i]++;
            else if (d == 2 && rY[i] > 0 && !vWall[rX[i]][rY[i] - 1]) rY[i]--;
            else if (d == 3 && rY[i] < col - 1 && !vWall[rX[i]][rY[i]]) rY[i]++;
        }
        for (int i = 0; i < S; i++) {
            int d = rand() % 4;
            if (d == 0 && sX[i] > 0 && !hWall[sX[i] - 1][sY[i]]) sX[i]--;
            else if (d == 1 && sX[i] < row - 1 && !hWall[sX[i]][sY[i]]) sX[i]++;
            else if (d == 2 && sY[i] > 0 && !vWall[sX[i]][sY[i] - 1]) sY[i]--;
            else if (d == 3 && sY[i] < col - 1 && !vWall[sX[i]][sY[i]]) sY[i]++;
        }
        for (int i = 0; i < S; i++) {
            assigned[i] = closestRunner(sX[i], sY[i], rX, rY, R);
        }
        *skipShadowMovement = 1;
    } else {
        printf("Chance: Move a Shadow!\n");
        int id, dir;
        printf("Shadow index (0-%d): ", S - 1);
        scanf("%d", &id);
        printf("Dir (0U 1D 2L 3R): ");
        scanf("%d", &dir);
        if (dir == 0 && sX[id] > 0 && !hWall[sX[id] - 1][sY[id]]) sX[id]--;
        if (dir == 1 && sX[id] < row - 1 && !hWall[sX[id]][sY[id]]) sX[id]++;
        if (dir == 2 && sY[id] > 0 && !vWall[sX[id]][sY[id] - 1]) sY[id]--;
        if (dir == 3 && sY[id] < col - 1 && !vWall[sX[id]][sY[id]]) sY[id]++;
        *skipShadowMovement = 1;
    }
}

/* ================== MAIN ================== */
int main() {
    int m, n, walls, num_mappers, num_shadowers;
    printf("Enter number of rows (m): ");
    scanf("%d", &m);
    printf("Enter number of columns (n): ");
    scanf("%d", &n);
    printf("Enter number of walls: ");
    scanf("%d", &walls);
    printf("Enter number of mappers: ");
    scanf("%d", &num_mappers);
    printf("Enter number of shadowers: ");
    scanf("%d", &num_shadowers);
    
    char grid[m][n];
    int chanceCell[m][n];
    int hWall[m][n], vWall[m][n];
    int rX[num_mappers], rY[num_mappers], sX[num_shadowers], sY[num_shadowers], assigned[num_shadowers];
    
    srand(time(NULL));
    
    /* مقداردهی اولیه */
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++) {
            grid[i][j] = '.';
            chanceCell[i][j] = 0;
        }
    
    /* ساخت جعبه‌ها */
    int chance = (m * n) / 10;
    for (int i = 0; i < chance; i++) {
        int x, y;
        do { x = rand() % m; y = rand() % n; }
        while (chanceCell[x][y]);
        chanceCell[x][y] = 1;
    }
    
    /* ساخت خانه هدف */
    int gx = rand() % m, gy = rand() % n;
    
    /* ساخت رانرها */
    for (int i = 0; i < num_mappers; i++) {
        rX[i] = rand() % m;
        rY[i] = rand() % n;
    }
    
    /* ساخت سایه‌ها */
    for (int i = 0; i < num_shadowers; i++) {
        sX[i] = rand() % m;
        sY[i] = rand() % n;
        assigned[i] = -1;
    }
    
    /* دیوارها */
    do {
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                hWall[i][j] = vWall[i][j] = 0;
        
        int placedWalls = 0;
        while (placedWalls < walls) {
            int t = rand() % 2, r = rand() % m, c = rand() % n;
            if (t == 0 && r < m - 1 && !hWall[r][c]) {
                hWall[r][c] = 1;
                placedWalls++;
            }
            if (t == 1 && c < n - 1 && !vWall[r][c]) {
                vWall[r][c] = 1;
                placedWalls++;
            }
        }
    } while (!checkConnectivity(m, n, hWall, vWall));
    
    while (1) {
        /* بازسازی کامل صفحه */
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                grid[i][j] = chanceCell[i][j] ? '?' : '.';
        
        for (int i = 0; i < num_mappers; i++)
            grid[rX[i]][rY[i]] = 'A' + i;
        
        for (int i = 0; i < num_shadowers; i++)
            grid[sX[i]][sY[i]] = 'S';
        
        grid[gx][gy] = 'H';
        showBoard(m, n, grid, hWall, vWall);
        
        /* حرکت هر نقشه‌بردار به ترتیب */
        for (int i = 0; i < num_mappers; i++) {
            int extraTurn = 0;
            int skipShadowMovement = 0;
            
            printf("\nPlayer %d's turn (A/S/W/D): ", i);
            char mv;
            scanf(" %c", &mv);
            
            int nx = rX[i], ny = rY[i];
            if ((mv == 'W' || mv == 'w') && nx > 0 && !hWall[nx - 1][ny]) nx--;
            if ((mv == 'S' || mv == 's') && nx < m - 1 && !hWall[nx][ny]) nx++;
            if ((mv == 'A' || mv == 'a') && ny > 0 && !vWall[nx][ny - 1]) ny--;
            if ((mv == 'D' || mv == 'd') && ny < n - 1 && !vWall[nx][ny]) ny++;
            
            rX[i] = nx;
            rY[i] = ny;
            
            /* بررسی برنده شدن */
            if (nx == gx && ny == gy) {
                printf("\nYOU WIN!\n");
                return 0;
            }
            
            /* بررسی جعبه شانسی */
            if (chanceCell[nx][ny]) {
                chanceCell[nx][ny] = 0;
                applyChance(&extraTurn, &walls, m, n,
                            rX, rY, num_mappers, sX, sY, num_shadowers, hWall, vWall, &skipShadowMovement, assigned);
            }
            
            /* اگر نوبت اضافی داشت، این بازیکن دوباره حرکت می‌کند */
            if (extraTurn) {
                i--; // برگشت به همان بازیکن برای حرکت اضافی
                continue;
            }
        }
        
        /* حرکت سایه‌نگرها (همزمان) */
        for (int i = 0; i < num_shadowers; i++) {
            if (assigned[i] == -1)
                assigned[i] = closestRunner(sX[i], sY[i], rX, rY, num_mappers);
            
            int t = assigned[i];
            if (sX[i] < rX[t] && !hWall[sX[i]][sY[i]]) sX[i]++;
            else if (sX[i] > rX[t] && !hWall[sX[i] - 1][sY[i]]) sX[i]--;
            else if (sY[i] < rY[t] && !vWall[sX[i]][sY[i]]) sY[i]++;
            else if (sY[i] > rY[t] && !vWall[sX[i]][sY[i] - 1]) sY[i]--;
            
            /* بررسی شکست */
            if (sX[i] == rX[t] && sY[i] == rY[t]) {
                printf("\nGAME OVER! Runner %d caught\n", t);
                return 0;
            }
        }
    }
}
