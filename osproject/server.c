#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define STORE_FILE "store.dat"
#define PRODUCT_FILE "product.dat"
#define MAX_NO_OF_PRODUCTS 1000
#define STOCK 1
#define CART 0
typedef struct product{
    int prodID;
    char prodName[20];
    int prodPrice; 
    int prodQuantity;
} product;
#define SIZE_PRODUCT sizeof(product)

product * createProduct(int id,char name[],int price,int quantity){
    product * p1 = malloc(SIZE_PRODUCT);
    p1->prodID = id;
    strcpy(p1->prodName,name);
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

void readAllProducts(int  counter,product * pArr){
    
    int prod_fd = open(PRODUCT_FILE,O_RDWR);

    for(int i=0;i<MAX_NO_OF_PRODUCTS;i++){
        pArr[i].prodID =-1;
    }
    //lock all?
    //lock counter?
    struct flock lock;
    lock.l_pid = getpid();
    lock.l_type =F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start =0;
    lock.l_len = SIZE_PRODUCT * MAX_NO_OF_PRODUCTS;
    fcntl(prod_fd,F_SETLKW,&lock);
    printf("FILE LOCKED\n");
    
    read(prod_fd,pArr,SIZE_PRODUCT*MAX_NO_OF_PRODUCTS);

    lock.l_type = F_UNLCK;
    fcntl(prod_fd,F_SETLK,&lock);
    printf("FILE UNLOCKED");
    //unlock
    close(prod_fd);
    
}

product * addToCart(int * prodID_counter,int * arr){
        product * p = malloc(SIZE_PRODUCT);
        int fd = open(PRODUCT_FILE,O_RDWR);
        //lock only one prod?
        //lock counter?
        struct flock lock;
        lock.l_pid = getpid();
        lock.l_type =F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start =0;
        lock.l_len = SIZE_PRODUCT * MAX_NO_OF_PRODUCTS;
        fcntl(fd,F_SETLKW,&lock);
        printf("FILE LOCKED\n");

        if (arr[0] >= *prodID_counter) {
            p = createProduct(-1,"INVALID ID",-1,-1);
        }
        else{
            
            lseek(fd,SIZE_PRODUCT*arr[0],SEEK_SET);
            read(fd,p,SIZE_PRODUCT);
            if(p->prodQuantity < arr[1]){
                p = createProduct(-1,"INSUFFICIENT STOCK",-1,-1);
            }
            else{
                lseek(fd,SIZE_PRODUCT*arr[0],SEEK_SET);
                p->prodQuantity -= arr[1];
                write(fd,p,SIZE_PRODUCT);
                p->prodQuantity = arr[1];
            } 
        }

        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK,&lock);
        printf("FILE UNLOCKED\n");
        //unlock 
        close (fd);
        return p;
           
}

int updateCart(int *counter,int * arr){
    int id,change,result;
    /* RESULT :
    * 0 => SUCCESS
    * 1 => ID not in store
    * 2 => Insufficient stock
    */

    id = arr[0];
    change = arr[1];
    printf("id:%d, change:%d\n",id,change);
    product * p = malloc(SIZE_PRODUCT);
    int fd = open(PRODUCT_FILE,O_RDWR);
    lseek(fd,SIZE_PRODUCT*id,SEEK_SET);
    //lock
    struct flock lock;
    lock.l_pid = getpid();
    lock.l_type =F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start =0;
    lock.l_len = SIZE_PRODUCT * MAX_NO_OF_PRODUCTS;
    fcntl(fd,F_SETLKW,&lock);
    printf("FILE LOCKED\n");

    if(arr[0] >= *counter){
        result = 1;
    }


    read(fd,p,SIZE_PRODUCT);  
    printf("%d %d\n",p->prodID,p->prodQuantity);
    lseek(fd,SIZE_PRODUCT*id,SEEK_SET);
    if(change < 0){
        change = abs(change);
        p->prodQuantity += change ;
        printf("%d %d\n",p->prodID,p->prodQuantity);
        write(fd,p,SIZE_PRODUCT);

        result = 0;
    }
    else if(change > p->prodQuantity){
        
        result = 2;
    }
    else{
        p->prodQuantity -= change;
        printf("%d %d\n",p->prodID,p->prodQuantity);
        write(fd,p,SIZE_PRODUCT);

        result = 0;
    }

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    printf("FILE UNLOCKED\n");
    //unlock
    close(fd);
    printf("%d",result);
    return result;   
}

void clientExit(int cart_size,product * Cart){
    int id,quantity;
    product * p = malloc(SIZE_PRODUCT);
    int fd = open(PRODUCT_FILE, O_RDWR);

    struct flock lock;
    lock.l_pid = getpid();
    lock.l_type =F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start =0;
    lock.l_len = SIZE_PRODUCT * MAX_NO_OF_PRODUCTS;
    fcntl(fd,F_SETLKW,&lock); 
printf("FILE LOCKED\n");
    for(int ndx = 0;ndx < cart_size;ndx++){
        id = Cart[ndx].prodID;
        quantity = Cart[ndx].prodQuantity;
        lseek(fd,id * SIZE_PRODUCT,SEEK_SET);
        //lock      

        read(fd,p,SIZE_PRODUCT);
        lseek(fd,id * SIZE_PRODUCT,SEEK_SET);
        p->prodQuantity += quantity;
        write(fd,p,SIZE_PRODUCT); 


        //unlock
    }

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
printf("FILE UNLOCKED\n");
    close(fd);
}

void prePaymentCheck(int cart_size,product * Cart,int * changeInPrice){
    int id,price;
    product * p=malloc(SIZE_PRODUCT); 
    int fd = open(PRODUCT_FILE,O_RDWR);

    
    
    struct flock lock;
    lock.l_pid = getpid();
    lock.l_type =F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start =0;
    lock.l_len = SIZE_PRODUCT * MAX_NO_OF_PRODUCTS;
    fcntl(fd,F_SETLKW,&lock); 
printf("FILE LOCKED\n");
    for(int ndx = 0;ndx<cart_size;ndx ++){
        id = Cart[ndx].prodID;
        price = Cart[ndx].prodPrice;
        if(id == -1) break;
            
        //lock
        lseek(fd,SIZE_PRODUCT * id,SEEK_SET);
        read(fd,p,SIZE_PRODUCT);
 
        if(p->prodPrice == price) changeInPrice[ndx] = -1;
        else changeInPrice[ndx] = p->prodPrice;
        //unlock
    }

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
printf("FILE UNLOCKED\n");
    close(fd);
}
//Admin Functions
int addProduct(product * p,int  counter){
    int fd_p = open(PRODUCT_FILE,O_RDWR);
    
    //lock
    struct flock lock;
    lock.l_pid = getpid();
    lock.l_type =F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start =0;
    lock.l_len = SIZE_PRODUCT * MAX_NO_OF_PRODUCTS;
    fcntl(fd_p,F_SETLKW,&lock); 
printf("FILE LOCKED\n");
    
    p->prodID = counter;
    int id = p->prodID;
    lseek(fd_p,SIZE_PRODUCT * id,SEEK_SET);
    write(fd_p,p,SIZE_PRODUCT);
    

    lock.l_type = F_UNLCK;
    fcntl(fd_p,F_SETLK,&lock);
    //unlock
printf("FILE UNLOCKED\n");
    close(fd_p);
    
    return p->prodID;
}

int addStock(int id,int change){
    int fd_p = open(PRODUCT_FILE,O_RDWR);
    product * p = malloc(SIZE_PRODUCT);
    //lock
    struct flock lock;
    lock.l_pid = getpid();
    lock.l_type =F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start =0;
    lock.l_len = SIZE_PRODUCT * MAX_NO_OF_PRODUCTS;
    fcntl(fd_p,F_SETLKW,&lock);
printf("FILE LOCKED\n");

    lseek(fd_p,SIZE_PRODUCT * id, SEEK_SET);
    read(fd_p,p,SIZE_PRODUCT);

    p->prodQuantity += change;

    lseek(fd_p,SIZE_PRODUCT * id, SEEK_SET);
    write(fd_p,p,SIZE_PRODUCT);


    //unlock
    lock.l_type = F_UNLCK;
    fcntl(fd_p,F_SETLK,&lock);
printf("FILE UNLOCKED\n");
    close(fd_p);
    return p->prodQuantity;
}

int changePrice(int id,int price){
    int fd_p = open(PRODUCT_FILE,O_RDWR);
    product * p = malloc(SIZE_PRODUCT);

    //locke
    struct flock lock;
    lock.l_pid = getpid();
    lock.l_type =F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start =0;
    lock.l_len = SIZE_PRODUCT * MAX_NO_OF_PRODUCTS;
    printf("BEFORE CRITICAL");
    fcntl(fd_p,F_SETLKW,&lock);
    printf("INSIDE CRIRICAL\n");

    lseek(fd_p,SIZE_PRODUCT * id, SEEK_SET);
    read(fd_p,p,SIZE_PRODUCT);

    p->prodPrice = price;

    lseek(fd_p,SIZE_PRODUCT * id, SEEK_SET);
    write(fd_p,p,SIZE_PRODUCT);

    lock.l_type = F_UNLCK;
    fcntl(fd_p,F_SETLK,&lock);
    printf("OUTSIDE CRITICAL\n");
    //unlock
    close(fd_p);
    return 0;
}

int main(){
    //setting counter
    int prodID_counter;
    
    int store_fd =open(STORE_FILE,O_RDWR | O_CREAT , 0744);
    read(store_fd,&prodID_counter,sizeof(int));
    close(store_fd);
    printf("Caught up\n");
    printf("%d\n",prodID_counter);
    int sd = socket(AF_INET,SOCK_STREAM,0);
    
    struct sockaddr_in server_address,client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int sz = sizeof(client_address);

    bind(sd,(struct sockaddr *)&server_address,sizeof(server_address));

    listen(sd,5);
while (1)
{
    printf("\nlistening...\n");
    int cd = accept(sd,(struct sockaddr*)&client_address,&sz);
    printf("Connected! on socket descriptor: %d\n",cd);
    if(!fork() ){
        printf("handling process's pid: %d\n",getpid());
        int key = 101;
        write(cd ,&key, sizeof(key));
        printf("Confirmation Key sent\n");
        read(cd,&key,sizeof(key));
        printf("client request is %d\n", key);
        if(key == 1){
            product apr[MAX_NO_OF_PRODUCTS];
            printf("Reading all products\n");
            readAllProducts(prodID_counter,apr);

            write(cd,apr,sizeof(apr));
        }
        else if(key == 3){
            int arr[2];
            read(cd,arr,sizeof(arr));
            product * p = addToCart(&prodID_counter,arr);
            write(cd,p,SIZE_PRODUCT); 
        }
        else if(key == 4){
            int arr[2];
            read(cd, arr, sizeof(arr));
            
            int result = updateCart(&prodID_counter,arr);
            
            write(cd,&result,sizeof(result));
        }
        else if(key == 0){
            int cart_size;
            read(cd,&cart_size,sizeof(cart_size));
            product Cart[MAX_NO_OF_PRODUCTS];
            read (cd,Cart,SIZE_PRODUCT * MAX_NO_OF_PRODUCTS);
            clientExit(cart_size,Cart);
            cart_size = 0;
            write(cd,&cart_size,sizeof(cart_size));
        }
        else if(key == 5){
            int cart_size;
            read(cd,&cart_size,sizeof(cart_size));
            product Cart[MAX_NO_OF_PRODUCTS];
            read (cd,Cart,sizeof(Cart));
            int changeInPrice[cart_size];

            // for(int i=0;i<cart_size;i++){
            //     printf("%d %s %d %d\n",Cart[i].prodID,Cart[i].prodName,Cart[i].prodPrice,Cart[i].prodQuantity);
            // }

            prePaymentCheck(cart_size,Cart,changeInPrice);

            for(int i=0;i<cart_size;i++){
                printf("%d, ",changeInPrice[i]);
            }
            printf(";\n");

            write(cd,changeInPrice,sizeof(changeInPrice));
        }
        else if(key == 11){
            product * p = malloc(SIZE_PRODUCT);
            read(cd,p,SIZE_PRODUCT);
            printProductDetails(*p,STOCK);
            printf("%d\n",prodID_counter);
            int new_id = addProduct(p,prodID_counter);
            prodID_counter ++;

            int fd_s = open(STORE_FILE,O_RDWR);
            lseek(fd_s,0,SEEK_SET);
            write(fd_s,&prodID_counter,sizeof(int)); 
            printf("%d\n",prodID_counter);

            write(cd,&new_id,sizeof(int));
        }
        else if(key == 12){
            int arr[2];
            read(cd,arr,sizeof(arr));
            int new_quantity = addStock(arr[0],arr[1]);
            printf("%d",new_quantity);
            write(cd,&new_quantity,sizeof(int));
        }
        else if(key == 13){
            int arr[2];
            
            read(cd,arr,sizeof(arr));
            printf("%d %d\n",arr[0],arr[1]);
            int status = changePrice(arr[0],arr[1]);
            write(cd,&status,sizeof(int));
            printf("%d\n",status);
        }
        close(cd);
    }
    else{
        close(cd);
    }
    
}

    

    return 0;

}