#ifndef TRANS_FTP
#ifdef SSL_ENCRYPTION

/****************************************************************************
 *
 * Function:	core_id_crypt
 *
 * Description:	Encrypts a string and returns an encrypted hex string
 *
 * Author:	Paul Lupa
 *
 ***************************************************************************/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/blowfish.h>
#include "trans_prototypes.h"

#include "global_debug.h"

/* 
  Routine core_id_crypt will take a string INPUT_LEN (#define)  characters or less and return 
  an encrypted hex string of INPUT_LEN*2+14 characters.   The encrypted string will include
  a salt and will not return a consistant string with a constant input.
  
  The second parameter is either "ENCRYPT" or "DECRYPT" 

  The routine returns an integer reporting how "old" the encrypted string is, and it
  always decrypts the string.

  The code uses the "Blowfish" alogorithm.

  Paul Lupa  2010
  NXP N.V.

*/

#define  INPUT_LEN 40 

#define  ENCRYPT  1
#define  DECRYPT  2
/* #define  DEBUG 0 */
#define  ENCRYPT_LEN  INPUT_LEN*2+40
                    /* Hexify input, Null, Salt, Time, Null */

int core_id_crypt( char * core_id,  int mode )
{

    DBG();
    
    unsigned char  key[8];
    char  ch;
    int   i, j, len, tmp;

    unsigned long long  seed_time , pass_time;
    unsigned char str_in[INPUT_LEN*2 + 40], str_out[INPUT_LEN*2 + 40];
  
    BF_KEY bf_key;
 
    len = strlen( core_id );

#ifdef DEBUG 
    printf( "Sizeof seed_time: %lu\n", sizeof(seed_time));
    printf( "Len: %d\n", len);
#endif

    key[0] = 'k';
    key[1] = 'e';
    key[2] = 'y';
    key[3] = 'k';
    key[4] = 'R';
    key[5] = 'E';
    key[6] = 'd';
    key[7] = 'e';
 
    /* Initialize the blowfish key */
    BF_set_key( &bf_key, 8, key);

    seed_time = time(NULL);
 
    /* Initialize the random number generator */ 
    srand((unsigned)time(NULL));

    /*Check for mode for encrypt*/
    if ( mode & ENCRYPT ) {
#ifdef DEBUG 
        printf( "Encrypt\n");
#endif
        if( len > INPUT_LEN ) {
            core_id[0] = 0 ;
            return(-2);
        }
      
        for( j=0; j< ENCRYPT_LEN ; j++ ) {        /* Fill with random Values */
            str_in[j] = (char)(rand() % 240 )  ;
    }
     
      /*  Seed + 11char time + String + null  */
      sprintf(str_in, "%c %010llu %s", (char)(seed_time%120), seed_time, core_id ); 
#ifdef DEBUG 
    printf( "Data to be encrypted: %s\n",str_in ); 
    printf( "Data:");
#endif

      for ( j=0; j<((len+21)/8) ; j++ ) {
        BF_ecb_encrypt( str_in+(j*8), str_out+(j*8), &bf_key , BF_ENCRYPT );
#ifdef DEBUG 
            int k;
           	printf( "Encrypt Block: %d  \n In: ", j);
            for ( k=0; k< 8 ; k++ ) {
               	printf( " %3d", (int)(str_in[j*8+k]) );
            } 
           	printf( ":  \nOut: ");
            for ( k=0; k< 8 ; k++ ) {
              	printf( " %3d", (int)(str_out[j*8+k]) );
            }
           	printf( ":\n");
#endif
      }  
 
      for (i = 0 ; i<((len+21)/8) ; i++ ) {
#ifdef DEBUG 
    printf( "Hexify Block: %d of %d\n", i, ((len+21)/8) );
#endif
         sprintf( core_id+(i*16), "%02X%02X%02X%02X%02X%02X%02X%02X" \
             , str_out[(i*8)+0] , str_out[(i*8)+1], str_out[(i*8)+2], str_out[(i*8)+3] \
             , str_out[(i*8)+4] , str_out[(i*8)+5], str_out[(i*8)+6], str_out[(i*8)+7] \
                );
      }
  
#ifdef DEBUG 
      printf( "Return Len: %d\n", (int) strlen( core_id ) );
      printf( "Return String: :%s:\n", core_id);
#endif
      return(0);
   }
   else if ( mode & DECRYPT  )  {
#ifdef DEBUG 
      printf( "Decrypt\n");
#endif

 /* DeHEXify  characters 4 */
       for (j = 0 ; j<(len/2) ; j++ ) {
         sscanf( core_id+(j*2), "%2X" , &tmp );
         str_in[j] = tmp;
         }


     for ( j=0; j<((len/2)/8) ; j++ ) {
#ifdef DEBUG 
        printf( "Decrypted %d of %d\n", j, ((len/2)/8));
#endif
        BF_ecb_encrypt( str_in+(j*8), str_out+(j*8), &bf_key , BF_DECRYPT );
#ifdef DEBUG 
            int k;
           	printf( "Encrypt Block: %d  \n In: ", j);
            for ( k=0; k< 8 ; k++ ) {
               	printf( " %3d", (int)(str_in[j*8+k]) );
            } 
           	printf( ":  \nOut: ");
            for ( k=0; k< 8 ; k++ ) {
              	printf( " %3d", (int)(str_out[j*8+k]) );
            }
           	printf( ":\n");
#endif
     }  
#ifdef DEBUG 
            int k;
            printf( "Data:");
            for ( k=0; k<((len/2)) ; k++ ) {
           	    printf( "%2d", (int)str_in[k] );
            } 
        	    printf( ":\n");
            printf( "Decrypted (all): %s\n", str_out);
#endif
    /* Skip the first character ( salt ) */
     sscanf(str_out, "%c%llu", &ch, &pass_time );
     j = seed_time - pass_time;

#ifdef DEBUG 
     printf( "TimeDiff: %d\n", j);
     printf( "Decrypted Data: %s\n", str_out+12);
#endif

     strcpy(core_id,  str_out+12) ;

     return(j+1);
  }

   return 0;
} /* end core_id_crypt */

#else
int core_id_crypt( char * core_id,  int mode )
{ }
#endif
#endif
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
