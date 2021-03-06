typedef struct Conf	Conf;
typedef struct Confmem	Confmem;
typedef struct FPsave	FPsave;
typedef struct PFPU	PFPU;
typedef struct ISAConf	ISAConf;
typedef struct Imap	Imap;
typedef struct Label	Label;
typedef struct Lock	Lock;
typedef struct Mach	Mach;
typedef struct PCArch	PCArch;
typedef struct PMMU	PMMU;
typedef struct Page	Page;
typedef struct Pcidev	Pcidev;
typedef struct Proc	Proc;
typedef struct Sys	Sys;
typedef struct Ureg	Ureg;
typedef struct Vctl	Vctl;
typedef long		Tval;

#pragma incomplete Ureg
#pragma incomplete Imap

#define MAXSYSARG	5	/* for mount(fd, mpt, flag, arg, srv) */

/*
 *  parameters for sysproc.c
 */
#define AOUT_MAGIC	Q_MAGIC

/*
 *  machine dependent definitions used by ../port/dat.h
 */

struct Lock
{
	ulong	key;			/* semaphore (non-zero = locked) */
	ulong	sr;
	ulong	pc;
	Proc	*p;
	ulong	pid;
	ushort	isilock;
};

struct Label
{
	ulong	sp;
	ulong	pc;
};

/*
 * This structure must agree with fpsave and fprestore asm routines
 */
struct FPsave
{
	double	fpreg[32];
	union {
		double	fpscrd;
		struct {
			ulong	pad;
			ulong	fpscr;
		};
	};
};

struct PFPU
{
	int	fpstate;
	FPsave	fpsave[1];
};

enum
{
	/* Floating point states */
	FPinit = 0,
	FPactive = 1,
	FPinactive = 2,
	/* Bit that's or-ed in during note handling (FP is illegal in note handlers) */
	FPillegal = 0x100,
};

struct Confmem
{
	ulong	base;
	ulong	npage;
	ulong	kbase;
	ulong	klimit;
};

struct Conf
{
	ulong	nmach;		/* processors */
	ulong	nproc;		/* processes */
	Confmem	mem[2];
	ulong	npage;		/* total physical pages of memory */
	ulong	upages;		/* user page pool */
	ulong	nimage;		/* number of page cache image headers */
	ulong	nswap;		/* number of swap pages */
	int	nswppo;		/* max # of pageouts per segment pass */
	ulong	copymode;	/* 0 is copy on write, 1 is copy on reference */
	int	monitor;	/* has display? */
	ulong	ialloc;		/* bytes available for interrupt time allocation */
	ulong	pipeqsize;	/* size in bytes of pipe queues */
};

/*
 *  mmu goo in the Proc structure
 */
#define NCOLOR 1
struct PMMU
{
	int	mmupid;
	Ureg	*mmureg;		/* pointer to ureg structure */
};

#include "../port/portdat.h"

/*
 *  machine dependent definitions not used by ../port/dat.h
 */
/*
 * Fake kmap
 */
typedef	void		KMap;
#define	VA(k)		((ulong)(k))
#define	kmap(p)		(KMap*)((p)->pa|KZERO)
#define	kunmap(k)

struct Mach
{
	/* OFFSETS OF THE FOLLOWING KNOWN BY l.s */
/*0x00*/	int	machno;	/* physical id of processor */
/*0x04*/	ulong	splpc;	/* pc that called splhi() */
/*0x08*/	Proc	*proc;	/* current process on this processor */
	/* Debugging/statistics for software TLB in l.s (therefore, also known by l.s) */
/*0x0c*/	ulong	tlbfault;	/* type of last miss */
/*0x10*/	ulong	imiss;	/* number of instruction misses */
/*0x14*/	ulong	dmiss;	/* number of data misses */

	/* ordering from here on irrelevant */
	PMach;

	Imap*	imap;

	uintptr	ptabbase;	/* start of page table in kernel virtual space */
	int	slotgen;	/* next pte (byte offset) when pteg is full */
	int	mmupid;		/* next mmu pid to use */
	int	sweepcolor;
	int	trigcolor;
	Rendez	sweepr;

	int	cputype;
	ulong	loopconst;

	ulong	clkin;		/* basic clock frequency */
	ulong	vco_out;
	vlong	cpuhz;
	ulong	bushz;
	ulong	dechz;
	ulong	tbhz;
	ulong	cpmhz;		/* communications processor module frequency */
	ulong	brghz;		/* baud rate generator frequency */

	/* MUST BE LAST */
	uintptr	stack[1];
};

struct
{
	char	machs[MAXMACH];
	int	exiting;
}active;

/*
 *  a parsed plan9.ini line
 */
#define NISAOPT		8

struct ISAConf {
	char	*type;
	ulong	port;
	int	irq;
	ulong	dma;
	ulong	mem;
	ulong	size;
	ulong	freq;

	int	nopt;
	char	*opt[NISAOPT];
};

struct Vctl {
	Vctl*	next;		/* handlers on this vector */

	char	name[KNAMELEN];	/* of driver */
	int	isintr;		/* interrupt or fault/trap */
	int	irq;

	void	(*f)(Ureg*, void*);	/* handler to call */
	void*	a;		/* argument to call it with */
};

extern Mach mach0;

extern register Mach *m;
extern register Proc *up;

extern FPsave initfp;
