/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_handlers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrameau <jrameau@student.42.us.org>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/03/27 13:40:08 by jrameau           #+#    #+#             */
/*   Updated: 2017/03/27 16:20:59 by jrameau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"

void reverse_files(t_files **files)
{
  t_files *curr;
  t_files *next;
  t_files *prev;

  prev = NULL;
  curr = *files;
  while (curr)
  {
    next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
  }
  *files = prev;
}

void add_date(t_date *date, struct stat f) {
  char buff[200];

  strftime(buff, 200, "%b", localtime(&(f.st_mtime)));
  MEMCHECK((date->month = ft_strdup(buff)));
  strftime(buff, 200, "%-d", localtime(&(f.st_mtime)));
  MEMCHECK((date->day = ft_strdup(buff)));
  strftime(buff, 200, "%H", localtime(&(f.st_mtime)));
  MEMCHECK((date->hour = ft_strdup(buff)));
  strftime(buff, 200, "%M", localtime(&(f.st_mtime)));
  MEMCHECK((date->minute = ft_strdup(buff)));
  date->unix_format = (unsigned long long)f.st_mtime;
}

void get_file_info(t_files **curr_file, t_dirs **dirs, char *file_name, struct stat f)
{
  format_handler(dirs, f);
  (*curr_file)->modes[0] = (S_ISDIR(f.st_mode)) ? 'd' : '-';
  (*curr_file)->modes[1] = (f.st_mode & S_IRUSR) ? 'r' : '-';
  (*curr_file)->modes[2] = (f.st_mode & S_IWUSR) ? 'w' : '-';
  (*curr_file)->modes[3] = (f.st_mode & S_IXUSR) ? 'x' : '-';
  (*curr_file)->modes[4] = (f.st_mode & S_IRGRP) ? 'r' : '-';
  (*curr_file)->modes[5] = (f.st_mode & S_IWGRP) ? 'w' : '-';
  (*curr_file)->modes[6] = (f.st_mode & S_IXGRP) ? 'x' : '-';
  (*curr_file)->modes[7] = (f.st_mode & S_IROTH) ? 'r' : '-';
  (*curr_file)->modes[8] = (f.st_mode & S_IWOTH) ? 'w' : '-';
  (*curr_file)->modes[9] = (f.st_mode & S_IXOTH) ? 'x' : '-';
  (*curr_file)->link = f.st_nlink;
  (*curr_file)->owner = ft_strdup(getpwuid(f.st_uid)->pw_name);
  (*curr_file)->group = ft_strdup(getgrgid(f.st_gid)->gr_name);
  (*curr_file)->size = f.st_size;
  add_date(&((*curr_file)->date), f);
  MEMCHECK(((*curr_file)->name = ft_strdup(file_name)));
}

void add_file(t_files **curr_file, t_dirs **dirs, t_flags flags)
{
  struct stat f;
  char *dir_name;

  dir_name = (*dirs)->name;
  if (lstat(!dir_name ? (*curr_file)->name : ft_pathjoin(dir_name, (*curr_file)->name), &f) < 0 ||
  !((*curr_file)->modes = ft_strnew(10)))
    exit(2);
  get_file_info(curr_file, dirs, (*curr_file)->name, f);
  if (S_ISDIR(f.st_mode) && (flags & RECURSIVE_FLAG))
    set_dir(ft_pathjoin(dir_name, (*curr_file)->name), &((*dirs)->sub_dirs));
}

t_files *file_handler(t_dirs *dirs, t_flags flags) {
  DIR   *dir;
  struct dirent *sd;
  t_files *files;
  t_files **tmp;

  if (!(dir = opendir(dirs->name)))
  {
    dirs->is_unreadable = 1;
    return (NULL);
  }
  files = NULL;
  tmp = &files;
  while ((sd = readdir(dir)))
  {
    if (!(flags & ALL_FLAG) && sd->d_name[0] == '.')
      continue ;
    MEMCHECK(((*tmp = (t_files *)ft_memalloc(sizeof(t_files)))));
    (*tmp)->name = sd->d_name;
    add_file(tmp, &dirs, flags);
    tmp = &((*tmp)->next);
  }
  closedir(dir);
  file_sort(&files, flags);
  return (files);
}
