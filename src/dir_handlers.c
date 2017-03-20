#include "ft_ls.h"

t_dirs *new_dir(char *name, int status, int is_default, t_dirs *head, t_dirs *tail)
{
  t_dirs *dir;

  MEMCHECK((dir = (t_dirs *)ft_memalloc(sizeof(*dir))));
  MEMCHECK((dir->name = ft_strdup(name)));
  dir->status = status;
  dir->format.link = 0;
  dir->format.owner = 0;
  dir->format.group = 0;
  dir->format.fileSize = 0;
  dir->format.date_month = 3;
  dir->format.date_day = 2;
  dir->format.date_hour = 2;
  dir->format.date_minute = 2;
  dir->next = head;
  dir->prev = tail;
  dir->date.unix = get_dir_date(dir->name);
  dir->is_default = is_default;
  dir->is_unreadable = 0;
  if (tail)
    tail->is_last_dir = 0;
  dir->is_last_dir = 1;
  return (dir);
}

void add_dir(t_dirs **dirs, t_dirs *new) {
	t_dirs *tmp;
	t_dirs *head;

	tmp = *dirs;
	head = tmp;
  while (tmp && !tmp->is_last_dir)
  		tmp = tmp->next;
	tmp->next = new;
	*dirs = head;
}

int dir_cmp(const void *a, const void *b)
{
    char * const *first = a;
    char * const *second = b;
    return (ft_strcmp(*first, *second));
}

t_dirs *set_dir(char *arg, t_dirs **dirs, t_dirs *head, t_dirs *tail) {
  t_dirs *new;
  int status;
  struct stat s;

  status = IS_DIR;
  if (lstat(arg, &s) == -1)
  {
    if (ENOENT == errno)
      status = IS_NONEXISTENT;
  }
  else
  {
    if (!S_ISDIR(s.st_mode))
      status = IS_NOTDIR;
  }
  new = new_dir(arg, status, 0, head, tail);
  if (!*dirs || (*dirs)->is_default)
    *dirs = new;
  else
    add_dir(dirs, new);
  return (*dirs);
}

t_dirs *dir_handler(char **args, int num_args, t_flags flags) {
  int i;
  t_dirs *dirs;
  t_dirs *tail;
  t_dirs *head;

  i = -1;
  dirs = new_dir(".", IS_DIR, 1, NULL, NULL);
  if (!(flags & NEWEST_FIRST_SORT_FLAG))
    qsort(args, num_args, sizeof(char *), &dir_cmp);
  tail = NULL;
  head = dirs;
  while (args[++i])
      tail = set_dir(args[i], &dirs, head, tail);
  if (flags & NEWEST_FIRST_SORT_FLAG)
    dir_sort(&dirs);
  return (dirs);
}
