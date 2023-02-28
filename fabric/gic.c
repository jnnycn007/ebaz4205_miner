/* Driver for the H3 GIC
 *
 * The GIC is the ARM "Generic Interrupt Controller"
 * It has two sections, "cpu" and "dist"
 *
 * The GIC in the Allwinner is the PL400
 * See the PL400 TRM and
 * the ARM GIC architecture specification, version 2.0
 *
 * The GIC in the Zynq-7000 is the earlier PL390
 * This code works for either
 *
 * Tom Trebisky  1-4-2017
 * Tom Trebisky  1-14-2021
 */

/* The H3 defines 157 interrupts (0-156)
 * We treat this as 160, more or less
 */
#define NUM_IRQ_H3		157
#define NUM_IRQ_ZYNQ		92

/* Never used */
#define NUM_IRQ		NUM_IRQ_ZYNQ

#define NUM_CONFIG	10
#define NUM_TARGET	40
#define NUM_PRIO	40
#define NUM_MASK	5

struct gic_dist {
	volatile unsigned long ctrl;		/* 0x00 */
	volatile unsigned long type;		/* 0x04 */
	volatile unsigned long iidr;		/* 0x08 */
	int __pad0[61];
	volatile unsigned long eset[NUM_MASK];		/* BG - 0x100 */
	int __pad1[27];
	volatile unsigned long eclear[NUM_MASK];	/* BG - 0x180 */
	int __pad2[27];
	volatile unsigned long pset[NUM_MASK];		/* BG - 0x200 */
	int __pad3[27];
	volatile unsigned long pclear[NUM_MASK];	/* BG - 0x280 */
	int __pad4[27];
	volatile unsigned long aset[NUM_MASK];		/* BG - 0x300 */
	int __pad5[27];
	volatile unsigned long aclear[NUM_MASK];	/* BG - 0x300 */
	int __pad55[27];
	volatile unsigned long prio[NUM_PRIO];		/* BG - 0x400 */
	int __pad6[216];
	volatile unsigned long target[NUM_TARGET];	/* 0x800 */
	int __pad7[216];
	volatile unsigned long config[NUM_CONFIG];	/* 0xc00 */
	int __pad8[182];
	volatile unsigned long soft;		/* 0xf00 */
};

struct gic_cpu {
	volatile unsigned long ctrl;		/* 0x00 */
	volatile unsigned long primask;		/* 0x04 */
	volatile unsigned long binpoint;	/* 0x08 */
	volatile unsigned long iack;		/* 0x0c */
	volatile unsigned long eoi;		/* 0x10 */
	volatile unsigned long run_pri;		/* 0x14 */
	volatile unsigned long high_pri;	/* 0x18 */
};

#ifdef notdef
/* Allwinner H3 */
#define GIC_DIST_BASE	((struct h3_gic_dist *) 0x01c81000)
#define GIC_CPU_BASE	((struct h3_gic_cpu *) 0x01c82000)
#endif

/* Zynq-7000 */
#define GIC_DIST_BASE	((struct gic_dist *) 0xf8f01000)
#define GIC_CPU_BASE	((struct gic_cpu *)  0xf8f00100)

#define	G0_ENABLE	0x01
#define	G1_ENABLE	0x02


#ifdef notdef
#define GIC_CPU_CTRL                    0x00
#define GIC_CPU_PRIMASK                 0x04
#define GIC_CPU_BINPOINT                0x08
#define GIC_CPU_INTACK                  0x0c
#define GIC_CPU_EOI                     0x10
#define GIC_CPU_RUNNINGPRI              0x14
#define GIC_CPU_HIGHPRI                 0x18

#define GIC_DIST_CTRL                   0x000
#define GIC_DIST_CTR                    0x004
#define GIC_DIST_ENABLE_SET             0x100
#define GIC_DIST_ENABLE_CLEAR           0x180
#define GIC_DIST_PENDING_SET            0x200
#define GIC_DIST_PENDING_CLEAR          0x280
#define GIC_DIST_ACTIVE_BIT             0x300
#define GIC_DIST_PRI                    0x400
#define GIC_DIST_TARGET                 0x800
#define GIC_DIST_CONFIG                 0xc00
#define GIC_DIST_SOFTINT                0xf00
#endif

void printf ( char *, ... );

void
gic_enable ( int irq )
{
	struct gic_dist *gp = GIC_DIST_BASE;
	int x = irq / 32;
	unsigned long mask = 1 << (irq%32);

	gp->eset[x] = mask;
}

void
gic_disable ( int irq )
{
	struct gic_dist *gp = GIC_DIST_BASE;
	int x = irq / 32;
	unsigned long mask = 1 << (irq%32);

	gp->eclear[x] = mask;
}

void
gic_unpend ( int irq )
{
	struct gic_dist *gp = GIC_DIST_BASE;
	int x = irq / 32;
	unsigned long mask = 1 << (irq%32);

	gp->pclear[x] = mask;
}

void
gic_handler ( void )
{
	struct gic_cpu *cp = GIC_CPU_BASE;
	int irq;

	irq = cp->iack;

	/* Do we need to EOI the spurious ? */
	if ( irq == 1023 ) {
	    uart_putc ( 'X' );
	    uart_putc ( '\n' );
	    // return;
	}

	cp->eoi = irq;

	// uart_putc ( '.' );
	// uart_putc ( '\n' );

	// printf ( "GIC iack = %08x\n", irq );
	// ms_delay ( 200 );
}

int
gic_irqwho ( void )
{
	struct gic_cpu *cp = GIC_CPU_BASE;

	return cp->iack;
}

void
gic_irqack ( int irq )
{
	struct gic_cpu *cp = GIC_CPU_BASE;

	cp->eoi = irq;
	gic_unpend ( irq );
}

void
gic_init ( void )
{
	struct gic_dist *gp = GIC_DIST_BASE;
	struct gic_cpu *cp = GIC_CPU_BASE;
	unsigned long *p;
	int i;

#ifdef notdef
	p = (unsigned long *) & gp->target;
	printf ( "GIC target = %08x\n", p );
	p = (unsigned long *) & gp->config;
	printf ( "GIC config  = %08x\n", p );
	p = (unsigned long *) & gp->soft;
	printf ( "GIC soft = %08x\n", p );

	/* yields: GIC iidr = 0100143B
	 * The 43B part is correct for ARM.
	printf ( "GIC iidr = %08x\n", gp->iidr );
	 */
#endif

	/* Initialize the distributor */
	gp->ctrl = 0;

	/* make all SPI level triggered */
	for ( i=2; i<NUM_CONFIG; i++ )
	    gp->config[i] = 0;

	for ( i=8; i<NUM_TARGET; i++ )
	    gp->target[i] = 0x01010101;

	for ( i=8; i<NUM_PRIO; i++ )
	    gp->prio[i] = 0xa0a0a0a0;

	for ( i=1; i<NUM_MASK; i++ )
	    gp->eclear[i] = 0xffffffff;

	for ( i=0; i<NUM_MASK; i++ )
	    gp->pclear[i] = 0xffffffff;

	gp->ctrl = G0_ENABLE;

	/* ** now initialize the per CPU stuff.
	 *  XXX - the following will need to be done by each CPU
	 *  when we get multiple cores running.
	 */

	/* enable all SGI, disable all PPI */
	gp->eclear[0] = 0xffff0000;
	gp->eset[0]   = 0x0000ffff;

	/* priority for PPI and SGI */
	for ( i=0; i<8; i++ )
	    gp->prio[i] = 0xa0a0a0a0;

	cp->primask = 0xf0;
	cp->ctrl = 1;
}

#ifdef notdef
extern volatile int timer_count;

/* This is TIMER 0 */
#define TIMER_MASK	0x40000

void
gic_check ( void )
{
	struct gic_dist *gp = GIC_DIST_BASE;

	printf ( " GIC pending: %08x %08x %d\n", gp->pset[0], gp->pset[1], timer_count );
}

void
gic_poll ( void )
{
	struct gic_dist *gp = GIC_DIST_BASE;

	for ( ;; ) {
	    // ms_delay ( 2000 );
	    if ( gp->pset[1] & TIMER_MASK ) {
		gic_check ();
		gic_handler ();
		printf ( "+GIC pending: %08x %08x %d\n", gp->pset[0], gp->pset[1], timer_count );
	    }
	}
}

void
gic_watch ( void )
{

	for ( ;; ) {
	    delay_x ();
	    delay_x ();
	    gic_check ();
	}
}
#endif

/* THE END */
