
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>


#include "linux/ktime.h"
#include "linux/slab.h"

MODULE_AUTHOR("Kovalkov Dima");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Labworks");
MODULE_LICENSE("Dual BSD/GPL");

static uint k = 1;
module_param(k, uint, 0444);
MODULE_PARM_DESC(k, "Number of times that message must be repeated");

struct time_list {
	struct time_list *next;
	ktime_t time;
};

static struct time_list *head;

static int __init hello_init(void)
{
	struct time_list *tail;
	uint i;

	if (k == 0) {
		printk(KERN_WARNING "k=%i is 0\n", k);
		return 0;
	} else if (k >= 5 && k <= 10) {
		printk(KERN_WARNING "k=%i in range of 5,10\n", k);
	} else if (k > 10) {
		printk(KERN_ERR "Parameter k=%i is greater than 10\n", k);
		return -EINVAL;
	}

	head = kmalloc(sizeof(struct time_list *), GFP_KERNEL);
	head->next = NULL;
	head->time = ktime_get();
	tail = head;
	printk(KERN_INFO "Hello, world!\n");

	for (i = 1; i < k; i++) {
		tail->next = kmalloc(sizeof(struct time_list *), GFP_KERNEL);
		if (tail->next == NULL) {
			while (head != NULL) {
				tail = head;
				head = tail->next;
				kfree(tail);
			}
			printk(KERN_ERR "Out of memory");
			return -ENOMEM;
		}
		tail = tail->next;
		tail->next = NULL;
		tail->time = ktime_get();
		printk(KERN_INFO "Hello, world!\n");
	}

	return 0;

}

static void __exit hello_exit(void)
{
	struct time_list *tail;

	while (head != NULL) {
		tail = head;
		pr_info("Time: %lld\n", tail->time);
		head = tail->next;
		kfree(tail);
	}
}

module_init(hello_init);
module_exit(hello_exit);
