/************************************************************

Created: Sunday, January 6, 1991 at 8:53 PM
    ADSP.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1986-1990
        All rights reserved

************************************************************/


#ifndef __ADSP__
#define __ADSP__

#ifndef __APPLETALK__
#include <AppleTalk.h>
#endif





/* driver control ioResults */

#define errRefNum -1280                         /* bad connection refNum */
#define errAborted -1279                        /* control call was aborted */
#define errState -1278                          /* bad connection state for this operation */
#define errOpening -1277                        /* open connection request failed */
#define errAttention -1276                      /* attention message too long */
#define errFwdReset -1275                       /* read terminated by forward reset */
#define errDSPQueueSize -1274                   /* DSP Read/Write Queue Too small */
#define errOpenDenied -1273                     /* open connection request was denied */

/*driver control csCodes*/
#define dspInit 255                             /* create a new connection end */
#define dspRemove 254                           /* remove a connection end */
#define dspOpen 253                             /* open a connection */
#define dspClose 252                            /* close a connection */
#define dspCLInit 251                           /* create a connection listener */
#define dspCLRemove 250                         /* remove a connection listener */
#define dspCLListen 249                         /* post a listener request */
#define dspCLDeny 248                           /* deny an open connection request */
#define dspStatus 247                           /* get status of connection end */
#define dspRead 246                             /* read data from the connection */
#define dspWrite 245                            /* write data on the connection */
#define dspAttention 244                        /* send an attention message */
#define dspOptions 243                          /* set connection end options */
#define dspReset 242                            /* forward reset the connection */
#define dspNewCID 241                           /* generate a cid for a connection end */

/* connection opening modes */
#define ocRequest 1                             /* request a connection with remote */
#define ocPassive 2                             /* wait for a connection request from remote */
#define ocAccept 3                              /* accept request as delivered by listener */
#define ocEstablish 4                           /* consider connection to be open */

/* connection end states */
#define sListening 1                            /* for connection listeners */
#define sPassive 2                              /* waiting for a connection request from remote */
#define sOpening 3                              /* requesting a connection with remote */
#define sOpen 4                                 /* connection is open */
#define sClosing 5                              /* connection is being torn down */
#define sClosed 6                               /* connection end state is closed */

/* client event flags */
#define eClosed 0x80                            /* received connection closed advice */
#define eTearDown 0x40                          /* connection closed due to broken connection */
#define eAttention 0x20                         /* received attention message */
#define eFwdReset 0x10                          /* received forward reset advice */

/* miscellaneous constants */
#define attnBufSize 570                         /* size of client attention buffer */
#define minDSPQueueSize 100                     /* Minimum size of receive or send Queue */

/* connection control block */
struct TRCCB {
    unsigned char *ccbLink;                     /* link to next ccb */
    unsigned short refNum;                      /* user reference number */
    unsigned short state;                       /* state of the connection end */
    unsigned char userFlags;                    /* flags for unsolicited connection events */
    unsigned char localSocket;                  /* socket number of this connection end */
    AddrBlock remoteAddress;                    /* internet address of remote end */
    unsigned short attnCode;                    /* attention code received */
    unsigned short attnSize;                    /* size of received attention data */
    unsigned char *attnPtr;                     /* ptr to received attention data */
    unsigned char reserved[220];                /* for adsp internal use */
};

typedef struct TRCCB TRCCB;
typedef TRCCB *TPCCB;

/* init connection end parameters */
struct TRinitParams {
    TPCCB ccbPtr;                               /* pointer to connection control block */
    ProcPtr userRoutine;                        /* client routine to call on event */
    unsigned short sendQSize;                   /* size of send queue (0..64K bytes) */
    unsigned char *sendQueue;                   /* client passed send queue buffer */
    unsigned short recvQSize;                   /* size of receive queue (0..64K bytes) */
    unsigned char *recvQueue;                   /* client passed receive queue buffer */
    unsigned char *attnPtr;                     /* client passed receive attention buffer */
    unsigned char localSocket;                  /* local socket number */
};

typedef struct TRinitParams TRinitParams;

/* open connection parameters */
struct TRopenParams {
    unsigned short localCID;                    /* local connection id */
    unsigned short remoteCID;                   /* remote connection id */
    AddrBlock remoteAddress;                    /* address of remote end */
    AddrBlock filterAddress;                    /* address filter */
    unsigned long sendSeq;                      /* local send sequence number */
    unsigned short sendWindow;                  /* send window size */
    unsigned long recvSeq;                      /* receive sequence number */
    unsigned long attnSendSeq;                  /* attention send sequence number */
    unsigned long attnRecvSeq;                  /* attention receive sequence number */
    unsigned char ocMode;                       /* open connection mode */
    unsigned char ocInterval;                   /* open connection request retry interval */
    unsigned char ocMaximum;                    /* open connection request retry maximum */
};

typedef struct TRopenParams TRopenParams;

/* close connection parameters */
struct TRcloseParams {
    unsigned char abort;                        /* abort connection immediately if non-zero */
};

typedef struct TRcloseParams TRcloseParams;

/* client status parameter block */
struct TRstatusParams {
    TPCCB ccbPtr;                               /* pointer to ccb */
    unsigned short sendQPending;                /* pending bytes in send queue */
    unsigned short sendQFree;                   /* available buffer space in send queue */
    unsigned short recvQPending;                /* pending bytes in receive queue */
    unsigned short recvQFree;                   /* available buffer space in receive queue */
};

typedef struct TRstatusParams TRstatusParams;

/* read/write parameter block */
struct TRioParams {
    unsigned short reqCount;                    /* requested number of bytes */
    unsigned short actCount;                    /* actual number of bytes */
    unsigned char *dataPtr;                     /* pointer to data buffer */
    unsigned char eom;                          /* indicates logical end of message */
    unsigned char flush;                        /* send data now */
};

typedef struct TRioParams TRioParams;

/* attention parameter block */
struct TRattnParams {
    unsigned short attnCode;                    /* client attention code */
    unsigned short attnSize;                    /* size of attention data */
    unsigned char *attnData;                    /* pointer to attention data */
    unsigned char attnInterval;                 /* retransmit timer in 10-tick intervals */
};

typedef struct TRattnParams TRattnParams;

/* client send option parameter block */
struct TRoptionParams {
    unsigned short sendBlocking;                /* quantum for data packets */
    unsigned char sendTimer;                    /* send timer in 10-tick intervals */
    unsigned char rtmtTimer;                    /* retransmit timer in 10-tick intervals */
    unsigned char badSeqMax;                    /* threshold for sending retransmit advice */
    unsigned char useCheckSum;                  /* use ddp packet checksum */
};

typedef struct TRoptionParams TRoptionParams;

/* new cid parameters */
struct TRnewcidParams {
    unsigned short newcid;                      /* new connection id returned */
};

typedef struct TRnewcidParams TRnewcidParams;

/* ADSP CntrlParam ioQElement */
struct DSPParamBlock
    {
    struct            QElem *qLink;
    short            qType;
    short            ioTrap;
    Ptr             ioCmdAddr;
    ProcPtr         ioCompletion;
    OSErr            ioResult;
    char            *ioNamePtr;
    short            ioVRefNum;
    short            ioCRefNum;                 /* adsp driver refNum */
    short            csCode;                    /* adsp driver control code */
    long            qStatus;                    /* adsp internal use */
    short            ccbRefNum;                 /* connection end refNum */
    union
        {
            TRinitParams    initParams;         /* dspInit, dspCLInit */
            TRopenParams    openParams;         /* dspOpen, dspCLListen, dspCLDeny */
            TRcloseParams    closeParams;       /* dspClose, dspRemove */
            TRioParams        ioParams;         /* dspRead, dspWrite */
            TRattnParams    attnParams;         /* dspAttention */
            TRstatusParams    statusParams;     /* dspStatus */
            TRoptionParams    optionParams;     /* dspOptions */
            TRnewcidParams    newCIDParams;     /* dspNewCID */
            } u;
    };

typedef struct DSPParamBlock DSPParamBlock;
typedef struct DSPParamBlock *DSPPBPtr;



#endif
