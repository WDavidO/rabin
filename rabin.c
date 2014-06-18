#include<stdio.h>
#include<openssl/bn.h>

        BN_CTX *ctx;

void extended_GCD(BIGNUM *x, BIGNUM *y,BIGNUM *gcd,BIGNUM *a,BIGNUM *b)//ax+by=gcd
{

	BIGNUM *x1,*y1,*div,*rem, *temp, *a1,*b1;
	x1=BN_new();
        y1=BN_new();
        div=BN_new();
       	rem=BN_new();
	temp=BN_new();
	a1=BN_new();
	b1=BN_new();

	BN_copy(a1,a);BN_copy(b1,b);
	BN_set_word(x1,0);BN_set_word(x,1);
        BN_set_word(y1,1);BN_set_word(y,0);
	
	while(!BN_is_zero(b1))
	{
		BN_copy(temp,b1);
	        BN_div(div,rem,a1,b1,ctx);
		BN_copy(b1,rem);
		BN_copy(a1,temp);

		BN_copy(temp,x1);
		BN_mul(x1,x1,div,ctx);
		BN_sub(x1,x,x1);
		BN_copy(x,temp);

                BN_copy(temp,y1);
                BN_mul(y1,y1,div,ctx);
                BN_sub(y1,y,y1);
                BN_copy(y,temp);
	}
	BN_copy(gcd,a1);
	BN_free(x1); BN_free(y1); BN_free(a1); BN_free(b1); BN_free(temp); BN_free(div); BN_free(rem);
}

//This file generates two primes and encrypts a given message.
int main()
{
	BIGNUM *p, *q, *n, *B,*x,*y, *add,*rem;


        ctx=BN_CTX_new();

	p=BN_new();
	q=BN_new();
	n=BN_new();
	B=BN_new();
	x=BN_new();
	y=BN_new();

	add=BN_new();
	rem=BN_new();
	BN_set_word(add,4);
	BN_set_word(rem,3);
	BN_generate_prime(p,80,1,add,rem,NULL,NULL);
	printf("prime p=3 mod 4=  ");
	BN_print_fp(stdout,p);printf("\n");
        BN_generate_prime(q,80,1,add,rem,NULL,NULL);
        printf("prime q=3 mod 4=  ");
        BN_print_fp(stdout,q);printf("\n");
	
	BN_mul(n,p,q,ctx);
	printf("n=pq= ");
        BN_print_fp(stdout,n);printf("\n");

// READ THE VALUE OF B AND X FROM THE USER.	

	char B_srt[100];
	char x_srt[100];
	printf("Enter the value of B:");
	scanf("%s",B_srt);

	printf("Enter the value of x, the message to be Encrypted:");
	scanf("%s",x_srt);

	BN_dec2bn(&x,x_srt);
	BN_dec2bn(&B,B_srt);

	printf("**********************************************************\n");
	printf("x= ");BN_print_fp(stdout,x);printf("\n");
        printf("B= ");BN_print_fp(stdout,B);printf("\n");
	BN_mod_add(y,x,B,n,ctx);
	BN_mod_mul(y,x,y,n,ctx);	
        printf("Encrypted message y=x(x+B) mod n = ");BN_print_fp(stdout,y);printf("\n");
	printf("***********************************************************\n");

//DECRYPT THE MESSAGE
	//y=x(x+B)mod n, put x=x1-B/2 , then x1^2 = y1 = y + (B^2)/4
	//so we solve for 4x= B^2 mod n to evaluate B^2/4.	
	// the solution is B^2/d * x , where d=gcd(4,n) and x is the x of extended euclid.

	BIGNUM *y1,*x1, *ex, *ey , *gcd,*b,*B2b4,*rem1;

	y1=BN_new();
	x1=BN_new();
	ex=BN_new();
	ey=BN_new();
	gcd=BN_new();
	b=BN_new();
	B2b4=BN_new();
	rem1=BN_new();	

	BN_set_word(b,4);
	extended_GCD(ex,ey,gcd,n,b);
        
        BN_mul(B2b4,ey,B,ctx);
        BN_mul(B2b4,B2b4,B,ctx);
        BN_div(B2b4,rem1 ,B2b4,gcd,ctx);
	
	BN_mod_add(y1,y,B2b4,n,ctx);	


	//printf("y1= ");BN_print_fp(stdout,y1);printf("\n");

	BIGNUM *r,*r1,*s,*s1,*mp,*mq,*yp,*yq,*p1,*q1,*cons ;
	r=BN_new();
	r1=BN_new();
	s=BN_new();
	s1=BN_new();
	mp=BN_new();
	mq=BN_new();
	yp=BN_new();
	yq=BN_new();
	p1=BN_new();
	q1=BN_new();
	cons=BN_new();

	BN_set_word(cons,1);
	BN_add(p1,p,cons);	
	BN_add(q1,q,cons);
	BN_set_word(cons,4);
	BN_div(p1,rem1,p1,cons,ctx);
	BN_div(q1,rem1,q1,cons,ctx);

	BN_mod_exp(mp,y1,p1,p,ctx);
        BN_mod_exp(mq,y1,q1,q,ctx);


        //printf("mp= ");BN_print_fp(stdout,mp);printf("\n");
        //printf("mq= ");BN_print_fp(stdout,mq);printf("\n");

	extended_GCD(yp,yq,gcd,p,q);

        //printf("yp= ");BN_print_fp(stdout,yp);printf("\n");
        //printf("yq= ");BN_print_fp(stdout,yq);printf("\n");
	
	BIGNUM *t1,*t2;
	t1=BN_new();
	t2=BN_new();
	
	BN_mod_mul(t1,p,yp,n,ctx);
        BN_mod_mul(t1,t1,mq,n,ctx);

        BN_mod_mul(t2,q,yq,n,ctx);
        BN_mod_mul(t2,t2,mp,n,ctx);

	BN_mod_add(r,t1,t2,n,ctx);
	BN_mod_sub(r1,n,r,n,ctx);

	BN_mod_sub(s,t1,t2,n,ctx);
	BN_mod_sub(s1,n,s,n,ctx);

//TODO x=x1-B/2
	BIGNUM *Bb2;
	Bb2=BN_new();
        BN_set_word(b,2);
        extended_GCD(ex,ey,gcd,n,b);

        BN_mul(Bb2,ey,B,ctx);
        BN_div(Bb2,rem1 ,Bb2,gcd,ctx);

        BN_mod_sub(r,r,Bb2,n,ctx);
        BN_mod_sub(r1,r1,Bb2,n,ctx);
        BN_mod_sub(s,s,Bb2,n,ctx);
        BN_mod_sub(s1,s1,Bb2,n,ctx);





	printf("The original message is one of the following:\n");
	printf("r= "); BN_print_fp(stdout,r) ;printf("\n");
        printf("r1= "); BN_print_fp(stdout,r1) ;printf("\n");
        printf("s= "); BN_print_fp(stdout,s) ;printf("\n");
        printf("s1= "); BN_print_fp(stdout,s1) ;printf("\n");

	
	//BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();
//BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();BN_free();

	
	return 0;
}


