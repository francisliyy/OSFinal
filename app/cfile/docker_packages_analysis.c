#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/netfilter.h>
#include<linux/skbuff.h>
#include<linux/netfilter_ipv4.h>
#include<linux/net.h>
#include<linux/ip.h>
#include<linux/tcp.h>
#include<net/net_namespace.h>
#include<net/ipv6.h>
#include<linux/inet.h>
#include<linux/fs.h>
#include<asm/segment.h>
#include<asm/uaccess.h>
#include<linux/buffer_head.h>
#include<linux/list.h>
#include<linux/spinlock.h>
#include<linux/kthread.h>

DEFINE_SPINLOCK(list_lock);
static struct task_struct *task;

struct Node {
	int source[4];
	int dest[4];
	int count;
	struct list_head list;
};
typedef struct Node IPNODE; 
static LIST_HEAD(ip_head);

//judging two ip is the same one
int match_ip(int *a, int *b) {
	int i, f;
	f = 1;
	for (i = 0; i < 4; i++) {
		if (a[i] != b[i]) {
			f = 0;
			break;
		}
	}
	return f;
}

// filter the package that is not from docker container or to docker container.
int ip_filter(int *a, int *b) {
	if ((a[0] == 172 && a[3] == 1) || (b[0] == 172 && b[3] == 1)) return 0; 
	if (a[0] == 172 && b[0] == 172 && a[3] != b[3]) return 1;
	return 0;
}

//if a packages from new source to new destination, insert this into list.
void insert_new(int *s, int *d) {
	IPNODE *new_node = NULL;
	int i;
	new_node = (IPNODE *)kmalloc(sizeof(IPNODE), GFP_ATOMIC);
	if (new_node) {
		new_node->count = 1;
		for (i = 0; i < 4; i++) {
			new_node->source[i] = s[i];
			new_node->dest[i] = d[i];
		}
		list_add(&new_node->list, &ip_head);
	}
}

// count the package in the network.
int insert_and_count(unsigned char *s, unsigned char *d) {
	IPNODE *pointer;
	int source[4];
	int dest[4];
	int i;
	int exist = 0;
	for (i = 0; i < 4; i++) {
		source[i] = s[i];
		dest[i] = d[i];
	}
	if (!ip_filter(source, dest)) {
		return 0;
	}
	spin_lock_irq(&list_lock);
	list_for_each_entry(pointer, &ip_head, list) {
		if (match_ip(pointer->source, source) && match_ip(pointer->dest, dest)) {
			pointer->count = pointer->count + 1;
			exist = 1;
		}
	}
	if (exist == 0) {
		insert_new(source, dest);
	}
	spin_unlock_irq(&list_lock);
	return 0;
}

//print source ip, destination ip and the number of packages between two ips to a file.
static int printer_ip_node(void *data) {
	IPNODE *pointer;
	struct file *fp = NULL;
	char *out_buff;
	char str[40];
	int i, count = 0;
	int buff_size = 1000;
	mm_segment_t fs;
	loff_t pos;
	while(!kthread_should_stop()) {
		pos = 0;
		fp = filp_open("/home/lyx/osfinal/visualTool/app/static/csv/logA.csv", O_CREAT | O_RDWR, 00777);
		if (IS_ERR(fp)) {
			printk("create file error/n");
			return -1;
		}	
		count = 0;
		out_buff = (char *)kmalloc(sizeof(char) * buff_size, GFP_ATOMIC);
		for (i = 0; i < buff_size; i++) { out_buff[i] = 0; }
		strcpy(out_buff, "from_ip_address,to_ip_address,package_count\n");
		printk("print the statistic of packages betwenn docker containers");
		spin_lock_irq(&list_lock);
		list_for_each_entry(pointer, &ip_head, list) {
			printk("source address: %d.%d.%d.%d destination address: %d.%d.%d.%d, count:%d ",
				pointer->source[0], pointer->source[1], pointer->source[2], pointer->source[3],
				pointer->dest[0], pointer->dest[1], pointer->dest[2], pointer->dest[3], pointer->count);

			sprintf(str, "%d.%d.%d.%d,%d.%d.%d.%d,%d\n",
				pointer->source[0], pointer->source[1], pointer->source[2], pointer->source[3],
				pointer->dest[0], pointer->dest[1], pointer->dest[2], pointer->dest[3], pointer->count);
			strcat(out_buff, str);		
			count++;
		}
		spin_unlock_irq(&list_lock);

		fs = get_fs();
		set_fs(KERNEL_DS);
		vfs_write(fp, out_buff, strlen(out_buff), &pos);
		set_fs(fs);
		filp_close(fp, 0);
		kfree(out_buff);

		if ((buff_size - count * 40) / 40 <= 6) buff_size += 1000;
		msleep(10000);
	}
	return 0;
}


static unsigned int Package_catch(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct iphdr *ip_header = (struct iphdr *)skb_network_header(skb); 
	unsigned char *s, *d;

	s = (unsigned char *)&(ip_header->saddr);
	d = (unsigned char *)&(ip_header->daddr);

	insert_and_count(s, d);
	return NF_ACCEPT;
}

/*
static struct nf_hook_ops nfho_in = {
	.hook = Package_catch,
	.hooknum = NF_INET_LOCAL_IN,
	.pf = PF_INET,
	.priority = NF_IP_PRI_FIRST
};

static struct nf_hook_ops nfho_out = {
	.hook = Package_catch,
	.hooknum = NF_INET_LOCAL_OUT,
	.pf = PF_INET,
	.priority = NF_IP_PRI_FIRST
};
*/

static struct nf_hook_ops nfho_router = {
	.hook = Package_catch,
	.hooknum = NF_INET_PRE_ROUTING,
	.pf = PF_INET,
	.priority = NF_IP_PRI_FIRST
};

int init(void){

	//nf_register_net_hook(&init_net, &nfho_in);
	//nf_register_net_hook(&init_net, &nfho_out);
	nf_register_net_hook(&init_net, &nfho_router);

	task = kthread_run(printer_ip_node, NULL, "mythread%d", 1);
	if (IS_ERR(task)) {
		printk(KERN_INFO "create kthread failed!\n");
	}
	else {
		printk(KERN_INFO "create ktrhead ok!\n");
	}


	return 0;
}

void cleanup(void){

	//nf_unregister_net_hook(&init_net, &nfho_in);
	//nf_unregister_net_hook(&init_net, &nfho_out);
	nf_unregister_net_hook(&init_net, &nfho_router);
	if (!IS_ERR(task)) {
		kthread_stop(task);
	}
}

module_init(init);
module_exit(cleanup);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Catch NetWork Package Module");
MODULE_AUTHOR("SGG");