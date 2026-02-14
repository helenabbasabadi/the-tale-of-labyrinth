#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// تابع برای چاپ جدول (با در نظر گرفتن دیوارهای موقت)
void printMap(int m, int n, char map[m][n], int hwall[m][n], int vwall[m][n], int temp_hwall[m][n], int temp_vwall[m][n]) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (map[i][j]) printf("%c", map[i][j]);
            else printf(".");
            if (j < n-1) {
                if (vwall[i][j] || temp_vwall[i][j]) printf("|");
                else printf(" ");
            }
        }
        printf("\n");
        if (i < m-1) {
            for (int j = 0; j < n; j++) {
                if (hwall[i][j] || temp_hwall[i][j]) printf("-");
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
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            visited[i][j] = 0;
    
    int stack[m*n][2], top = 0;
    stack[top][0] = startX;
    stack[top][1] = startY;
    top++;
    visited[startX][startY] = 1;
    
    while (top > 0) {
        int x = stack[top-1][0];
        int y = stack[top-1][1];
        top--;
        
        if (x > 0 && !visited[x-1][y] && !hwall[x-1][y]) { 
            visited[x-1][y] = 1; 
            stack[top][0] = x-1; 
            stack[top][1] = y; 
            top++; 
        }
        if (x < m-1 && !visited[x+1][y] && !hwall[x][y]) { 
            visited[x+1][y] = 1; 
            stack[top][0] = x+1; 
            stack[top][1] = y; 
            top++; 
        }
        if (y > 0 && !visited[x][y-1] && !vwall[x][y-1]) { 
            visited[x][y-1] = 1; 
            stack[top][0] = x; 
            stack[top][1] = y-1; 
            top++; 
        }
        if (y < n-1 && !visited[x][y+1] && !vwall[x][y]) { 
            visited[x][y+1] = 1; 
            stack[top][0] = x; 
            stack[top][1] = y+1; 
            top++; 
        }
    }
    
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (!visited[i][j]) return 0;
    return 1;
}

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
    
    // محاسبه حداکثر دیوار موقت
    int max_temp_walls = (m < n ? m : n) / 3;
    if (max_temp_walls < 1) max_temp_walls = 1;
    
    char map[m][n];
    int hwall[m][n];
    int vwall[m][n];
    int temp_hwall[m][n];
    int temp_vwall[m][n];
    
    srand(time(NULL));
    
    // قرار دادن هسته نور
    int hx = rand() % m;
    int hy = rand() % n;
    
    // قرار دادن نقشه‌بردارها
    int r_x[num_mappers], r_y[num_mappers];
    for (int i = 0; i < num_mappers; i++) {
        do {
            r_x[i] = rand() % m;
            r_y[i] = rand() % n;
        } while ((r_x[i] == hx && r_y[i] == hy));
        for (int j = 0; j < i; j++) {
            while (r_x[i] == r_x[j] && r_y[i] == r_y[j]) {
                r_x[i] = rand() % m;
                r_y[i] = rand() % n;
            }
        }
    }
    
    // قرار دادن سایه‌نگرها
    int s_x[num_shadowers], s_y[num_shadowers];
    for (int i = 0; i < num_shadowers; i++) {
        do {
            s_x[i] = rand() % m;
            s_y[i] = rand() % n;
        } while ((s_x[i] == hx && s_y[i] == hy));
        for (int j = 0; j < i; j++) {
            while (s_x[i] == s_x[j] && s_y[i] == s_y[j]) {
                s_x[i] = rand() % m;
                s_y[i] = rand() % n;
            }
        }
        for (int j = 0; j < num_mappers; j++) {
            while (s_x[i] == r_x[j] && s_y[i] == r_y[j]) {
                s_x[i] = rand() % m;
                s_y[i] = rand() % n;
            }
        }
    }
    
    // مقداردهی اولیه نقشه
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            map[i][j] = '.';
    
    map[hx][hy] = 'H';
    for (int i = 0; i < num_mappers; i++)
        map[r_x[i]][r_y[i]] = 'R';
    for (int i = 0; i < num_shadowers; i++)
        map[s_x[i]][s_y[i]] = 'S';
    
    // مقداردهی اولیه دیوارها
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            hwall[i][j] = vwall[i][j] = temp_hwall[i][j] = temp_vwall[i][j] = 0;
    
    // تولید دیوارهای ثابت
    int validMap = 0;
    while (!validMap) {
        int placedWalls = 0;
        while (placedWalls < walls) {
            int t = rand() % 2, i = rand() % m, j = rand() % n;
            if (t == 0 && i < m-1 && !hwall[i][j]) { 
                hwall[i][j] = 1; 
                placedWalls++; 
            }
            if (t == 1 && j < n-1 && !vwall[i][j]) { 
                vwall[i][j] = 1; 
                placedWalls++; 
            }
        }
        if (isConnected(m, n, hwall, vwall, 0, 0)) 
            validMap = 1;
        else 
            printf("Invalid random wall placement! Generating a new map...\n");
    }
    
    // متغیرهای بازی
    int temp_walls_used = 0;
    int gameOver = 0;
    
    while (!gameOver) {
        system("clear");
        printMap(m, n, map, hwall, vwall, temp_hwall, temp_vwall);
        
        // حرکت نقشه‌بردارها (یکی یکی)
        for (int i = 0; i < num_mappers; i++) {
            printf("\nMapper %d's turn (W/A/S/D/P): ", i+1);
            char move;
            scanf(" %c", &move);
            
            // قرار دادن دیوار موقت
            if (move == 'P' || move == 'p') {
                if (temp_walls_used >= max_temp_walls) {
                    printf("Max temporary walls reached!\n");
                    continue;
                }
                char wall_type;
                int pos_i, pos_j;
                printf("Enter wall type (H/V) and position (i j): ");
                scanf(" %c %d %d", &wall_type, &pos_i, &pos_j);
                
                if (wall_type == 'H' && pos_i < m-1 && !temp_hwall[pos_i][pos_j]) {
                    temp_hwall[pos_i][pos_j] = 1;
                    temp_walls_used++;
                    printf("Temporary wall placed at (%d, %d)!\n", pos_i, pos_j);
                } else if (wall_type == 'V' && pos_j < n-1 && !temp_vwall[pos_i][pos_j]) {
                    temp_vwall[pos_i][pos_j] = 1;
                    temp_walls_used++;
                    printf("Temporary wall placed at (%d, %d)!\n", pos_i, pos_j);
                } else {
                    printf("Invalid wall placement!\n");
                }
                continue;
            }
            
            // حرکت نقشه‌بردار
            int nr = r_x[i], nc = r_y[i];
            if ((move == 'W' || move == 'w') && nr > 0 && !hwall[nr-1][nc] && !temp_hwall[nr-1][nc]) nr--;
            if ((move == 'S' || move == 's') && nr < m-1 && !hwall[nr][nc] && !temp_hwall[nr][nc]) nr++;
            if ((move == 'A' || move == 'a') && nc > 0 && !vwall[nr][nc-1] && !temp_vwall[nr][nc-1]) nc--;
            if ((move == 'D' || move == 'd') && nc < n-1 && !vwall[nr][nc] && !temp_vwall[nr][nc]) nc++;
            
            if (nr != r_x[i] || nc != r_y[i]) {
                map[r_x[i]][r_y[i]] = '.';
                r_x[i] = nr;
                r_y[i] = nc;
                map[r_x[i]][r_y[i]] = 'R';
                
                // بررسی برنده شدن
                if (r_x[i] == hx && r_y[i] == hy) {
                    system("clear");
                    printMap(m, n, map, hwall, vwall, temp_hwall, temp_vwall);
                    printf("\n YOU WIN!\n");
                    gameOver = 1;
                    break;
                }
            }
        }
        
        if (gameOver) break;
        
        // حرکت سایه‌نگرها (همزمان)
        for (int i = 0; i < num_shadowers; i++) {
            map[s_x[i]][s_y[i]] = '.';
            
            // پیدا کردن نزدیک‌ترین نقشه‌بردار
            int closest_idx = 0;
            int min_dist = INT_MAX;
            for (int j = 0; j < num_mappers; j++) {
                int dist = abs(s_x[i] - r_x[j]) + abs(s_y[i] - r_y[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_idx = j;
                }
            }
            
            // حرکت به سمت نقشه‌بردار نزدیک‌ترین
            int mx = r_x[closest_idx], my = r_y[closest_idx];
            int moved = 0;
            
            if (s_y[i] < my && s_y[i] < n-1 && !vwall[s_x[i]][s_y[i]] && !temp_vwall[s_x[i]][s_y[i]]) {
                s_y[i]++;
                moved = 1;
            } else if (s_y[i] > my && s_y[i] > 0 && !vwall[s_x[i]][s_y[i]-1] && !temp_vwall[s_x[i]][s_y[i]-1]) {
                s_y[i]--;
                moved = 1;
            }
            
            if (!moved) {
                if (s_x[i] < mx && s_x[i] < m-1 && !hwall[s_x[i]][s_y[i]] && !temp_hwall[s_x[i]][s_y[i]]) {
                    s_x[i]++;
                } else if (s_x[i] > mx && s_x[i] > 0 && !hwall[s_x[i]-1][s_y[i]] && !temp_hwall[s_x[i]-1][s_y[i]]) {
                    s_x[i]--;
                }
            }
            
            map[s_x[i]][s_y[i]] = 'S';
            
            // بررسی شکست
            for (int j = 0; j < num_mappers; j++) {
                if (s_x[i] == r_x[j] && s_y[i] == r_y[j]) {
                    system("clear");
                    printMap(m, n, map, hwall, vwall, temp_hwall, temp_vwall);
                    printf("\n GAME OVER!\n");
                    gameOver = 1;
                    break;
                }
            }
            if (gameOver) break;
        }
    }
    
    return 0;
}
