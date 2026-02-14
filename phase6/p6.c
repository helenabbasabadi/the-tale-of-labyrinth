#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// تابع محاسبه قدر مطلق
int magnitude(int x) { return x < 0 ? -x : x; }

/* ---------- نمایش صفحه بازی ---------- */
void showBoard(int rows, int cols, char **field,
               int **hWall, int **vWall) {
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

/* ---------- بررسی اتصال تمام خانه‌ها ---------- */
int checkConnectivity(int rows, int cols, int **hWall, int **vWall) {
    // تخصیص حافظه برای آرایه بازدید
    int **vis = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        vis[i] = calloc(cols, sizeof(int));
    }
    
    // استفاده از مخزن برای جستجوی عمقی
    int st[rows * cols][2], sp = 0;
    st[sp][0] = 0; st[sp][1] = 0; vis[0][0] = 1; sp++;
    
    while (sp) {
        int x = st[--sp][0], y = st[sp][1];
        // حرکت به بالا
        if (x > 0 && !vis[x - 1][y] && !hWall[x - 1][y])
            vis[x - 1][y] = 1, st[sp][0] = x - 1, st[sp++][1] = y;
        // حرکت به پایین
        if (x < rows - 1 && !vis[x + 1][y] && !hWall[x][y])
            vis[x + 1][y] = 1, st[sp][0] = x + 1, st[sp++][1] = y;
        // حرکت به چپ
        if (y > 0 && !vis[x][y - 1] && !vWall[x][y - 1])
            vis[x][y - 1] = 1, st[sp][0] = x, st[sp++][1] = y - 1;
        // حرکت به راست
        if (y < cols - 1 && !vis[x][y + 1] && !vWall[x][y])
            vis[x][y + 1] = 1, st[sp][0] = x, st[sp++][1] = y + 1;
    }
    
    // بررسی اتصال تمام خانه‌ها
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (!vis[i][j]) {
                // آزادسازی حافظه و بازگشت 0 (اتصال ندارد)
                for (int k = 0; k < rows; k++) free(vis[k]);
                free(vis);
                return 0;
            }
    
    // آزادسازی حافظه و بازگشت 1 (اتصال دارد)
    for (int k = 0; k < rows; k++) free(vis[k]);
    free(vis);
    return 1;
}

/* ---------- پیدا کردن نزدیک‌ترین نقشه‌بردار برای سایه‌نگر ---------- */
int closestRunner(int sx, int sy, int rX[], int rY[], int n) {
    int idx = 0;
    int min = magnitude(sx - rX[0]) + magnitude(sy - rY[0]);
    for (int i = 1; i < n; i++) {
        int d = magnitude(sx - rX[i]) + magnitude(sy - rY[i]);
        if (d < min) { min = d; idx = i; }
    }
    return idx;
}

/* ---------- رویداد شانسی (جعبه‌های شانسی) ---------- */
void applyChance(int *extraTurn, int *wallNum, int row, int col,
                int rX[], int rY[], int R,
                int sX[], int sY[], int S,
                int **hWall, int **vWall,
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
        // حرکت نقشه‌بردارها
        for (int i = 0; i < R; i++) {
            int d = rand() % 4;
            if (d == 0 && rX[i] > 0 && !hWall[rX[i] - 1][rY[i]]) rX[i]--;
            else if (d == 1 && rX[i] < row - 1 && !hWall[rX[i]][rY[i]]) rX[i]++;
            else if (d == 2 && rY[i] > 0 && !vWall[rX[i]][rY[i] - 1]) rY[i]--;
            else if (d == 3 && rY[i] < col - 1 && !vWall[rX[i]][rY[i]]) rY[i]++;
        }
        // حرکت سایه‌نگرها
        for (int i = 0; i < S; i++) {
            int d = rand() % 4;
            if (d == 0 && sX[i] > 0 && !hWall[sX[i] - 1][sY[i]]) sX[i]--;
            else if (d == 1 && sX[i] < row - 1 && !hWall[sX[i]][sY[i]]) sX[i]++;
            else if (d == 2 && sY[i] > 0 && !vWall[sX[i]][sY[i] - 1]) sY[i]--;
            else if (d == 3 && sY[i] < col - 1 && !vWall[sX[i]][sY[i]]) sY[i]++;
        }
        // به‌روزرسانی هدف سایه‌نگرها
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

/* ---------- ذخیره بازی در فایل ---------- */
void saveGame(FILE *f,
             int row, int col, int wallNum, int R, int S, int gx, int gy,
             int **chanceCell, int **hWall, int **vWall,
             int rX[], int rY[], int sX[], int sY[], int assigned[]) {
    fwrite(&row, sizeof(int), 1, f);
    fwrite(&col, sizeof(int), 1, f);
    fwrite(&wallNum, sizeof(int), 1, f);
    fwrite(&R, sizeof(int), 1, f);
    fwrite(&S, sizeof(int), 1, f);
    fwrite(&gx, sizeof(int), 1, f);
    fwrite(&gy, sizeof(int), 1, f);
    
    // ذخیره آرایه‌های دو بعدی
    for (int i = 0; i < row; i++) {
        fwrite(chanceCell[i], sizeof(int), col, f);
        fwrite(hWall[i], sizeof(int), col, f);
        fwrite(vWall[i], sizeof(int), col, f);
    }
    
    // ذخیره آرایه‌های یک بعدی
    fwrite(rX, sizeof(int), R, f);
    fwrite(rY, sizeof(int), R, f);
    fwrite(sX, sizeof(int), S, f);
    fwrite(sY, sizeof(int), S, f);
    fwrite(assigned, sizeof(int), S, f);
}

/* ================== اصل بازی ================== */
int main() {
    srand(time(NULL));
    int row, col, wallNum, R, S;
    int gx, gy;
    
    // انتخاب نوع بازی
    printf("1. New Game\n2. Continue Saved Game\n");
    int choice;
    scanf("%d", &choice);
    
    FILE *f;
    if (choice == 2) {
        f = fopen("save.dat", "rb");
        if (!f) {
            printf("No saved game found!\n");
            return 0;
        }
        
        // خواندن اطلاعات بازی از فایل
        fread(&row, sizeof(int), 1, f);
        fread(&col, sizeof(int), 1, f);
        fread(&wallNum, sizeof(int), 1, f);
        fread(&R, sizeof(int), 1, f);
        fread(&S, sizeof(int), 1, f);
        fread(&gx, sizeof(int), 1, f);
        fread(&gy, sizeof(int), 1, f);
    } else {
        // دریافت اطلاعات بازی جدید
        printf("Enter number of rows (m): ");
        scanf("%d", &row);
        printf("Enter number of columns (n): ");
        scanf("%d", &col);
        printf("Enter number of walls: ");
        scanf("%d", &wallNum);
        printf("Enter number of mappers: ");
        scanf("%d", &R);
        printf("Enter number of shadowers: ");
        scanf("%d", &S);
    }
    
    // تخصیص حافظه برای آرایه‌های دو بعدی
    char **grid = malloc(row * sizeof(char *));
    int **chanceCell = malloc(row * sizeof(int *));
    int **hWall = malloc(row * sizeof(int *));
    int **vWall = malloc(row * sizeof(int *));
    
    for (int i = 0; i < row; i++) {
        grid[i] = malloc(col * sizeof(char));
        chanceCell[i] = calloc(col, sizeof(int));
        hWall[i] = calloc(col, sizeof(int));
        vWall[i] = calloc(col, sizeof(int));
    }
    
    // تعریف آرایه‌های موقعیت
    int rX[R], rY[R], sX[S], sY[S], assigned[S];
    
    // بارگذاری بازی ذخیره شده
    if (choice == 2) {
        for (int i = 0; i < row; i++) {
            fread(chanceCell[i], sizeof(int), col, f);
            fread(hWall[i], sizeof(int), col, f);
            fread(vWall[i], sizeof(int), col, f);
        }
        fread(rX, sizeof(int), R, f);
        fread(rY, sizeof(int), R, f);
        fread(sX, sizeof(int), S, f);
        fread(sY, sizeof(int), S, f);
        fread(assigned, sizeof(int), S, f);
        fclose(f);
        printf("Game loaded!\n");
    } else {
        // ساخت جعبه‌های شانسی
        int chance = (row * col) / 10;
        for (int i = 0; i < chance; i++) {
            int x, y;
            do { x = rand() % row; y = rand() % col; }
            while (chanceCell[x][y]);
            chanceCell[x][y] = 1;
        }
        
        // تعیین موقعیت هسته نور
        gx = rand() % row;
        gy = rand() % col;
        
        // تعیین موقعیت نقشه‌بردارها
        for (int i = 0; i < R; i++) {
            rX[i] = rand() % row;
            rY[i] = rand() % col;
        }
        
        // تعیین موقعیت سایه‌نگرها
        for (int i = 0; i < S; i++) {
            sX[i] = rand() % row;
            sY[i] = rand() % col;
            assigned[i] = -1;
        }
        
        // ساخت دیوارها
        do {
            for (int i = 0; i < row; i++)
                for (int j = 0; j < col; j++)
                    hWall[i][j] = vWall[i][j] = 0;
            
            int placedWalls = 0;
            while (placedWalls < wallNum) {
                int t = rand() % 2, r = rand() % row, c = rand() % col;
                if (t == 0 && r < row - 1 && !hWall[r][c]) {
                    hWall[r][c] = 1;
                    placedWalls++;
                }
                if (t == 1 && c < col - 1 && !vWall[r][c]) {
                    vWall[r][c] = 1;
                    placedWalls++;
                }
            }
        } while (!checkConnectivity(row, col, hWall, vWall));
    }
    
    // حلقه اصلی بازی
    while (1) {
        // بازسازی صفحه بازی
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
                grid[i][j] = chanceCell[i][j] ? '?' : '.';
        
        for (int i = 0; i < R; i++)
            grid[rX[i]][rY[i]] = 'A' + i;
        
        for (int i = 0; i < S; i++)
            grid[sX[i]][sY[i]] = 'S';
        
        grid[gx][gy] = 'H';
        showBoard(row, col, grid, hWall, vWall);
        
        // حرکت نقشه‌بردارها (یکی یکی)
        for (int player = 0; player < R; player++) {
            int extraTurn = 0;
            int skipShadowMovement = 0;
            
            // پیام واضح برای کاربر
            printf("\nPlayer %d's turn (W/A/S/D or -1 to save): ", player);
            int sel;
            scanf("%d", &sel);
            
            // اگر کاربر -1 وارد کند، بازی ذخیره می‌شود
            if (sel == -1) {
                f = fopen("save.dat", "wb");
                if (!f) {
                    printf("Error saving game!\n");
                    continue;
                }
                saveGame(f, row, col, wallNum, R, S, gx, gy,
                         chanceCell, hWall, vWall,
                         rX, rY, sX, sY, assigned);
                fclose(f);
                printf("Game saved!\n");
                continue;
            }
            
            // دریافت حرکت
            char mv;
            scanf(" %c", &mv);
            
            // محاسبه موقعیت جدید
            int nx = rX[player], ny = rY[player];
            if ((mv == 'W' || mv == 'w') && nx > 0 && !hWall[nx - 1][ny]) nx--;
            if ((mv == 'S' || mv == 's') && nx < row - 1 && !hWall[nx][ny]) nx++;
            if ((mv == 'A' || mv == 'a') && ny > 0 && !vWall[nx][ny - 1]) ny--;
            if ((mv == 'D' || mv == 'd') && ny < col - 1 && !vWall[nx][ny]) ny++;
            
            // به‌روزرسانی موقعیت
            rX[player] = nx;
            rY[player] = ny;
            
            // بررسی برنده شدن
            if (nx == gx && ny == gy) {
                printf("\nYou win!\n");
                remove("save.dat");
                return 0;
            }
            
            // بررسی جعبه شانسی
            if (chanceCell[nx][ny]) {
                chanceCell[nx][ny] = 0;
                applyChance(&extraTurn, &wallNum, row, col,
                            rX, rY, R, sX, sY, S, hWall, vWall, &skipShadowMovement, assigned);
            }
            
            // اگر نوبت اضافی داشت، این نقشه‌بردار دوباره حرکت می‌کند
            if (extraTurn) {
                player--; // برگشت به همان نقشه‌بردار
                continue;
            }
        }
        
        // حرکت سایه‌نگرها (همزمان)
        for (int i = 0; i < S; i++) {
            if (assigned[i] == -1)
                assigned[i] = closestRunner(sX[i], sY[i], rX, rY, R);
            
            int t = assigned[i];
            if (sX[i] < rX[t] && !hWall[sX[i]][sY[i]]) sX[i]++;
            else if (sX[i] > rX[t] && !hWall[sX[i] - 1][sY[i]]) sX[i]--;
            else if (sY[i] < rY[t] && !vWall[sX[i]][sY[i]]) sY[i]++;
            else if (sY[i] > rY[t] && !vWall[sX[i]][sY[i] - 1]) sY[i]--;
            
            // بررسی شکست
            if (sX[i] == rX[t] && sY[i] == rY[t]) {
                printf("\nGame over! Runner %d caught\n", t);
                remove("save.dat");
                return 0;
            }
        }
    }
}
