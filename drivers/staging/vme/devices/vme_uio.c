#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uio_driver.h>
#include <linux/vme.h>

static void vme_uio_int(int level, int status_id, void *priv)
{
	struct uio_info *info = priv;

	uio_event_notify(info);
}

struct int_sysfs_entry {
	struct kobj_attribute kobj_attr;
	struct vme_dev *vdev;
	struct uio_info uio;
	int level;
	int statid;
	int enabled;
};

static ssize_t int_enabled_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	struct int_sysfs_entry *entry;

	entry = container_of(attr, struct int_sysfs_entry, kobj_attr);
	return sprintf(buf, "%d\n", entry->enabled);
}

static ssize_t int_enabled_store(struct kobject *kobj,
				 struct kobj_attribute *attr, const char *buf,
				 size_t count)
{
	int enabled;
	struct int_sysfs_entry *entry;
	int ret;

	entry = container_of(attr, struct int_sysfs_entry, kobj_attr);

	ret = kstrtoint(buf, 0, &enabled);
	if (ret)
		return ret;
	enabled = !!enabled;

	if (enabled == entry->enabled)
		return count;

	if (enabled) {
		ret = uio_register_device(&entry->vdev->dev, &entry->uio);
		if (ret) {
			enabled = 0;
			return ret;
		}

		ret = vme_irq_request(entry->vdev, entry->level, entry->statid,
				      vme_uio_int, &entry->uio);
		if (ret) {
			enabled = 0;
			return ret;
		}
	} else {
		vme_irq_free(entry->vdev, entry->level, entry->statid);

		uio_unregister_device(&entry->uio);
	}

	entry->enabled = enabled;

	return count;
}

static struct kobj_attribute int_enabled_attribute =
	__ATTR(enabled, 0644, int_enabled_show, int_enabled_store);

static void int_release(struct kobject *kobj)
{
	pr_info("release %s\n", kobj->name);
	kfree(kobj);
};

static struct kobj_type int_attr_type = {
	.release = int_release,
	.sysfs_ops = &kobj_sysfs_ops,
};

static int vme_uio_match(struct vme_dev *vdev)
{
	return 1;
}

static int vme_uio_probe(struct vme_dev *vdev)
{
	int ret, level, statid;

	int bus_num = vme_bus_num(vdev);

	struct kobject *kobj = kobject_create_and_add("irq", &vdev->dev.kobj);

	for (level = 1; level <= 7; level++) {
		char *level_node_name = kasprintf(GFP_KERNEL, "%d", level);
		struct kobject *level_node = kobject_create_and_add(
			level_node_name, kobj);
		if (!level_node)
			return -ENOMEM;

		for (statid = 0; statid < VME_NUM_STATUSID; statid++) {
			char *statid_node_name = kasprintf(GFP_KERNEL,
							   "%02x", statid);
			struct kobject *statid_node;
			struct int_sysfs_entry *entry;

			statid_node = kobject_create_and_add(statid_node_name,
							     level_node);
			if (!statid_node)
				return -ENOMEM;
			statid_node->ktype = &int_attr_type;

			entry = kzalloc(sizeof(*entry), GFP_KERNEL);
			if (!entry)
				return -ENOMEM;
			entry->uio.name = kasprintf(GFP_KERNEL,
						    "vme_irq_%d_%d_%02x",
						    bus_num, level, statid);
			entry->uio.version = "1";
			entry->uio.irq = UIO_IRQ_CUSTOM;
			entry->level = level;
			entry->statid = statid;
			entry->vdev = vdev;
			entry->enabled = 0;
			entry->kobj_attr = int_enabled_attribute;
			ret = sysfs_create_file(statid_node,
						&entry->kobj_attr.attr);
			if (ret)
				return ret;
		}
	}

	return 0;
}

static int vme_uio_remove(struct vme_dev *vdev)
{
	/* XXX Cleanup here */
	return 0;
}

static struct vme_driver vme_uio_driver = {
	.name = "vme_uio",
	.match = vme_uio_match,
	.probe = vme_uio_probe,
	.remove = vme_uio_remove,
};

static int __init vme_uio_init(void)
{
	return vme_register_driver(&vme_uio_driver, 1);
}

static void __exit vme_uio_exit(void)
{
	vme_unregister_driver(&vme_uio_driver);
}

module_init(vme_uio_init);
module_exit(vme_uio_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Dmitry Kalinkin <dmitry.kalinkin@gmail.com>");
