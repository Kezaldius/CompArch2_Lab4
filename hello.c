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
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");


static unsigned int print_count = 1;
module_param(print_count, uint, S_IRUGO);
MODULE_PARM_DESC(print_count, "Number of times to print 'Hello, world!'");


struct timestamp_entry {
	struct list_head list;
	ktime_t timestamp;
};


static LIST_HEAD(timestamp_list);

static int __init hello_init(void)
{
	unsigned int i;

	if (print_count == 0 || (print_count >= 5 && print_count <= 10)) {
		pr_warn("Warning: print_count is %u, continuing.\n", print_count);
	} else if (print_count > 10) {
		pr_err("Error: print_count is %u, which is greater than 10. Aborting.\n", print_count);
		return -EINVAL; 
	}

	for (i = 0; i < print_count; i++) {
		struct timestamp_entry *new_entry;

		new_entry = kmalloc(sizeof(*new_entry), GFP_KERNEL);
		if (!new_entry) {
			pr_err("Failed to allocate memory for timestamp_entry\n");
			return -ENOMEM;
		}

		new_entry->timestamp = ktime_get();
		list_add_tail(&new_entry->list, &timestamp_list);

		printk(KERN_EMERG "Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct timestamp_entry *entry, *next;

	list_for_each_entry_safe(entry, next, &timestamp_list, list) {
		pr_info("Event timestamp (ns): %lld\n", ktime_to_ns(entry->timestamp));
		list_del(&entry->list);
		kfree(entry);
	}
	pr_info("Module unloaded.\n");
}

module_init(hello_init);
module_exit(hello_exit);
