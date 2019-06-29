typedef struct LINKTYPE {
	struct LINKTYPE *next;
	struct LINKTYPE *previous;
	char *item;
	} LINKTYPE;
	
typedef struct LINKLIST {
	struct LINKTYPE *head;
	struct LINKTYPE *tail;
	struct LINKTYPE *clp;
	int listlength;
	int itemlength;
	int (*match)();
	} LINKLIST;
	
resource 'mxwt' (143) {
		"LINKTYPE",
			"next", 	"^LINKTYPE", 1,
			"previous", "^LINKTYPE", 1,
			"item *",	"Long",		 1
		},
		"LINKLIST",
		{	/* array fields: 6 elements */
			/* [1] */
			"head",
			"^LINKTYPE",
			1,
			/* [2] */
			"tail",
			"^LINKTYPE",
			1,
			/* [3] */
			"clp",
			"^LINKTYPE",
			1,
			/* [4] */
			"list length",
			"UnsignedLong",
			1,
			/* [5] */
			"item length",
			"UnsignedLong",
			1,
			/* [6] */
			"match function",
			"Long",
			1
		},
		/* [3] */
		"EPPCBlk",
		{	/* array fields: 6 elements */
			/* [1] */
			"nameOfMsgQHdl",
			"^^pString",
			1,
			/* [2] */
			"muid",
			"Long",
			1,
			/* [3] */
			"msgBuffSize",
			"UnsignedLong",
			1,
			/* [4] */
			"optionFlags",
			"Long",
			1,
			/* [5] */
			"portID",
			"Long",
			1,
			/* [6] */
			"buffLocHdl",
			"Handle",
			1
		},
		/* [4] */
		"MFID",
		{
		"localPid",
		"ulong",
		1;
	ulong	hostID;
	} MFID;
	
	}
};

