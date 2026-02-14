#include <stdio.h>


//تابع جایگزاری اعداد در جدول
void Tset(int arr[50][50],int r,int c,int number){
    arr[r][c]=number;
}


//چک می‌کند آیا دیوار ضخیم است یا نه
int Tcheck(int r,int c,int R[],int C[],int count){
    for(int i=0;i<count;i++){
        if(R[i]==r && C[i]==c) return 1;
    }
    return 0;
}



int main(){

    int n,m;
    int hr[100],hc[100],hCount;
    int vr[100],vc[100],vCount;
    int arr[50][50];

    printf("row : ");
    scanf("%d",&n);

    printf("column : ");
    scanf("%d",&m);

    for(int i=0;i<n;i++){ //مقداردهی اولیه
        for(int j=0;j<m;j++){
            arr[i][j]=-1;//خانه خالی
        }
    }


    printf("How many horizontal thick walls? : "); //چند دیوار افقی ضخیم؟
    scanf("%d",&hCount);

    printf("Enter the coordinates of the horizontal walls : "); //مختصات دیوارهای افقی را وارد کنید
    for(int i=0;i<hCount;i++){
        scanf("%d %d",&hr[i],&hc[i]);
    }

    printf("How many vertical thick walls? : "); //چند دیوار عمودی ضخیم؟
    scanf("%d",&vCount);

    printf("Enter the coordinates of the vertical walls : "); //مختصات دیوارهای عمودی را وارد کنید
    for(int i=0;i<vCount;i++){
        scanf("%d %d",&vr[i],&vc[i]);
    }

    //منظور از ۲ همان موقعیت خانه هسته نور است
    int r2,c2;
    printf("Enter position of 2 : "); //مختصات عدد ۲
    scanf("%d %d",&r2,&c2);
    Tset(arr,r2,c2,2); //جایگزاری ۲ در مختصات داده شده


    //منظور از ۰ همان موقعیت خانه نقشه بردار است
    int zCount;
    printf("How many zeros? ");//چند صفر؟
    scanf("%d",&zCount);

    for(int i=0;i<zCount;i++){
        int r,c;
        scanf("%d %d",&r,&c); //مختصات عدد ۰
        Tset(arr,r,c,0); //جایگزاری ۰ در مختصات داده شده
    }


    //منظور از ۱ همان موقعیت خانه سایه نگر است

    int oCount;
    printf("How many ones? "); //چند ۱؟
    scanf("%d",&oCount);


    for(int i=0;i<oCount;i++){
        int r,c;
        scanf("%d %d",&r,&c); //مختصات عدد ۱
        Tset(arr,r,c,1); //جایگزاری ۱ در مختصات داده شده
    }


    //چاپ جدول کامل
    for(int i=0;i<n;i++){

        for(int j=0;j<m;j++){ //خط افقی
            printf("+");
            if(Tcheck(i,j,hr,hc,hCount))
                printf("======");
            else
                printf("-----");
        }
        printf("+\n");

        for(int j=0;j<m;j++){ //خط عمودی + عدد داخل خانه
            if(Tcheck(i,j,vr,vc,vCount))
                printf("‖  ");
            else
                printf("|  ");
            if(arr[i][j]==-1)
                    printf("   ");
            else
                printf("%d  ", arr[i][j]);
        }
        printf("|\n");

    }

    for(int j=0;j<m;j++){ //خط افقی آخر
        printf("+");
        if(Tcheck(n,j,hr,hc,hCount))
            printf("======");
        else
            printf("-----");
    }
    printf("+\n");



return 0;

}
