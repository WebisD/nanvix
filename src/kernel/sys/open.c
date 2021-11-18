/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/const.h>
#include <nanvix/dev.h>
#include <nanvix/fs.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <nanvix/klib.h>

/*
 * Returns access permissions.
 */
#define PERM(o)                                        \
	((ACCMODE(o) == O_RDWR) ? (MAY_READ | MAY_WRITE) : \
	((ACCMODE(o) == O_WRONLY) ? MAY_WRITE :            \
	(MAY_READ | ((o & O_TRUNC) ? MAY_WRITE : 0))))     \

/*
 * Creates a file.
 */
PRIVATE struct inode *do_creat(struct inode *d, const char *name, mode_t mode, int oflag)
{
	kprintf("- Criar arquivo");
	struct inode *i;
	
	/* Not asked to create file. */
	if (!(oflag & O_CREAT))
	{
		kprintf("- oi1");
		curr_proc->errno = -ENOENT;
		return (NULL);
	}

	kprintf("- Pediu para criar");
		
	/* Not allowed to write in parent directory. */
	if (!permission(d->mode, d->uid, d->gid, curr_proc, MAY_WRITE, 0))
	{
		kprintf("- oi2");
		curr_proc->errno = -EACCES;
		return (NULL);
	}	
	
	i = inode_alloc(d->sb);
	
	/* Failed to allocate inode. */
	if (i == NULL)
		return (NULL);
		
	i->mode = (mode & MAY_ALL & ~curr_proc->umask) | S_IFREG;

	/* Failed to add directory entry. */
	if (dir_add(d, i, name))
	{
		kprintf("- oi3");
		inode_put(i);
		return (NULL);
	}
		
	inode_unlock(i);

	kprintf("- Criou algo");

	return (i);
}

/*
 * Creates a Directory.
 */
PRIVATE struct inode *do_creatDir(struct inode *d, const char *name, mode_t mode, int oflag)
{
	kprintf("- Criar PASTA");
	struct inode *i;
	
	/* Not asked to create file. */
	if (!(oflag & O_CREAT))
	{
		kprintf("- oi1");
		curr_proc->errno = -ENOENT;
		return (NULL);
	}

	kprintf("- Pediu para criar");
		
	/* Not allowed to write in parent directory. */
	if (!permission(d->mode, d->uid, d->gid, curr_proc, MAY_WRITE, 0))
	{
		kprintf("- oi2");
		curr_proc->errno = -EACCES;
		return (NULL);
	}	
	
	i = inode_alloc(d->sb);
	
	/* Failed to allocate inode. */
	if (i == NULL)
		return (NULL);
		
	i->mode = (mode & MAY_ALL) | S_IFDIR;

	/* Failed to add directory entry. */
	if (dir_add(d, i, name))
	{
		kprintf("- oi3");
		inode_put(i);
		return (NULL);
	}
		
	inode_unlock(i);

	kprintf("- Criou algo");

	return (i);
}


/*
 * Opens a file.
 */
PRIVATE struct inode *do_openDir(const char *path, int oflag, mode_t mode)
{
	const char *name;     /* File name.           */
	struct inode *dinode; /* Directory's inode.   */
	ino_t num;            /* File's inode number. */
	dev_t dev;            /* File's device.       */
	struct inode *i;      /* File's inode.        */
	
	dinode = inode_dname(path, &name);
	
	/* Failed to get directory. */
	if (dinode == NULL)
		return (NULL);
	
	num = dir_search(dinode, name);
	
	/* File does not exist. */
	if (num == INODE_NULL)
	{
		kprintf("- oi0");
		i = do_creatDir(dinode, name, mode, oflag);
		
		/* Failed to create inode. */
		if (i == NULL)
		{
			kprintf("- oi4");
			inode_put(dinode);
			return (NULL);
		}
		
		inode_put(dinode);
		kprintf("- Safe do safe");
		return (i);
	}
	
	dev = dinode->dev;
	inode_put(dinode);
	
	/* File already exists. */

	kprintf("- flag 1");

	i = inode_get(dev, num);
	
	/* Failed to get inode. */
	if (i == NULL)
		return (NULL);
	
	kprintf("- flag 2");

	/* Not allowed. */
	if (!permission(i->mode, i->uid, i->gid, curr_proc, PERM(oflag), 0))
	{
		curr_proc->errno = -EACCES;
		goto error2;
	}

	kprintf("- flag 3");
	
	/* Directory. */
	if (S_ISDIR(i->mode))
	{
		kprintf("- Mas deveria ser aqui");
		/* Directories are not writable. */
		if (ACCMODE(oflag) != O_RDONLY)
		{
			curr_proc->errno = -EISDIR;
			goto error2;
		}
	}

	kprintf("- Finalizando");
	
	inode_unlock(i);
	
	return (i);

error2:
	inode_put(i);
	return (NULL);
	
}


/*
 * Opens a file.
 */
PRIVATE struct inode *do_open(const char *path, int oflag, mode_t mode)
{
	int err;              /* Error?               */
	const char *name;     /* File name.           */
	struct inode *dinode; /* Directory's inode.   */
	ino_t num;            /* File's inode number. */
	dev_t dev;            /* File's device.       */
	struct inode *i;      /* File's inode.        */
	
	dinode = inode_dname(path, &name);
	
	/* Failed to get directory. */
	if (dinode == NULL)
		return (NULL);
	
	num = dir_search(dinode, name);
	kprintf("Motivo disso ir bbbbbbb: %s - %d", path, oflag);
	
	/* File does not exist. */
	if (num == INODE_NULL)
	{
		kprintf("- oi0");
		i = do_creat(dinode, name, mode, oflag);
		kprintf("Motivo disso nao ir aaaaaa: %s - %d", name, oflag);
		
		/* Failed to create inode. */
		if (i == NULL)
		{
			kprintf("- oi4");
			inode_put(dinode);
			return (NULL);
		}
		
		inode_put(dinode);
		kprintf("- Safe do safe");
		return (i);
	}
	
	dev = dinode->dev;
	inode_put(dinode);
	
	/* File already exists. */
	if (oflag & O_EXCL)
	{
		curr_proc->errno = -EEXIST;
		return (NULL);
	}

	kprintf("- flag 1");

	i = inode_get(dev, num);
	
	/* Failed to get inode. */
	if (i == NULL)
		return (NULL);
	
	kprintf("- flag 2");

	/* Not allowed. */
	if (!permission(i->mode, i->uid, i->gid, curr_proc, PERM(oflag), 0))
	{
		curr_proc->errno = -EACCES;
		goto error;
	}

	kprintf("- flag 3");
	
	/* Character special file. */
	if (S_ISCHR(i->mode))
	{
		err = cdev_open(i->blocks[0]);
		
		/* Failed to open character device. */
		if (err)
		{
			curr_proc->errno = err;
			goto error;
		}
	}

	/* Block special file. */
	else if (S_ISBLK(i->mode))
	{
		kprintf("- flag 4");
		/* TODO: open device? */
	}
	
	/* Pipe file. */
	else if (S_ISFIFO(i->mode))
	{
		kprintf("- flag 5");
		curr_proc->errno = -ENOTSUP;
		goto error;
	}
	
	/* Regular file. */
	else if (S_ISREG(i->mode))
	{
		kprintf("- Sipá entra aqui");
		/* Truncate file. */
		if (oflag & O_TRUNC)
			inode_truncate(i);
	}
	
	/* Directory. */
	else if (S_ISDIR(i->mode))
	{
		kprintf("- Mas deveria ser aqui");
		/* Directories are not writable. */
		if (ACCMODE(oflag) != O_RDONLY)
		{
			curr_proc->errno = -EISDIR;
			goto error;
		}
	}

	kprintf("- Finalizando");
	
	inode_unlock(i);
	
	return (i);

error:
	inode_put(i);
	return (NULL);
	
}

/*
 * Opens a file.
 */
PUBLIC int sys_open(const char *path, int oflag, mode_t mode)
{

	kprintf("%d %d %d", oflag, oflag & O_CREAT, oflag & O_CREATD);
	
	if (oflag == O_CREATD){
		kprintf("Estou indo criar uma pasta");
		struct inode *i;  /* Underlying inode. */
		char *name;       /* Path name.        */

		if ((name = getname(path)) == NULL)
			return (curr_proc->errno);
			
		if ((i = do_openDir(path, oflag, mode)) == NULL)
		{
			return (curr_proc->errno);
		}
		kprintf("Terminei");
		return i->mode;
	}
	else{
		kprintf("AAAAAAAAAAAAAA");
		int fd;           /* File descriptor.  */
		struct file *f;   /* File.             */
		struct inode *i;  /* Underlying inode. */
		char *name;       /* Path name.        */
		
		/* Fetch path from user address space. */
		if ((name = getname(path)) == NULL)
			return (curr_proc->errno);
		
		fd = getfildes();
		
		/* Too many opened files. */
		if (fd >= OPEN_MAX)
		{
			putname(name);
			return (-EMFILE);
		}
		
		f = getfile();
		
		/* Too many files open in the system. */
		if (f == NULL)
		{
			putname(name);
			return (-ENFILE);
		}
		
		/* Increment reference count before actually opening
		* the file because we can sleep below and another process
		* may want to use this file table entry also.  */	
		f->count = 1;	
		
		/* Open file. */
		if ((i = do_open(name, oflag, mode)) == NULL)
		{
			putname(name);
			f->count = 0;
			return (curr_proc->errno);
		}
		kprintf("Inicializa arquivo");
		
		/* Initialize file. */
		f->oflag = oflag;
		f->pos = 0;
		f->inode = i;
		
		curr_proc->ofiles[fd] = f;
		curr_proc->close &= ~(1 << fd);

		putname(name);
		
		return (fd);
	}

	kprintf("%d %d", oflag, O_CREATD);
	/*else if (!(oflag & O_CREATD)){
		kprintf("BBBBBBBBBBBBBBBBBBBB");
		return (-1);
	}
	else{
		kprintf("CCCCCCCCCCCCCCCCCCCC");
		return (-1);
	}*/
	return (-1);
	
}
