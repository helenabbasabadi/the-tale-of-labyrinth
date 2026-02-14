#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int map[50][50];
int visited[50][50];

//تابع جایگذاری مهره ها که ما انها را عدد در نظر گرفتیم
void Tset(int map[][50], int r, int c, int number){
    map[r][c] = number;
}

//‌‌این تابع چک می کند ایا دیوار دو لایه است یا نه
int Tcheck(int r,int c,int R[],int C[],int count){
    for(int i=0;i<count;i++)
        if(R[i]==r && C[i]==c) return 1;
    return 0;
}

/*
hr,hc : دیوار افقی ضخیم بین (r,c) و (r+1,c)
vr,vc : دیوار عمودی ضخیم بین (r,c) و (r,c+1)
*/

void DFS(int r,int c,int n,int m,
         int hr[],int hc[],int hCount,
         int vr[],int vc[],int vCount){

    if(r<0 || r>=n || c<0 || c>=m) return;
    if(visited[r][c]) return;

    visited[r][c] = 1;

    if(r>0 && !Tcheck(r-1,c,hr,hc,hCount))
        DFS(r-1,c,n,m,hr,hc,hCount,vr,vc,vCount);

    if(r<n-1 && !Tcheck(r,c,hr,hc,hCount))
        DFS(r+1,c,n,m,hr,hc,hCount,vr,vc,vCount);

    if(c>0 && !Tcheck(r,c-1,vr,vc,vCount))
        DFS(r,c-1,n,m,hr,hc,hCount,vr,vc,vCount);

    if(c<m-1 && !Tcheck(r,c,vr,vc,vCount))
        DFS(r,c+1,n,m,hr,hc,hCount,vr,vc,vCount);
}

int DFScheck(int n,int m,
             int hr[],int hc[],int hCount,
             int vr[],int vc[],int vCount){

    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++)
            visited[i][j] = 0;

    DFS(0,0,n,m,hr,hc,hCount,vr,vc,vCount);

    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++)
            if(!visited[i][j]) return 0;

    return 1;
}

// فاصله حداقلی بین اعداد
int checkdistance(int arr[][50],int n,int m){
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            if(arr[i][j] != -1){
                for(int x=-1;x<=1;x++){
                    for(int y=-1;y<=1;y++){
                        if(x==0 && y==0) continue;
                        int ni=i+x, nj=j+y;
                        if(ni>=0 && ni<n && nj>=0 && nj<m)
                            if(arr[ni][nj] != -1) return 0;
                    }
                }
            }
        }
    }
    return 1;
}

int main(){

    int n,m,numWalls,numMappers,numWatchers;
    int hr[200],hc[200],hCount;
    int vr[200],vc[200],vCount;
    int r,c;

    srand(time(NULL));

    scanf("%d%d",&n,&m);
    scanf("%d",&numWalls);
    scanf("%d",&numMappers);
    scanf("%d",&numWatchers);

    do{
        hCount = vCount = 0;

        for(int i=0;i<n;i++)
            for(int j=0;j<m;j++)
                map[i][j] = -1;

        // دیوارهای دو لایه
        for(int i=0;i<numWalls;i++){
            if(rand()%2){
                r = rand()%(n-1); c = rand()%m;
                hr[hCount]=r; hc[hCount]=c; hCount++;
            }else{
                r = rand()%n; c = rand()%(m-1);
                vr[vCount]=r; vc[vCount]=c; vCount++;
            }
        }

        // هسته نور
        r=rand()%n; c=rand()%m;
        Tset(map,r,c,2);

        // نقشه‌بردار
        for(int i=0;i<numMappers;i++){
            do{ r=rand()%n; c=rand()%m; }while(map[r][c]!=-1);
            Tset(map,r,c,0);
        }

        // سایه‌نگر
        for(int i=0;i<numWatchers;i++){
            do{ r=rand()%n; c=rand()%m; }while(map[r][c]!=-1);
            Tset(map,r,c,1);
        }

    }while(!DFScheck(n,m,hr,hc,hCount,vr,vc,vCount)
        || !checkdistance(map,n,m));

    //چاپ جدول
    for(int i=0;i<n;i++){

        // خط افقی بالا
        for(int j=0;j<m;j++){
            printf("+");
            if(Tcheck(i,j,hr,hc,hCount)) printf("====");
            else printf("----");
        }
        printf("+\n");

        // خط محتوا
        for(int j=0;j<m;j++){
            if(j>0 && Tcheck(i,j-1,vr,vc,vCount)) printf("||");
            else printf("| ");

            if(map[i][j]==-1) printf("  ");
            else printf("%d ",map[i][j]);
        }
        printf("|\n"); // دیوار سمت راست
    }

    // خط پایانی
    for(int j=0;j<m;j++) printf("+----");
    printf("+\n");

    return 0;
}
