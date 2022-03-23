#ifndef TRANSAPI
#ifndef trans_client_msg_h
#define trans_client_msg_h

#ifdef TRANSCEND
#ifdef NXP 
#include "transcend_client_msg.h"
#endif
#ifdef ONSEMI
#include "onsemi_client_msg.h"
#endif
#endif
#ifdef M2
#include "transcend_client_msg.h"
#endif

#ifdef INTERCOM
#ifdef TRANSNET
#include "transnet_client_msg.h"
#endif
#ifdef PHOTRONICS
#include "intercom_phot_msg.h"
#endif
#ifdef NXP
#include "intercom_free_msg.h"
#endif
#ifdef LOCAL
#endif
#endif


#endif /* Define for trans_client_msg_h */
#endif /* for TRANSAPI */
