#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define STOCK 1
#define CART 0
#define MAX_NO_OF_PRODUCTS 1000

typedef struct product{
    int prodID;
    char prodName[20];
    int prodPrice; 
    int prodQuantity;
} product;
typedef struct cart{
    struct product cartItems[MAX_NO_OF_PRODUCTS];
} cart;

void printProductDetails(product p,int flag){
    // flag => 0 for cart ; 1 for stock
    if(flag == 0) printf("Product ID: %d, Name: %s, Price: %d, Quantity in cart: %d\n",p.prodID,p.prodName,p.prodPrice,p.prodQuantity);
    else if(flag == 1) printf("Product ID: %d, Name: %s, Price: %d, Quantity in stock: %d\n",p.prodID,p.prodName,p.prodPrice,p.prodQuantity);
    else printf("illegal flag");
}

void printProdArr(int counter,product prodArr[]){
    for(int i=0;i<counter;i++){
        printProductDetails(prodArr[i],STOCK);
    }
}

product * createProduct(int id,char name[],int price,int quantity){
    product * p1 = malloc(sizeof(product));
    p1->prodID = id;
    strcpy(p1->prodName,name);
    p1->prodPrice = price;
    p1->prodQuantity = quantity;
    return p1; 
}

int main(){
    //setting id counter 
        int prodID_counter =10;
        int store_fd =open("store.dat",O_RDWR | O_CREAT , 0744);
        write(store_fd,&prodID_counter,sizeof(int));
        close(store_fd);

        product parr[MAX_NO_OF_PRODUCTS];
        for(int i=0;i<MAX_NO_OF_PRODUCTS;i++){
            parr[i].prodID = -1;
            strcpy( parr[i].prodName,"") ;
            parr[i].prodPrice = -1;
            parr[i].prodQuantity = -1;
        }
        int prod_fd = open("product.dat",O_CREAT | O_RDWR, 0744);
        write(prod_fd,parr,sizeof(parr));    
        close(prod_fd);

        //populating data
        product pArr[10];
        pArr[0] = *createProduct(0,"Akahnda",150,21);
        pArr[1] = *createProduct(1,"Legend",500,15);
        pArr[2] = *createProduct(2,"Simha",200,10);
        pArr[3] = *createProduct(3,"Jai Simha",178,12);
        pArr[4] = *createProduct(4,"Chennakesava Reddy",900,6);
        pArr[5] = *createProduct(5,"Veera Simha Reddy",123,11);
        pArr[6] = *createProduct(6,"Paisa Vasool",90,16);
        pArr[7] = *createProduct(7,"Rowdy Inspector",110,14);
        pArr[8] = *createProduct(8,"Ruler",120,5);
        pArr[9] = *createProduct(9,"NTR Kathanayakudu",157,10);

        prod_fd = open("product.dat", O_RDWR);
        write(prod_fd,pArr,sizeof(pArr));    
        close(prod_fd);
        //printProdArr(10,pArr);
    // int fd = open("product.dat",O_RDWR);
    // lseek(fd,sizeof(product),SEEK_SET);
    // product * p = malloc(sizeof(product));
    // read(fd,p,sizeof(product));
    // printProductDetails(*p,STOCK);

}