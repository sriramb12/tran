#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/blowfish.h>

#include "global_debug.h" // Mazieri's debug facilities

/* 
  Routine core_id_crypt will take a string INPUT_LEN (#define)  characters or less and return 
  an encrypted hex string of INPUT_LEN*2+14 characters.   The encrypted string will include
  a salt and will not return a consistant string with a constant input.
  
  The second parameter is either "ENCRYPT" or "DECRYPT" 

  The routine returns an integer reporting how "old" the encrypted string is, and it
  always decrypts the string.

  The code uses the "Blowfish" alogorithm.

  Paul Lupa  2010
*/

#define  INPUT_LEN      80 
#define  ENCRYPT        1
#define  DECRYPT        2
#define  ENCRYPT_LEN    INPUT_LEN*2+40    /* Hexify input, Null, Salt, Time, Null */

int core_id_crypt(char * core_id, int mode)
{
    char  key[8],ch;
    int   i,j,k, len, tmp;
    unsigned long long    seed_time , pass_time;
    unsigned char str_in[INPUT_LEN*2 + 40], str_out[INPUT_LEN*2 + 40];
    BF_KEY  bf_key;
    
    DBG("core_id='%s' and mode='%d'", core_id, mode);
    
    len = strlen( core_id );
    
    DBG("Sizeof seed_time: %d", sizeof(seed_time));
    DBG("Len: %d", len);
    
    key[0] = 'k';
    key[1] = 'e';
    key[2] = 'y';
    key[3] = 'k';
    key[4] = 'R';
    key[5] = 'E';
    key[6] = 'd';
    key[7] = 'e';
    
    /* Initialize the blowfish key */
    BF_set_key(&bf_key, 8, key);
    
    seed_time = time(NULL);
     
    /* Initialize the random number generator */ 
    srand((unsigned)time(NULL));
    
    /*Check for mode for encrypt*/
    if ( mode & ENCRYPT ) {
        DBG("Encrypt");

        if (len > INPUT_LEN) {
            core_id[0] = 0 ;
            return(-2);
        }
    
        for(j = 0; j< ENCRYPT_LEN ; j++) {
            /* Fill with random Values */
            str_in[j] = (char)(rand() % 240);
        }
         
        /* Seed + 11char time + String + null */
        sprintf(str_in,"%c%010llu %s",(char)(seed_time%120),seed_time,core_id ); 
        DBG("Data to be encrypted: %s", str_in); 
    
        for ( j=0; j<((len+21)/8) ; j++ ) {
            BF_ecb_encrypt(str_in+(j*8), str_out+(j*8), &bf_key , BF_ENCRYPT);
        }  
     
        for (i = 0 ; i<((len+21)/8) ; i++ ) {
            DBG("Hexify Block: %d of %d", i, ((len+21)/8));
            sprintf(core_id+(i*16), "%02X%02X%02X%02X%02X%02X%02X%02X", str_out[(i*8)+0], str_out[(i*8)+1], str_out[(i*8)+2], str_out[(i*8)+3], str_out[(i*8)+4], str_out[(i*8)+5], str_out[(i*8)+6], str_out[(i*8)+7]);
        }
      
        DBG("Return Len: %d", strlen(core_id));
        DBG("Return String: '%s'", core_id);
        return(0);
    } else if (mode & DECRYPT)  {
        DBG("Decrypt");
    
        /* DeHEXify characters 4 */
        for (j = 0 ; j<(len/2) ; j++ ) {
            sscanf(core_id+(j*2), "%2X" , &tmp);
            str_in[j] = tmp;
        }
    
        for ( j=0; j<((len/2)/8) ; j++ ) {
            DBG("Decrypted %d of %d", j, ((len/2)/8));

            BF_ecb_encrypt( str_in+(j*8), str_out+(j*8), &bf_key , BF_DECRYPT );
        }  
        DBG("Decrypted (all): %s", str_out);

        /* Skip the first character ( salt ) */
        sscanf(str_out, "%c%llu", &ch, &pass_time );
        j = seed_time - pass_time;

        DBG("TimeDiff: %d", j);
        DBG("Decrypted Data: '%s'", str_out+12);

        strcpy(core_id,  str_out+12) ;
        return(j+1);
    }
} /* end core_id_crypt */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
