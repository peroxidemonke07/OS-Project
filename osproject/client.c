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

void printProdArr(product prodArr[])
{
    for (int i = 0; i < MAX_NO_OF_PRODUCTS; i++)
    {
        if (prodArr[i].prodID == -1)
            break;
        printProductDetails(prodArr[i], STOCK);
    }
}

int displayCart(product *c)
{
    int cartPrice = 0;
    printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    printf("                        <= DISPLAYING CART =>                          \n");
    for (int i = 0; i < MAX_NO_OF_PRODUCTS; i++)
    {
        if (c[i].prodID == -1)
            break;
        printProductDetails(c[i], CART);
        cartPrice += (c[i].prodPrice * c[i].prodQuantity);
    }
    printf("\nTotal cost of items in cart:%d\n", cartPrice);
    printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    return cartPrice;
}

int getAllProducts()
{
    product apr[MAX_NO_OF_PRODUCTS];

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
    if (key == 101)
    {
        printf("Connected to server!\n");
        int cReq = 1;
        write(cd, &cReq, sizeof(int));
        printf("req sent please wait\n");

        int BytesRead = read(cd, apr, sizeof(apr));

        printProdArr(apr);
    }

    close(cd);
}

product *addToCart(int id, int q)
{
    product *p = malloc(SIZE_PRODUCT);
    p->prodID = -2;
    printf("Connecting to server...\n");
    int cd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int status = connect(cd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (status == -1)
    {
        printf("error connecting to server\n");
        return p;
    }

    int key = 0;
    read(cd, &key, sizeof(key));
    if (key = 101)
    {
        printf("Connected to server!\n");
        int cReq = 3;
        write(cd, &cReq, sizeof(int));
        printf("req sent please wait\n");
        int arr[2];
        arr[0] = id;
        arr[1] = q;
        write(cd, arr, sizeof(arr));

        read(cd, p, SIZE_PRODUCT);
    }
    close(cd);
    return (p);
}

int updateCart(int id, int change)
{
    int result = -1;
    printf("Connecting to server...\n");
    int cd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int status = connect(cd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (status == -1)
    {
        printf("error connecting to server\n");
        return -1;
    }

    int key;
    read(cd, &key, sizeof(key));

    if (key == 101)
    {
        printf("Connected to server!\n");
        int cReq = 4;
        write(cd, &cReq, sizeof(int));
        printf("req sent please wait\n");
        int arr[2];
        arr[0] = id;
        arr[1] = change;

        write(cd, arr, sizeof(arr));

        read(cd, &result, sizeof(result));
    }
    close(cd);
    return result;
}

void exitProtocol(product *Cart, int counter)
{
    printf("Connecting to server...\n");
    int cd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int status = connect(cd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (status == -1)
    {
        printf("error connecting to server\n");
        return;
    }

    int key;
    read(cd, &key, sizeof(key));

    if (key == 101)
    {
        printf("Connected to server!\n");
        int cReq = 0;
        write(cd, &cReq, sizeof(int));
        printf("req sent please wait\n");
        write(cd, &counter, sizeof(counter));
        write(cd, Cart, SIZE_PRODUCT * MAX_NO_OF_PRODUCTS);
        int result;
        read(cd, &result, sizeof(result));
        if (result == 0)
            printf("cart emptied succesfully\n");
    }
    close(cd);
}

void prePaymentProcessing(int cart_size, product *Cart, int *changeInPrice)
{
    printf(">>>Checking for update in product prices\n");
    printf(">>Connecting to server...\n");
    int cd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int status = connect(cd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (status == -1)
    {
        printf(">>error connecting to server\n");
        return;
    }

    int key;
    read(cd, &key, sizeof(key));

    if (key = 101)
    {
        printf(">>Connected to server!\n");
        int cReq = 5;
        write(cd, &cReq, sizeof(int));
        printf(">>req sent please wait\n");
        write(cd, &cart_size, sizeof(cart_size));
        write(cd, Cart, SIZE_PRODUCT * MAX_NO_OF_PRODUCTS);

        read(cd, changeInPrice, sizeof(int) * cart_size);
    }
    close(cd);
}

int main()
{
    product Cart[MAX_NO_OF_PRODUCTS];
    for (int i = 0; i < MAX_NO_OF_PRODUCTS; i++)
    {
        Cart[i].prodID = -1;
        strcpy(Cart[i].prodName, "");
        Cart[i].prodPrice = -1;
        Cart[i].prodQuantity = -1;
    }
    int cart_counter = 0;

    int choice;
    while (1)
    {
        // menu:
        printf("\n############################  MENU  ############################\n");
        printf("=> Items In your cart are locked, no other user can add them to their cart.\n");
        printf("=> Items are lost if you exit\n");
        printf("=> Cost of items in your cart may get upadted,\n \tupdated cost will be shown when you select to pay.\n");
        printf("Press 0 to EXIT\n");
        printf("Press 1 to DISPLAY ALL PRODUCTS\n");
        printf("Press 2 to DISPLAY CART\n");
        printf("Press 3 to ADD A PRODUCT TO CART\n");
        printf("Press 4 to UPDATE THE QUANTITY OF AN ITEM ALREADY IN CART\n");
        printf("Press 5 to PAY \n");
        printf("\n################################################################\n");
        scanf("%d", &choice);
        /////////////////////////////////////////////////////////////////////////////////////////
        if (choice == 0)
        {
            int exit;
            printf("\n############################=>  EXIT <=############################\n");
            printf("=> Items are lost if you exit\n");
            printf("Press 0 to exit\nPress 1 to go back to menu\n");
            scanf("%d", &exit);
            if (exit == 0)
            {
                if (cart_counter > 0)
                {
                    exitProtocol(Cart, cart_counter);
                }
                break;
            }
            printf("\n################################################################\n");
        }
        /////////////////////////////////////////////////////////////////////////////////////////
        else if (choice == 1)
        {
            printf("\n########################  ALL PRODUCTS  ########################\n");
            getAllProducts();
            printf("\n################################################################\n");
        }
        /////////////////////////////////////////////////////////////////////////////////////////
        else if (choice == 2)
        {
            
            displayCart(Cart);
        }
        /////////////////////////////////////////////////////////////////////////////////////////
        else if (choice == 3)
        {
            printf("#################ADD TO CART#################\n");
            int id = -2, quantity, alreadyInCart = 0;
            printf("Enter product ID: ");
            scanf("%d", &id);
            printf("Enterd product ID: %d\n", id);
            for (int i = 0; i < cart_counter; i++)
            {
                if (Cart[i].prodID == id)
                {
                    alreadyInCart = 1;
                    break;
                }
            }
            printf("out\n");
            if (id < 0)
            {
                printf("ILLEGAL ID\n");
            }
            else if (alreadyInCart)
            {
                printf("ITEM ALREADY INCART\nIF YOU MEANT TO UPDATE THE QUANTITY OF AN ITEM ALREADY IN CART PRESS 4 IN MENU\n");
            }
            else
            {
                printf("Enter product Quantity: ");
                scanf("%d", &quantity);
                product *p = addToCart(id, quantity);
                if (p->prodID > -2)
                {
                    if (p->prodID >= 0)
                    {
                        Cart[cart_counter] = *p;
                        cart_counter++;
                        printf("SUCCESSFULLY ADDED TO CART");
                        displayCart(Cart);
                    }
                    else
                    {
                        printf("FAILED TO ADD TO CART: %s", p->prodName);
                    }
                }
            }
            printf("#############################################\n");
        }
        /////////////////////////////////////////////////////////////////////////////////////////
        else if (choice == 4)
        {
            printf("##################### UPDATE CART ########################\n");
            displayCart(Cart);
            int id, q, notInCart = 1, ndx;
            printf("Enter product ID: ");
            scanf("%d", &id);
            for (int i = 0; i < cart_counter; i++)
            {
                if (id == Cart[i].prodID)
                {
                    notInCart = 0;
                    ndx = i;
                    break;
                }
            }
            if (notInCart)
            {
                printf("ITEM NOT IN CART\nTO ADD ITEM TO CART PRESS 3 IN MENU\n");
            }
            else if (id < 0)
            {
                printf("ILLEGAL ID");
            }
            else if (id >= 0)
            {
                printf("Enter new quantity for:\nProduct name:%s,  Product ID: %d,  Price:%d\n", Cart[ndx].prodName, Cart[ndx].prodID, Cart[ndx].prodPrice);
                scanf("%d", &q);
                int change = q - Cart[ndx].prodQuantity;
                printf("PRESS 1 TO CONFIRM CHANGE\nPRESS 0 GO BACK TO MENU\n");
                scanf("%d", &q);
                if (q > 0)
                {
                    int result = updateCart(id, change);
                    if (result == 0)
                    {
                        Cart[ndx].prodQuantity += change;
                        printf("SUCCESFULLY UPDATED CART\n");
                        displayCart(Cart);
                    }
                    else if (result == 1)
                    {
                        printf("UPDATE FAILED : INVALID ID\n");
                    }
                    else if (result == 2)
                    {
                        printf("UPDATE FAILED : INSUFFICIENT STOCK\n");
                    }
                }
            }
            printf("##########################################################\n");
        }

        else if (choice == 5)
        {
            printf("\n ########################### PAYMENT ###########################\n");
            printf("Your Cart:\n");
            displayCart(Cart);
            if (cart_counter == 0)
            {
                printf(">>Cart empty\n>>Going back to menu");
            }
            else
            {
                // changeInPrice => Arr , cip[i] == -1 if no change | cip[i] = NEW_PRICE{>0} if change in price of Cart[i] .
                int changeInPrice[cart_counter];
                prePaymentProcessing(cart_counter, Cart, changeInPrice);
                int cip_no = 0;
                for (int i = 0; i < cart_counter; i++)
                {
                    if (changeInPrice[i] > 0)
                    {
                        cip_no++;
                    }
                }
                printf("There has been change in price of %d Products\n", cip_no);
                if (cip_no > 0)
                {
                    for (int i = 0; i < cart_counter; i++)
                    {
                        if (changeInPrice[i] > 0)
                        {
                            printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
                            printf(">>>Change in price of Product in cart:\n");
                            printf(">>BEFORE => ");
                            printProductDetails(Cart[i], CART);
                            Cart[i].prodPrice = changeInPrice[i];
                            printf(">>NOW => ");
                            printProductDetails(Cart[i], CART);
                            printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
                        }
                    }
                }
                printf("YOUR CART =>\n");
                displayCart(Cart);
                int pay;
                printf("Press 1 to PAY FOR ITEMS IN YOUR CART\nPress 0 to go back to MENU\n");
                scanf("%d", &pay);
                if (pay == 1)
                {
                    int cartprice = displayCart(Cart);
                    while (1)
                    {
                        int payment;
                        printf("Your Bill amount is : %d\n", cartprice);
                        printf("Enter the amount to pay: ");
                        scanf("%d", &payment);
                        if (payment != cartprice)
                            printf("INCORRECT AMOUNT (MONEY NOT DEPOSITED) , TRY AGAIN\n");
                        else
                        {
                            printf("PAYMENT SUCCESSFUL :)\n");
                                for (int i = 0; i < MAX_NO_OF_PRODUCTS; i++){
                                    Cart[i].prodID = -1;
                                    strcpy(Cart[i].prodName, "");
                                    Cart[i].prodPrice = -1;
                                    Cart[i].prodQuantity = -1;
                                }
                            break;
                        }
                    }
                }
            }

            printf("\n################################################################\n");
        }
    }
}