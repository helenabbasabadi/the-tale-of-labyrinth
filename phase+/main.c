#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define MAX_QUEUE 10000

/* Directions: up, down, left, right */
int dx[4] = {-1, 1, 0, 0};
int dy[4] = {0, 0, -1, 1};
char dirChars[4] = {'W', 'S', 'A', 'D'};

/* Manhattan distance */
int magnitude(int x) { return x < 0 ? -x : x; }

/* Display the game board */
void showBoard(int rows, int cols, char **field,
               int **hWall, int** vWall) {
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

/* Check full connectivity with BFS */
int checkConnectivity(int rows, int cols, int **hWall, int** vWall) {
    int **vis = malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        vis[i] = calloc(cols, sizeof(int));
    }

    int qx[MAX_QUEUE], qy[MAX_QUEUE];
    int head = 0, tail = 0;
    vis[0][0] = 1;
    qx[tail] = 0; qy[tail++] = 0;

    while (head < tail) {
        int x = qx[head];
        int y = qy[head++];
        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && !vis[nx][ny]) {
                int blocked = 0;
                if (d == 0) blocked = hWall[x - 1][y];
                else if (d == 1) blocked = hWall[x][y];
                else if (d == 2) blocked = vWall[x][y - 1];
                else if (d == 3) blocked = vWall[x][y];
                if (!blocked) {
                    vis[nx][ny] = 1;
                    qx[tail] = nx; qy[tail++] = ny;
                }
            }
        }
    }

    int connected = 1;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (!vis[i][j]) connected = 0;

    for (int i = 0; i < rows; i++) free(vis[i]);
    free(vis);
    return connected;
}

/* Find closest alive runner (Manhattan) */
int closestAliveRunner(int sx, int sy, int rX[], int rY[], int runnerAlive[], int n) {
    int bestIdx = -1;
    int minDist = 2147483647;
    for (int i = 0; i < n; i++) {
        if (!runnerAlive[i]) continue;
        int d = magnitude(sx - rX[i]) + magnitude(sy - rY[i]);
        if (d < minDist) {
            minDist = d;
            bestIdx = i;
        }
    }
    return bestIdx; /* -1 if no alive runner */
}

/* Find best next move using BFS (shortest path) */
void getBestNextPosition(int curr_x, int curr_y, int target_x, int target_y,
                         int rows, int cols, int **hWall, int **vWall,
                         int avoidHunters, int sX[], int sY[], int S,
                         int *new_x, int *new_y) {
    *new_x = curr_x;
    *new_y = curr_y;

    if (curr_x == target_x && curr_y == target_y) return;

    int **dist = malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        dist[i] = calloc(cols, sizeof(int));
        for (int j = 0; j < cols; j++) dist[i][j] = -1;
    }

    int qx[MAX_QUEUE], qy[MAX_QUEUE];
    int head = 0, tail = 0;
    dist[target_x][target_y] = 0;
    qx[tail] = target_x; qy[tail++] = target_y;

    while (head < tail) {
        int x = qx[head];
        int y = qy[head++];
        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && dist[nx][ny] == -1) {
                int blocked = 0;
                if (d == 0) blocked = hWall[x - 1][y];
                else if (d == 1) blocked = hWall[x][y];
                else if (d == 2) blocked = vWall[x][y - 1];
                else if (d == 3) blocked = vWall[x][y];

                /* Avoid hunter positions if requested */
                if (avoidHunters) {
                    for (int k = 0; k < S; k++) {
                        if (sX[k] == nx && sY[k] == ny) {
                            blocked = 1;
                            break;
                        }
                    }
                }

                if (!blocked) {
                    dist[nx][ny] = dist[x][y] + 1;
                    qx[tail] = nx; qy[tail++] = ny;
                }
            }
        }
    }

    if (dist[curr_x][curr_y] == -1) goto cleanup; /* No path → stay */

    /* Find first neighbor with smaller distance */
    for (int d = 0; d < 4; d++) {
        int nx = curr_x + dx[d];
        int ny = curr_y + dy[d];
        if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
            int blocked = 0;
            if (d == 0) blocked = hWall[curr_x - 1][curr_y];
            else if (d == 1) blocked = hWall[curr_x][curr_y];
            else if (d == 2) blocked = vWall[curr_x][curr_y - 1];
            else if (d == 3) blocked = vWall[curr_x][curr_y];

            if (avoidHunters) {
                for (int k = 0; k < S; k++) {
                    if (sX[k] == nx && sY[k] == ny) {
                        blocked = 1;
                        break;
                    }
                }
            }

            if (!blocked && dist[nx][ny] == dist[curr_x][curr_y] - 1) {
                *new_x = nx;
                *new_y = ny;
                goto cleanup;
            }
        }
    }

cleanup:
    for (int i = 0; i < rows; i++) free(dist[i]);
    free(dist);
}

/* Chance events */
void applyChance(int *extraTurn, int wallNum, int row, int col,
                 int rX[], int rY[], int R,
                 int sX[], int sY[], int S,
                 int **hWall, int** vWall,
                 int *skipShadowMovement) {
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
            int nx = rX[i] + dx[d];
            int ny = rY[i] + dy[d];
            if (nx >= 0 && nx < row && ny >= 0 && ny < col) {
                int blocked = (d == 0 ? hWall[rX[i]-1][rY[i]] :
                              d == 1 ? hWall[rX[i]][rY[i]] :
                              d == 2 ? vWall[rX[i]][rY[i]-1] :
                                       vWall[rX[i]][rY[i]]);
                if (!blocked) { rX[i] = nx; rY[i] = ny; }
            }
        }
        for (int i = 0; i < S; i++) {
            int d = rand() % 4;
            int nx = sX[i] + dx[d];
            int ny = sY[i] + dy[d];
            if (nx >= 0 && nx < row && ny >= 0 && ny < col) {
                int blocked = (d == 0 ? hWall[sX[i]-1][sY[i]] :
                              d == 1 ? hWall[sX[i]][sY[i]] :
                              d == 2 ? vWall[sX[i]][sY[i]-1] :
                                       vWall[sX[i]][sY[i]]);
                if (!blocked) { sX[i] = nx; sY[i] = ny; }
            }
        }
    } else {
        printf("Chance: Move a Shadow!\n");
        int id, dir;
        printf("Shadow index (0-%d): ", S - 1);
        scanf("%d", &id);
        printf("Direction (0=Up 1=Down 2=Left 3=Right): ");
        scanf("%d", &dir);
        int nx = sX[id] + dx[dir];
        int ny = sY[id] + dy[dir];
        if (nx >= 0 && nx < row && ny >= 0 && ny < col) {
            int blocked = (dir == 0 ? hWall[sX[id]-1][sY[id]] :
                          dir == 1 ? hWall[sX[id]][sY[id]] :
                          dir == 2 ? vWall[sX[id]][sY[id]-1] :
                                     vWall[sX[id]][sY[id]]);
            if (!blocked) { sX[id] = nx; sY[id] = ny; }
        }
        *skipShadowMovement = 1;
    }
}

/* Save game */
void saveGame(FILE *f, int row, int col, int wallNum, int R, int S, int gx, int gy,
              int **chanceCell, int** hWall, int **vWall,
              int rX[], int rY[], int sX[], int sY[], int runnerAlive[]) {
    fwrite(&row, sizeof(int), 1, f);
    fwrite(&col, sizeof(int), 1, f);
    fwrite(&wallNum, sizeof(int), 1, f);
    fwrite(&R, sizeof(int), 1, f);
    fwrite(&S, sizeof(int), 1, f);
    fwrite(&gx, sizeof(int), 1, f);
    fwrite(&gy, sizeof(int), 1, f);
    for (int i = 0; i < row; i++) {
        fwrite(chanceCell[i], sizeof(int), col, f);
        fwrite(hWall[i], sizeof(int), col, f);
        fwrite(vWall[i], sizeof(int), col, f);
    }
    fwrite(rX, sizeof(int), R, f);
    fwrite(rY, sizeof(int), R, f);
    fwrite(sX, sizeof(int), S, f);
    fwrite(sY, sizeof(int), S, f);
    fwrite(runnerAlive, sizeof(int), R, f);
}

int main() {
    srand(time(NULL));

    int row, col, wallNum, R, S, gx, gy;
    printf("1. New Game\n2. Continue Saved Game\n");
    int choice;
    scanf("%d", &choice);

    char **grid;
    int **chanceCell, **hWall, **vWall;
    int *rX, *rY, *sX, *sY, *runnerAlive;

    FILE *f = NULL;
    if (choice == 2) {
        f = fopen("save.dat", "rb");
        if (!f) {
            printf("No saved game found!\n");
            return 0;
        }
        fread(&row, sizeof(int), 1, f);
        fread(&col, sizeof(int), 1, f);
        fread(&wallNum, sizeof(int), 1, f);
        fread(&R, sizeof(int), 1, f);
        fread(&S, sizeof(int), 1, f);
        fread(&gx, sizeof(int), 1, f);
        fread(&gy, sizeof(int), 1, f);
    } else {
        printf("Enter number of rows (m): ");
        scanf("%d", &row);
        printf("Enter number of columns (n): ");
        scanf("%d", &col);
        printf("Enter number of walls: ");
        scanf("%d", &wallNum);
        printf("Enter number of runners: ");
        scanf("%d", &R);
        printf("Enter number of shadowers: ");
        scanf("%d", &S);
    }

    /* Memory allocation */
    grid = malloc(row * sizeof(char*));
    chanceCell = malloc(row * sizeof(int*));
    hWall = malloc(row * sizeof(int*));
    vWall = malloc(row * sizeof(int*));
    for (int i = 0; i < row; i++) {
        grid[i] = malloc(col * sizeof(char));
        chanceCell[i] = calloc(col, sizeof(int));
        hWall[i] = calloc(col, sizeof(int));
        vWall[i] = calloc(col, sizeof(int));
    }
    rX = malloc(R * sizeof(int));
    rY = malloc(R * sizeof(int));
    sX = malloc(S * sizeof(int));
    sY = malloc(S * sizeof(int));
    runnerAlive = calloc(R, sizeof(int));

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
        fread(runnerAlive, sizeof(int), R, f);
        fclose(f);
        printf("Game loaded!\n");
    } else {
        /* Chance cells */
        int chance_num = (row * col) / 10;
        for (int i = 0; i < chance_num; i++) {
            int x, y;
            do {
                x = rand() % row; y = rand() % col;
            } while (chanceCell[x][y]);
            chanceCell[x][y] = 1;
        }

        /* Light core (*) */
        gx = rand() % row;
        gy = rand() % col;

        /* Runners - no overlap */
        for (int i = 0; i < R; i++) {
            int x, y;
            int overlap;
            do {
                x = rand() % row;
                y = rand() % col;
                overlap = (x == gx && y == gy);
                for (int j = 0; j < i; j++) {
                    if (rX[j] == x && rY[j] == y) {
                        overlap = 1;
                        break;
                    }
                }
            } while (overlap);
            rX[i] = x;
            rY[i] = y;
            runnerAlive[i] = 1;
        }

        /* Shadowers - no overlap */
        for (int i = 0; i < S; i++) {
            int x, y;
            int overlap;
            do {
                x = rand() % row;
                y = rand() % col;
                overlap = (x == gx && y == gy);
                for (int j = 0; j < R; j++) {
                    if (rX[j] == x && rY[j] == y) {
                        overlap = 1;
                        break;
                    }
                }
                if (!overlap) {
                    for (int j = 0; j < i; j++) {
                        if (sX[j] == x && sY[j] == y) {
                            overlap = 1;
                            break;
                        }
                    }
                }
            } while (overlap);
            sX[i] = x;
            sY[i] = y;
        }

        /* Walls with connectivity */
        do {
            for (int i = 0; i < row; i++)
                for (int j = 0; j < col; j++)
                    hWall[i][j] = vWall[i][j] = 0;
            int placed = 0;
            while (placed < wallNum) {
                int t = rand() % 2;
                int r = rand() % row;
                int c = rand() % col;
                if (t == 0 && r < row - 1 && !hWall[r][c]) {
                    hWall[r][c] = 1; placed++;
                } else if (t == 1 && c < col - 1 && !vWall[r][c]) {
                    vWall[r][c] = 1; placed++;
                }
            }
        } while (!checkConnectivity(row, col, hWall, vWall));
    }

    while (1) {
        /* Rebuild display grid - only alive runners */
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
                grid[i][j] = chanceCell[i][j] ? '?' : '.';
        for (int i = 0; i < R; i++)
            if (runnerAlive[i])
                grid[rX[i]][rY[i]] = 'A' + i;
        for (int i = 0; i < S; i++)
            grid[sX[i]][sY[i]] = 'a' + i;
        grid[gx][gy] = '*';

        showBoard(row, col, grid, hWall, vWall);

        int skipShadowMovement = 0;

        /* Runners' turns (only alive ones) */
        for (int player = 0; player < R; player++) {
            if (!runnerAlive[player]) continue;

            int extraTurn = 0;

            /* Safe suggestion: avoid current hunter positions */
            int newx, newy;
            getBestNextPosition(rX[player], rY[player], gx, gy, row, col, hWall, vWall,
                                1, sX, sY, S, &newx, &newy);
            if (newx != rX[player] || newy != rY[player]) {
                char suggested = 0;
                for (int d = 0; d < 4; d++) {
                    if (rX[player] + dx[d] == newx && rY[player] + dy[d] == newy) {
                        suggested = dirChars[d];
                        break;
                    }
                }
                if (suggested)
                    printf("Suggested safe move to reach *: %c\n", suggested);
            } else if (rX[player] != gx || rY[player] != gy) {
                printf("No safe path to * currently.\n");
            }

            /* Get move */
            char mv;
            do {
                printf("\nRunner %c's turn. Move (W/A/S/D) or Q to save: ", 'A' + player);
                scanf(" %c", &mv);
                mv = toupper(mv);
                if (mv == 'Q') {
                    f = fopen("save.dat", "wb");
                    if (f) {
                        saveGame(f, row, col, wallNum, R, S, gx, gy,
                                 chanceCell, hWall, vWall, rX, rY, sX, sY, runnerAlive);
                        fclose(f);
                        printf("Game saved successfully!\n");
                    } else {
                        printf("Error saving game!\n");
                    }
                } else if (mv != 'W' && mv != 'A' && mv != 'S' && mv != 'D') {
                    printf("Invalid input!\n");
                }
            } while (mv == 'Q' || (mv != 'W' && mv != 'A' && mv != 'S' && mv != 'D'));

            /* Apply move */
            int nx = rX[player], ny = rY[player];
            if (mv == 'W' && nx > 0 && !hWall[nx - 1][ny]) nx--;
            if (mv == 'S' && nx < row - 1 && !hWall[nx][ny]) nx++;
            if (mv == 'A' && ny > 0 && !vWall[nx][ny - 1]) ny--;
            if (mv == 'D' && ny < col - 1 && !vWall[nx][ny]) ny++;

            rX[player] = nx;
            rY[player] = ny;

            /* Win if any runner reaches * */
            if (nx == gx && ny == gy) {
                printf("\nCongratulations! Runner %c reached the light core! You win!\n", 'A' + player);
                remove("save.dat");
                return 0;
            }

            /* Chance cell */
            if (chanceCell[nx][ny]) {
                chanceCell[nx][ny] = 0;
                applyChance(&extraTurn, wallNum, row, col,
                            rX, rY, R, sX, sY, S,
                            hWall, vWall, &skipShadowMovement);
            }

            if (extraTurn) {
                player--; /* Repeat this player's turn */
                continue;
            }
        }

        /* Shadowers' movement */
        if (!skipShadowMovement) {
            int allCaught = 1;
            for (int i = 0; i < S; i++) {
                int target = closestAliveRunner(sX[i], sY[i], rX, rY, runnerAlive, R);
                if (target == -1) continue; /* No alive runner */

                allCaught = 0;

                int nx, ny;
                getBestNextPosition(sX[i], sY[i], rX[target], rY[target], row, col, hWall, vWall,
                                    0, NULL, NULL, 0, &nx, &ny);
                sX[i] = nx;
                sY[i] = ny;

                if (sX[i] == rX[target] && sY[i] == rY[target]) {
                    printf("\nRunner %c was caught by a shadower!\n", 'A' + target);
                    runnerAlive[target] = 0;
                }
            }

            /* Check if all runners are caught */
            if (allCaught) {
                printf("\nAll runners caught! Game Over!\n");
                remove("save.dat");
                return 0;
            }
        }
    }

    return 0;
}
