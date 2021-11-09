/*
 * @Author Amara Tariq
 * 
 * booga.c - booga module
 * simulation of booga driver devices using semaphore
 */
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/version.h> /* printk() */
#include <linux/init.h>  /*  for module_init and module_cleanup */
#include <linux/slab.h>  /*  for kmalloc/kfree */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/proc_fs.h>	/* for the proc filesystem */
#include <linux/seq_file.h>
#include "booga.h"        /* local definitions */
#include <linux/random.h> /* get_random_bytes */
#include <linux/uaccess.h> /* copy_user */


#define DEVICE_NAME "booga" 
static int booga_major = BOOGA_MAJOR;
static int booga_nr_devs = BOOGA_NR_DEVS;    /* number of bare booga devices */
module_param(booga_major, int, 0);
module_param(booga_nr_devs, int, 0);
MODULE_AUTHOR("Amara Tariq");
MODULE_LICENSE("GPL v2");

static booga_stats *booga_device_stats;
static struct proc_dir_entry* booga_proc_file;

//random phrases, NEW PHRASE MUST BE SEPARATED BY A SPACE
static char* phrases[] = {"booga! booga! ", "googoo! gaagaa! ", "neka! maka! ", "wooga! wooga! "};

static ssize_t booga_read (struct file *, char *, size_t , loff_t *);
static ssize_t booga_write (struct file *, const char *, size_t , loff_t *);
static int booga_open (struct inode *, struct file *);
static int booga_release (struct inode *, struct file *);
static int booga_proc_open(struct inode *inode, struct file *file);


/*  The different file operations */
/* The syntax you see below is an extension to gcc. The prefered */
/* way to init structures is to use C99 Taged syntax */
/* static struct file_operations example_fops = { */
/* 		    .read    =       example_read, */
/* 			.write   =       example_write, */
/* 			.open    =       example_open, */
/* 			.release =       example_release */
/* }; */
/*  This is where we define the standard read,write,open and release function */
/*  pointers that provide the drivers main functionality. */
static struct file_operations booga_fops = {
		    read:       booga_read,
			write:      booga_write,
			open:       booga_open,
			release:    booga_release,
};

/* The file operations struct holds pointers to functions that are defined by */
/* driver impl. to perform operations on the device. What operations are needed */
/* and what they should do is dependent on the purpose of the driver. */
static const struct file_operations booga_proc_fops = {
		.owner	= THIS_MODULE,
		.open	= booga_proc_open,
		.read	= seq_read,
		.llseek	= seq_lseek,
		.release	= single_release,
};


/*
 * Open and close
 */
static int booga_open (struct inode *inode, struct file *filp)
{
		int num = NUM(inode->i_rdev);
        booga_device_stats -> device_num = num; //sets dev number
        printk (KERN_INFO "Booga: booga_open function called\n"); 

		if (num >= booga_nr_devs){
            return -ENODEV;
        }

        booga_device_stats -> openCount[num]++; //increment whenever device_num gets accessed
		
		/*
		if(num == 0){
			booga_device_stats -> openCount0++;
		}else if(num == 1){
			booga_device_stats -> openCount1++;
		}else if(num == 2){
			booga_device_stats -> openCount2++;
		}else if(num == 3){
			booga_device_stats -> openCount3++;
		}
		*/

		filp->f_op = &booga_fops;

		/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */
		if (down_interruptible (&booga_device_stats->sem)){
			return (-ERESTARTSYS);
        }

		booga_device_stats->num_open++;
		up(&booga_device_stats->sem);

		try_module_get(THIS_MODULE);
        printk(KERN_INFO "Booga: booga_open function was called successfully\n");
		return 0;          /* success */
}

static int booga_release (struct inode *inode, struct file *filp)
{
		/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */
		if (down_interruptible (&booga_device_stats->sem)){
            return (-ERESTARTSYS);
        }	
		booga_device_stats->num_close++;
		up(&booga_device_stats->sem);
        //printk(KERN_INFO "Booga: Device was closed successfully\n");
		module_put(THIS_MODULE);
		return 0;
}

/*
 * Data management: read and write
 */

static ssize_t booga_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
        //variables for function
        char randVal;
        int randIndex;
        char* phrase_randIndex = "";
        char* substr = "";

        int i = 0;
        int j;

		printk("<1>booga_read invoked.\n");
		/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */
		if (down_interruptible (&booga_device_stats->sem)){
				return (-ERESTARTSYS);
        }
        
        substr = (char*)kmalloc(sizeof(char)*count, GFP_KERNEL);

        //Getting a random phrase from array
        get_random_bytes(&randVal,1);
        randIndex = (randVal & 0x7F) % 4; //AND guarentees positivity
        phrase_randIndex = phrases[randIndex]; 
        
		//booga_device_stats -> phraseCount[randIndex]++; //increment as we aaccess

		if(randIndex == 0){
			booga_device_stats -> phraseCount0++;
		}else if(randIndex == 1){
			booga_device_stats -> phraseCount1++;
		}else if(randIndex == 2){
			booga_device_stats -> phraseCount2++;
		}else if(randIndex == 3){
			booga_device_stats -> phraseCount3++;
		}


        for(j = 0; j<count; j++){
            substr[j] = phrase_randIndex[i]; //creating one string with all chars according to user input
            i++;

            if(phrase_randIndex[i] == '\0'){ //if end of string char, reset to beginning of phrase
                i=0;
            }
        }

        if(copy_to_user(buf, substr, count)){
            return -ERESTARTSYS;
        }

        booga_device_stats -> num_read += count; //bytes read in
        //booga_device_stats->num_read++;
		up(&booga_device_stats->sem);
		return count;
}

static ssize_t booga_write (struct file *filp, const char *buf, size_t count , loff_t *f_pos)
{
        struct siginfo sig;

		printk("<1>booga_write invoked.\n");
		/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */
		if (down_interruptible (&booga_device_stats->sem))
				return (-ERESTARTSYS);

        //if attempt to write to booga3, we need to terminate wihtin kernel
        if(booga_device_stats -> device_num == 3){
            memset(&sig, 0, sizeof(struct siginfo));
            sig.si_signo = SIGTERM; //terminatation signal
            sig.si_code = SI_QUEUE; //set by sigqueue()
            sig.si_int = 1; //0 = sent by usermode, 1 = kernelmode
            send_sig_info(SIGTERM, &sig, current); 
        }else{
            booga_device_stats -> num_write += count; //increment bytes written
        }

		//booga_device_stats->num_write++;
		up(&booga_device_stats->sem);
		return count; // pretend that count bytes were written
}

static void init_booga_device_stats(void)
{
        int i;

		booga_device_stats->num_read=0;
		booga_device_stats->num_write=0;
		booga_device_stats->num_open=0;
		booga_device_stats->num_close=0;

		for(i = 0; i<4; i++){
            booga_device_stats -> openCount[i] = 0;
            //booga_device_stats -> phraseCount[i] = 0;
        }
		/*
        booga_device_stats -> openCount0 = 0;         
		booga_device_stats -> openCount1 = 0;         
		booga_device_stats -> openCount2 = 0;         
		booga_device_stats -> openCount3 = 0;  
		*/
		booga_device_stats -> phraseCount0 = 0;         
		booga_device_stats -> phraseCount1 = 0;         		
		booga_device_stats -> phraseCount2 = 0;         
		booga_device_stats -> phraseCount3 = 0;

		sema_init(&booga_device_stats->sem, 1);
}

static int booga_proc_show(struct seq_file *m, void *v)
{
        int i = 0; 
        
		//seq_printf(m, "open = %ld times\n", booga_device_stats->num_open);
		//seq_printf(m, "close = %ld times\n", booga_device_stats->num_close);
		seq_printf(m, "bytes read = %ld \n", booga_device_stats->num_read);
		seq_printf(m, "bytes written = %ld \n", booga_device_stats->num_write);

		seq_printf(m, "number of opens:\n");
        for(i=0; i<4;i++){
            seq_printf(m, " /dev/booga%d = %ld times\n", i, booga_device_stats -> openCount[i]);
        }
		/*
        seq_printf(m, "number of opens:\n");
        seq_printf(m, " /dev/booga0 = %ld times\n", booga_device_stats -> openCount0);
        seq_printf(m, " /dev/booga1 = %ld times\n", booga_device_stats -> openCount1);
        seq_printf(m, " /dev/booga2 = %ld times\n", booga_device_stats -> openCount2);
        seq_printf(m, " /dev/booga3 = %ld times\n", booga_device_stats -> openCount3);
		*/

        seq_printf(m, "strings output:\n");
        seq_printf(m, " booga! booga! = %ld times\n", booga_device_stats -> phraseCount0);
        seq_printf(m, " googoo! gaagaa! = %ld times\n", booga_device_stats -> phraseCount1);
		seq_printf(m, " wooga! wooga! = %ld times\n", booga_device_stats -> phraseCount3);
        seq_printf(m, " neka! maka! = %ld times\n", booga_device_stats -> phraseCount2);
        
        
		return 0;
}


static int booga_proc_open(struct inode *inode, struct file *file)
{
		return single_open(file, booga_proc_show, NULL);
}


static __init int booga_init(void)
{
		int result;

		/*
		 * Register your major, and accept a dynamic number
		 */
		result = register_chrdev(booga_major, "booga", &booga_fops);
		if (result < 0) {
				printk(KERN_WARNING "booga: can't get major %d\n",booga_major);
				return result;
		}
		if (booga_major == 0) booga_major = result; /* dynamic */
		printk("<1> booga device driver version 4: loaded at major number %d\n", booga_major);

		booga_device_stats = (booga_stats *) kmalloc(sizeof(booga_stats),GFP_KERNEL);
		if (!booga_device_stats) {
				result = -ENOMEM;
				goto fail_malloc;
		}
		init_booga_device_stats();

		/* We assume that the /proc/driver exists. Otherwise we need to use proc_mkdir to
		 * create it as follows: proc_mkdir("driver", NULL);
		 */
		booga_proc_file = proc_create("driver/booga", 0, NULL, &booga_proc_fops);
		if (!booga_proc_file)  {
				result = -ENOMEM;
				goto fail_malloc;
		}

		return 0;

fail_malloc:
		unregister_chrdev(booga_major, "booga");
		return  result;
}



static __exit  void booga_exit(void)
{
		remove_proc_entry("driver/booga", NULL /* parent dir */);
		kfree(booga_device_stats);
		unregister_chrdev(booga_major, "booga");
		printk("<1> booga device driver version 4: unloaded\n");
}


module_init(booga_init);
module_exit(booga_exit);

/* vim: set ts=4: */
