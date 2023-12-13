#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#define MAX_NO_OF_PRODUCTS 1000
#define STOCK 1
#define CART 0

typedef struct product
{
    int prodID;
    char prodName[20];
    int prodPrice;
    int prodQuantity;
} product;
#define SIZE_PRODUCT sizeof(product)
product *createProduct(int id, char name[], int price, int quantity)
{
    product *p1 = malloc(SIZE_PRODUCT);
    p1->prodID = id;
    strcpy(p1->prodName, name);
    p1->prodPrice = price;
    p1->prodQuantity = quantity;
    return p1;
}

void printProductDetails(product p, int flag)
{
    // flag => 0 for cart ; 1 for stock
    if (flag == 0)
        printf("Product ID: %d, Name: %s, Price: %d, Quantity in cart: %d\n", p.prodID, p.prodName, p.prodPrice, p.prodQuantity);
    else if (flag == 1)
        printf("Product ID: %d, Name: %s, Price: %d, Quantity in stock: %d\n", p.prodID, p.prodName, p.prodPrice, p.prodQuantity);
    else
        printf("illegal flag");
}

int addProduct(product * p){
    printf("Connecting to server...\n");
    int cd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int status = connect(cd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (status == -1)
    {
        printf("error connecting to server");
        return -1;
    }

    int key;
    read(cd, &key, sizeof(key));
    if (key == 101){
        printf("Connected to server!\n");
        int cReq = 11;
        write(cd, &cReq, sizeof(int));
        printf("req sent please wait\n");
        write(cd,p,SIZE_PRODUCT);
        int new_id;
        read(cd,&new_id,sizeof(int));
        return new_id;
    }
    return -1;
}

int addStock(int id,int quantity){
    printf("Connecting to server...\n");
    int cd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int status = connect(cd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (status == -1)
    {
        printf("error connecting to server");
        return -1;
    }

    int key;
    read(cd, &key, sizeof(key));
    if (key == 101){
        int arr[2] = {id,quantity};
        printf("Connected to server!\n");
        int cReq = 12;
        write(cd, &cReq, sizeof(int));
        printf("req sent please wait\n");
        write(cd,arr,sizeof(arr));
        int new_quantity;
        read(cd,&new_quantity,sizeof(int));
        return new_quantity;
    }
    return -1;
}

int changePrice(int id,int price){
        printf("Connecting to server...\n");
    int cd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int status = connect(cd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (status == -1)
    {
        printf("error connecting to server");
        return -1;
    }

    int key;
    read(cd, &key, sizeof(key));
    if (key == 101){
        int arr[2] = {id,price};
        printf("Connected to server!\n");
        int cReq = 13;
        write(cd, &cReq, sizeof(int));
        write(cd,arr,sizeof(arr));
        int status;
        read(cd,&status,sizeof(int));
        return status;
    }
    return -1;
}

int main(){
    int choice;
    while(1){
        printf("\n################### ADMIN MENU ###################\n");
        printf("0 to EXIT\n");
        printf("Enter 1 to ADD A PRODUCT\n");
        printf("Enter 2 to CHANGE STOCK OF A PRODUCT\n");
        printf("Enter 3 to CHANGE PRICE OF A PRODUCT\n"); 
        printf("\n##################################################\n");
        scanf("%d",&choice);
        if (choice == 0)
        {
            break;
        }
        else if(choice == 1){
            printf("\n@@@@@@@@@@@@@ ADD A PRODUCT @@@@@@@@@@@@@\n");

            int price,quantity;
            char name [20] ;
            printf("Enter product name: \n");
            scanf("%s",name);
            printf("Enter Product price: ");
            scanf("%d",&price);
            printf("Enter Product quantity: ");
            scanf("%d",&quantity);
            product * p = createProduct(-1,name,price,quantity);
            int new_id = addProduct(p);
            printf("new ID : %d\n",new_id);
            //writing in log
            char  log[] = "Added New PRODUCT => ";
            char newline = '\n';
            FILE * fp = fopen("Adminlog.txt","a");
            fseek(fp,0,SEEK_END);
            fprintf(fp,"%s ",log);
            fprintf(fp," %d ",new_id);
            fprintf(fp," %s ",name);
            fprintf(fp," %d ",price);
            fprintf(fp," %d \n",quantity);
            fclose(fp);

            printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        }
        else if(choice == 2){
            printf("\n@@@@@@@@@@@@@@@ ADD STOCK @@@@@@@@@@@@@@@\n");
            int id,quantity;
            printf("Enter Product ID: ");
            scanf("%d",&id);

            printf("Enter Product quantity to be added: ");
            scanf("%d",&quantity);

            int new_quantity = addStock(id,quantity);
            printf("New quantity for id: %d, is %d\n",id,new_quantity);

            char  log[] = "Changed Product quantity => ";
            char newline = '\n';
            FILE * fp = fopen("Adminlog.txt","a");
            fseek(fp,0,SEEK_END);
            fprintf(fp,"%s ",log);
            fprintf(fp," %d ",id);
            fprintf(fp," %d \n",quantity);
            fclose(fp);

            printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        }
        else if(choice == 3){
            printf("\n@@@@@@@@@@@@@ CHANGE PRICE @@@@@@@@@@@@@@\n");
            int id,price;
            printf("Enter Product ID: ");
            scanf("%d",&id);

            printf("Enter Product price (new): ");
            scanf("%d",&price);

            int status = changePrice(id,price);
            if(status == 0 ){
            char  log[] = "Changed Product price => ";
            char newline = '\n';
            FILE * fp = fopen("Adminlog.txt","a");
            fseek(fp,0,SEEK_END);
            fprintf(fp,"%s ",log);
            fprintf(fp," %d ",id);
            fprintf(fp," %d \n",price);
            fclose(fp);
            }
            printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        }
        else{
            printf("INVALID INPUT\n");
        }    
    }
   
    return 0;
}