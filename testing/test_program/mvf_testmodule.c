#include <linux/module.h>
#include <linux/init.h>

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/console.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/compat.h>

#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/system.h>

#include "ftimer.h"
#include "pitimer.h"
#include "lptimer.h"

#ifdef CONFIG_LPTMR
static int lpt_interrupt_counter;
void lpt_test_handler( void)
{
	lpt_interrupt_counter++;
}

void lpt_test_main(void)
{
	int timer_handle;
	struct mvf_lpt_request req;
	unsigned long counter;

	printk("\n------------- LPT test -------------\n");

	timer_handle = lpt_alloc_timer();

	req.compare_value = 0x8000;
	req.timer_mode = LPT_PARAM_TM_TIMECOUNTER;
	req.pulse_pin_polarity = 0;
	req.pulse_pin_select = 0;
	req.prs_clock_sel = LPT_PARAM_PCS_CLOCK0;
	req.prs_bypass = LPT_PARAM_PB_GF_BYPASS;
	req.prs_value = LPT_PARAM_PV_DIV512_RISE256;


	printk("Alloc LPT %d\n", timer_handle);
	printk("  LPT Paramset (ret=%d)\n", lpt_param_set( timer_handle, &req, lpt_test_handler));
	printk("  LPT Enable (ret=%d)\n", lpt_enable_timer( timer_handle));

#if 0
	lpt_read_counter( timer_handle, &counter);
	printk("  LPT 1st read point : counter = %d\n",counter);

	lpt_read_counter( timer_handle, &counter);
	printk("  LPT 2nd read point : counter = %d\n",counter);
#endif

	//	2. wait interrupt occur
	printk("  < Waiting 1000 msec >\n");
	mdelay(1000);
	printk("  LPT Load value = %x(hex) : Interrupt count = %d / 1000 msec. \n", req.compare_value, lpt_interrupt_counter);
	
	lpt_read_counter( timer_handle, &counter);
	printk("  LPT 1st read point : counter = %d\n",counter);

	lpt_read_counter( timer_handle, &counter);
	printk("  LPT 2nd read point : counter = %d\n",counter);

	printk("  LPT Disable (ret=%d)\n", lpt_disable_timer( timer_handle));
	printk("Free LPT (ret=%d)\n\n", lpt_free_timer( timer_handle));
}
#endif

#ifdef CONFIG_FTM
static int ftm_interrupt_counter[2];
void ftm_test_handler( int index)
{
	ftm_interrupt_counter[index]++;
}

void ftm_test_main(void)
{
//	int index0, index1;
	int index[3],i;
	int invalid_index;
	unsigned long counter;

	struct mvf_ftm_request req;

	printk("\n------------- FTM test -------------\n");

	for( i = 0; i < 3; i ++){
#if 0
		index[i] = ftm_alloc_timer( i);
#else
		index[i] = ftm_alloc_timer( FMT_AVAILABLE_CHANNEL);
#endif

		printk("Alloc FTM%d (Handle=%d)\n", i, index[i]);

		if (i==2) break;

		req.clocksource = FTM_PARAM_CLK_SYSTEMCLOCK;
		if ( i==0){
			req.divider		= FTM_PARAM_DIV_BY_128;
		}else{
			req.divider		= FTM_PARAM_DIV_BY_32;
		}
		req.start		= 0x0001;
		req.end		= 0xfff0;
		printk("  FTM%d Paramset (ret=%d)\n",i, ftm_param_set( index[i], &req, ftm_test_handler));


		//	1. start interrupt
		printk("  FTM%d Enable (ret=%d)\n", i, ftm_enable_timer( index[i]));
		mdelay(1000);

		//	2. wait interrupt occur
		printk("  < Waiting 1000 msec >\n");
		mdelay(1000);

		//	3. display result
		printk("  FTM%d Load value = %x(hex) : Divider %x : Interrupt count = %d / 1000 msec. \n", index[i],  (req.end - req.start), req.divider, ftm_interrupt_counter[i]);

		//	4. 1st read
		ftm_read_counter( index[i], &counter);
		printk("  FTM%d 1st read point : counter = %d\n",i, counter);

		//	5. 2nd read
		ftm_read_counter( index[i], &counter);
		printk("  FTM%d 2nd read point : counter = %d\n",i, counter);

		//	6. disable timer
		printk("  FTM%d Disable (ret=%d)\n", i, ftm_disable_timer( index[i]));

#if 0
		printk("Free FTM%d (ret=%d)\n\n", i, ftm_free_timer( index[i]));
#endif
	}

#if 1
	printk("\n------------- FTM edit param and restart -------------\n");
	for( i = 0; i < 2; i ++){
		printk("Alloc FTM%d (Handle=%d)\n", i, index[i]);
		req.clocksource = FTM_PARAM_CLK_SYSTEMCLOCK;
		if ( i==0){
			req.divider		= FTM_PARAM_DIV_BY_128;
		}else{
			req.divider		= FTM_PARAM_DIV_BY_32;
		}
		req.start		= 0x0001;
		req.end		= 0x8ff0;
		printk("  FTM%d Paramset (ret=%d)\n",i, ftm_param_set( index[i], &req, ftm_test_handler));


		//	1. start interrupt
		printk("  FTM%d Enable (ret=%d)\n", i, ftm_enable_timer( index[i]));
		mdelay(1000);

		//	2. wait interrupt occur
		printk("  < Waiting 1000 msec >\n");
		mdelay(1000);

		//	3. display result
		printk("  FTM%d Load value = %x(hex) : Divider %x : Interrupt count = %d / 1000 msec. \n", index[i],  (req.end - req.start), req.divider, ftm_interrupt_counter[i]);

		//	4. 1st read
		ftm_read_counter( index[i], &counter);
		printk("  FTM%d 1st read point : counter = %d\n",i, counter);

		//	5. 2nd read
		ftm_read_counter( index[i], &counter);
		printk("  FTM%d 2nd read point : counter = %d\n",i, counter);

		//	6. disable timer
		printk("  FTM%d Disable (ret=%d)\n", i, ftm_disable_timer( index[i]));
	}
#endif


#if 1
	for( i = 0; i < 2; i ++){
		printk("Free FTM%d (ret=%d)\n\n", i, ftm_free_timer( index[i]));
	}
#endif
}
#endif

#ifdef CONFIG_PIT
static int pit_interrupt_counter[10];

void pit_test_handler( int index)
{
	pit_interrupt_counter[index]++;
}

void pit_test_main(void)
{
	int index[9];
	int invalid_index;
	unsigned long counter;
	unsigned long flags;
	int i;

	printk("\n------------- PIT test -------------\n");

	for( i=0; i < 9; i ++){
#if 0
		index[i] = pit_alloc_timer( i);
#else
		index[i] = pit_alloc_timer( PIT_AVAILABLE_CHANNEL);
#endif

		printk("Alloc PIT%d (Handle=%d)\n", i, index[i]);

		if (i==8) break;

		printk("  PIT%d Paramset (ret=%d)\n",i, pit_param_set( index[i], (0x00001100) * (i+1) * (i+1), pit_test_handler));

		//	1. start interrupt
		printk("  PIT%d Enable (ret=%d)\n", i, pit_enable_timer( index[i]));

		//	2. wait interrupt occur
		printk("  < Waiting 1000 msec >\n");
		mdelay(1000);

		//	3. display result
		printk("  PIT%d Load value = %x(hex) : Interrupt count = %d / 1000 msec. \n", index[i], (0x00001100) * (i+1) * (i+1), pit_interrupt_counter[i]);

		//	4. 1st read
		pit_read_counter( index[i], &counter);
		printk("  PIT%d 1st read point : counter = %d\n",i, counter);

		//	5. 2nd read
		pit_read_counter( index[i], &counter);
		printk("  PIT%d 2nd read point : counter = %d\n",i, counter);

		//	6. disable timer
		printk("  PIT%d Disable (ret=%d)\n", i, pit_disable_timer( index[i]));

		//	7. check stopping timer
		printk("  PIT%d Counter stop check ", i );
		pit_read_counter( index[i], &counter);
		printk(" 1st %d", counter);
		pit_read_counter( index[i], &counter);
		printk(" 2nd %d\n", counter);

#if 0
		printk("Free PIT%d (ret=%d)\n\n", i, pit_free_timer( index[i]));
#endif
	}

#if 1
	printk("\n------------- PIT edit param and restart -------------\n");
	for( i=0; i < 8; i ++){
		printk("  PIT%d Paramset (ret=%d)\n",i, pit_param_set( index[i], (0x00002200) * (i+1) * (i+1), pit_test_handler));

		//	1. start interrupt
		printk("  PIT%d Enable (ret=%d)\n", i, pit_enable_timer( index[i]));

		//	2. wait interrupt occur
		printk("  < Waiting 1000 msec >\n");
		mdelay(1000);

		//	3. display result
		printk("  PIT%d Load value = %x(hex) : Interrupt count = %d / 1000 msec. \n", index[i], (0x00001100) * (i+1) * (i+1), pit_interrupt_counter[i]);

		//	4. 1st read
		pit_read_counter( index[i], &counter);
		printk("  PIT%d 1st read point : counter = %d\n",i, counter);

		//	5. 2nd read
		pit_read_counter( index[i], &counter);
		printk("  PIT%d 2nd read point : counter = %d\n",i, counter);

		//	6. disable timer
		printk("  PIT%d Disable (ret=%d)\n", i, pit_disable_timer( index[i]));

		//	7. check stopping timer
		printk("  PIT%d Counter stop check ", i );
		pit_read_counter( index[i], &counter);
		printk(" 1st %d", counter);
		pit_read_counter( index[i], &counter);
		printk(" 2nd %d\n", counter);
	}
#endif

#if 1
	for( i=0; i < 8; i ++){
		printk("Free PIT%d (ret=%d)\n\n", i, pit_free_timer( index[i]));
	}
#endif

}
#endif

static int __init test_init_module( void)
{
#ifdef CONFIG_PIT
	pit_test_main();
#endif

#ifdef CONFIG_FTM
	ftm_test_main();
#endif

#ifdef CONFIG_LPTMR
	lpt_test_main();
#endif

	return 0;
}
static void test_cleanup_module( void)
{
}

module_init(test_init_module);
module_exit(test_cleanup_module);
