
#ifndef __BOOGA_H
#define __BOOGA_H
/*
 * booga.h -- PROTOTYPES FOR BOOGA.C
 *
 */
#ifndef BOOGA_MAJOR
#define BOOOGA_MAJOR 0   /* dynamic major by default */
#endif

#ifndef BOOGA_NR_DEVS
#define BOOGA_NR_DEVS 4    /* booga0 through booga3 */
#endif
/*
 * Split minors in two parts
 */
#define TYPE(dev)   (MINOR(dev) >> 4)  /* high nibble */
#define NUM(dev)    (MINOR(dev) & 0xf) /* low  nibble */

/*
 * The different configurable parameters
 */
struct booga_stats {
	long int num_open;
	long int num_read; 
	long int num_write; 
	long int num_close; 
    long int device_num; 
    long int phraseCount1; //splitting up to avoid overflow
	long int phraseCount2;
	long int phraseCount3;
	long int phraseCount4;
    long int openCount1;
	long int openCount2; 
	long int openCount3;
	long int openCount4;
	struct semaphore sem;
};
typedef struct booga_stats booga_stats;

/**
 * opens target device to be able to access
 */
static int booga_open (struct inode *inode, struct file *filp);

/**
 * Releases device from read or write mode
 */
static int booga_release (struct inode *inode, struct file *filp);

/**
 * Read the device
 */
static ssize_t booga_read (struct file *filp, char *buf, size_t count, loff_t *f_pos);

/**
 * Write to the device
 */
static ssize_t booga_write (struct file *filp, const char *buf, size_t count , loff_t *f_pos);

/**
 * Initializes everything in struct booga_stats
 */
static __init int booga_init(void);

/**
 * Frees everything in struct booga_stats
 */
static __exit  void booga_cleanup(void);

/*extern booga_stats booga_Device_Stats;*/

#endif /* __BOOGA_H */