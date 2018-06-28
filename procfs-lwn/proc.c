#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>

static void *my_seq_start(struct seq_file *seq, loff_t *pos)
{
    loff_t *t_pos = vzalloc(sizeof(loff_t));
    if (!t_pos)
        return NULL;           

    *t_pos = *pos;

    return t_pos;
}

static void *my_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
    loff_t *t_pos =  (loff_t *) v;
    *pos = ++(*t_pos);
    return t_pos;
}

static void my_seq_stop(struct seq_file *seq, void *v)
{
    vfree(v);
}

static int my_seq_show(struct seq_file *seq, void *v)
{
    loff_t *t_pos = (loff_t *) v;
    seq_printf(seq, "%Ld\n", *t_pos);

    return 0;
}

static struct seq_operations my_seq_ops = {
        .start = my_seq_start,
        .next  = my_seq_next,
        .stop  = my_seq_stop,
        .show  = my_seq_show,
};

static int mine_open(struct inode *node, struct file *file)
{
    return seq_open(file, &my_seq_ops);
}

static struct file_operations proc_fops = {
        .owner   = THIS_MODULE,
        .open    = mine_open,
        .llseek  = seq_lseek,
        .read    = seq_read,
        .release = seq_release,
};

static int my_init(void)
{
    static struct proc_dir_entry *my_proc;

    my_proc = proc_create("mine", 0, NULL, &proc_fops);

    if (!my_proc)
        printk(KERN_ERR "Failed to register proc entry\n");

    return 0;
}

static void my_exit(void)
{
    remove_proc_entry("mine", NULL);
} 

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Manja");
MODULE_LICENSE("GPL");
