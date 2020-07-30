/*
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
██ █████▄██▀▄▀█ ▄▄█ ▄▄▀█ ▄▄█ ▄▄██
██ █████ ▄█ █▀█ ▄▄█ ██ █▄▄▀█ ▄▄██
██ ▀▀ █▄▄▄██▄██▄▄▄█▄██▄█▄▄▄█▄▄▄██
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀

MIT License

Copyright (c) 2020 Oriol Ornaque Blázquez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


/*
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
█▄ ▄█ ▄▄▀█▀▄▀█ ██ ██ █ ▄▀█ ▄▄█ ▄▄██
██ ██ ██ █ █▀█ ██ ██ █ █ █ ▄▄█▄▄▀██
█▀ ▀█▄██▄██▄██▄▄██▄▄▄█▄▄██▄▄▄█▄▄▄██
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

#include <curses.h>


/*
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
██ ▄▄▀█ ▄▄█ ▄▄██▄██ ▄▄▀██▄██▄ ▄██▄██▀▄▄▀█ ▄▄▀█ ▄▄██
██ ██ █ ▄▄█ ▄███ ▄█ ██ ██ ▄██ ███ ▄█ ██ █ ██ █▄▄▀██
██ ▀▀ █▄▄▄█▄███▄▄▄█▄██▄█▄▄▄██▄██▄▄▄██▄▄██▄██▄█▄▄▄██
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
*/

#ifndef SIGWINCH
	#define SIGWINCH 28
#endif /* SIGWINCH */

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))


/*
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
██ ▄▄▄█ ▄▄▀█ ██ █ ▄▀▄ █ ▄▄██
██ ▄▄▄█ ██ █ ██ █ █▄█ █▄▄▀██
██ ▀▀▀█▄██▄██▄▄▄█▄███▄█▄▄▄██
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
*/

enum ACTIONS
{
	QUIT,
	QUIT_NO_CD,
	MOVE_LEFT,
	MOVE_DOWN,
	MOVE_UP,
	MOVE_RIGHT,
	TOGGLE_SELECTION,
	TOGGLE_HIDDEN,
	RELOAD_REDRAW,
	JUMP_TO_FIRST,
	JUMP_TO_LAST,
	NEW_FILE,
	DELETE_FILE,
	ACTION_UNKNOWN,
};


/*
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
██ ▄▄▄ █▄ ▄█ ▄▄▀█ ██ █▀▄▀█▄ ▄█ ▄▄██
██▄▄▄▀▀██ ██ ▀▀▄█ ██ █ █▀██ ██▄▄▀██
██ ▀▀▀ ██▄██▄█▄▄██▄▄▄██▄███▄██▄▄▄██
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
*/

struct entry
{
	char* name;
	unsigned int is_dir:1;
	unsigned int is_selected:1;
};

struct entries
{
	struct entry* list;
	ssize_t size;
	size_t capacity;
	ssize_t index;
	size_t display_start_index;
	size_t display_end_index;
};

struct screen
{
	WINDOW* w;
	size_t rows;
	size_t cols;
	bool winch_received;
};


/*
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
██ ▄▄▄█ ██ █ ▄▄▀█▀▄▀█▄ ▄██▄██▀▄▄▀█ ▄▄▀█ ▄▄██
██ ▄▄██ ██ █ ██ █ █▀██ ███ ▄█ ██ █ ██ █▄▄▀██
██ █████▄▄▄█▄██▄██▄███▄██▄▄▄██▄▄██▄██▄█▄▄▄██
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
*/

void entries_new(void);
void entries_fill(char* path);
void entries_free(void);

void init_curses(void);
void handler_winch(int sig);
void draw(void);
void draw_command(const char* prompt, char* buffer);
enum ACTIONS get_next_action(void);

char* read_command_input(const char* prompt);
void create_file(const char* name, const bool is_dir);
void delete_all_selected_files(void);

void get_parent_dir(void);
void get_child_path(void);
bool path_is_dir(const char* path);
char* path_concat(char* base, char* appended);


/*
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
██ ▄▄ █ ██▀▄▄▀█ ▄▄▀█ ▄▄▀█ ████▀███▀█ ▄▄▀█ ▄▄▀██▄██ ▄▄▀█ ▄▄▀█ ██ ▄▄█ ▄▄██
██ █▀▀█ ██ ██ █ ▄▄▀█ ▀▀ █ █████ ▀ ██ ▀▀ █ ▀▀▄██ ▄█ ▀▀ █ ▄▄▀█ ██ ▄▄█▄▄▀██
██ ▀▀▄█▄▄██▄▄██▄▄▄▄█▄██▄█▄▄█████▄███▄██▄█▄█▄▄█▄▄▄█▄██▄█▄▄▄▄█▄▄█▄▄▄█▄▄▄██
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
*/

struct entries entries;
struct screen screen;
bool run;
bool show_hidden;
bool cd_on_exit;
char* cwd;


/*
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
████ ▄▀▄ █ ▄▄▀██▄██ ▄▄▀████
████ █ █ █ ▀▀ ██ ▄█ ██ ████
████ ███ █▄██▄█▄▄▄█▄██▄████
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
*/

int main()
{
	FILE* lastd = NULL;
	char* lastd_path = NULL;

	if((lastd_path = getenv("FM_LASTD")) == NULL)
		lastd_path = "/tmp/fm_lastd";

	cd_on_exit = true;
	show_hidden = false;

	cwd = getcwd(cwd, PATH_MAX);

	init_curses();

	entries_new();
	entries_fill(cwd);

	draw();

	run = true;
	while(run)
	{
		switch(get_next_action())
		{
			case QUIT:
				run = false;
				cd_on_exit = true;
				break;
			case QUIT_NO_CD:
				run = false;
				cd_on_exit = false;
				break;
			case MOVE_LEFT:
				get_parent_dir();
				entries_fill(cwd);
				draw();
				break;
			case MOVE_DOWN:
				if((entries.index + 1) < entries.size)
				{
					++(entries.index);
					if(entries.index >= entries.display_end_index)
					{
						++(entries.display_start_index);
						++(entries.display_end_index);
					}
					draw();
				}
				break;
			case MOVE_UP:
				if((entries.index - 1) >= 0)
				{
					--(entries.index);
					if(entries.index < entries.display_start_index)
					{
						--(entries.display_start_index);
						--(entries.display_end_index);
					}
					draw();
				}
				break;
			case MOVE_RIGHT:
				if(entries.list[entries.index].is_dir)
				{
					get_child_path();
					entries_fill(cwd);
					draw();
				}
				break;
			case TOGGLE_SELECTION:
				entries.list[entries.index].is_selected ^= 1;
				draw();
				break;
			case TOGGLE_HIDDEN:
				show_hidden ^= 1;
				entries_fill(cwd);
				draw();
				break;
			case RELOAD_REDRAW:
				entries_fill(cwd);
				draw();
				break;
			case JUMP_TO_LAST:
				entries.index = entries.size - 1;
				entries.display_end_index = entries.size;
				if(entries.size < screen.rows - 1)
					entries.display_start_index = 0;
				else
					entries.display_start_index = entries.size - screen.rows + 1;
				draw();
				break;
			case JUMP_TO_FIRST:
				entries.index = 0;
				entries.display_start_index = 0;
				entries.display_end_index = MIN(entries.size, screen.rows-1);
				draw();
				break;
			case NEW_FILE:
			{
				char* buffer = NULL;
				bool cancel_operation = false;

				buffer = read_command_input("new");
				cancel_operation = (buffer == NULL);

				if(!cancel_operation)
				{
					bool is_dir = path_is_dir(buffer);

					create_file(buffer, is_dir);

					entries_fill(cwd);
				}

				free(buffer);

				draw();
				break;
			}
			case DELETE_FILE:
				delete_all_selected_files();
				entries_fill(cwd);
				draw();
				break;
			default:
				break;
		}
	}

	endwin();

	entries_free();

	if(cd_on_exit)
	{
		lastd = fopen(lastd_path, "w");
		fprintf(lastd, "%s", cwd);
		fclose(lastd);
	}

	free(cwd);

	return 0;
}


/*
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
█▄ ▄█ ▄▀▄ █▀▄▄▀█ ██ ▄▄█ ▄▀▄ █ ▄▄█ ▄▄▀█▄ ▄█ ▄▄▀█▄ ▄██▄██▀▄▄▀█ ▄▄▀█ ▄▄██
██ ██ █▄█ █ ▀▀ █ ██ ▄▄█ █▄█ █ ▄▄█ ██ ██ ██ ▀▀ ██ ███ ▄█ ██ █ ██ █▄▄▀██
█▀ ▀█▄███▄█ ████▄▄█▄▄▄█▄███▄█▄▄▄█▄██▄██▄██▄██▄██▄██▄▄▄██▄▄██▄██▄█▄▄▄██
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
*/

void entries_new(void)
{
	entries.capacity = 32;
	entries.size = 0;
	entries.index = 0;
	entries.list = malloc(sizeof(struct entry) * entries.capacity);
	memset(entries.list, 0x0, sizeof(struct entry) * entries.capacity);
}

void entries_fill(char* path)
{
	if(path == NULL)
		return;

	DIR* dir_ptr = NULL;
	struct dirent* dir_entry = NULL;
	int dir_fd;
	unsigned int index = 0;

	if(access(path, R_OK) == -1)
		return;	/* ERROR_NO_PERMISSIONS */

	if((dir_ptr = opendir(path)) == NULL)
		return;	/* ERROR_OPEN_DIR */

	dir_fd = dirfd(dir_ptr);

	while((dir_entry = readdir(dir_ptr)) != NULL)
	{
		if(dir_entry->d_name[0] == '.' && !show_hidden)	
			continue;

		if(index >= entries.capacity)
		{	/* increase array */
			entries.capacity += 32;
			entries.list = reallocarray(entries.list, entries.capacity, sizeof(struct entry));
		}

		struct entry* e = &(entries.list[index]);

		if(e->name != NULL)
			free(e->name);

		e->name = strndup(dir_entry->d_name, PATH_MAX);

		struct stat info;
		fstatat(dir_fd, dir_entry->d_name, &info, AT_SYMLINK_NOFOLLOW);
		e->is_dir = S_ISDIR(info.st_mode);
		e->is_selected = 0;

		++index;
	}

	closedir(dir_ptr);

	entries.size = index;
	entries.index = 0;
	entries.display_start_index = 0;
	entries.display_end_index = screen.rows - 1;
}

void entries_free(void)
{
	struct entry* e = NULL;

	for(int i = 0; i < entries.capacity; ++i)
	{
		e = &(entries.list[i]);
		free(e->name);
	}

	free(entries.list);
	entries.size = 0;
	entries.capacity = 0;
}

void init_curses(void)
{
	screen.w = initscr();
	atexit((void (*)(void)) endwin);

	getmaxyx(screen.w, screen.rows, screen.cols);

	cbreak();
	noecho();
	nonl();

	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	curs_set(FALSE);

	start_color();
	use_default_colors();

	timeout(1000); /* 1 sec */
	set_escdelay(25);

	struct sigaction signal_handler;
	memset(&signal_handler, 0, sizeof(struct sigaction));
	signal_handler.sa_handler = handler_winch;
	sigaction(SIGWINCH, &signal_handler, NULL);
}

void handler_winch(int sig)
{
	screen.winch_received = TRUE;
	getmaxyx(screen.w, screen.rows, screen.cols);
	draw();
}

void draw(void)
{
	erase(); /* clear screen */
	struct entry* e = NULL;

	printw("cwd: %s\t%ix%i\t%i/%i\n", cwd, screen.cols, screen.rows, entries.index+1, MAX(entries.size, 0));

	for(int i = entries.display_start_index; i < MIN(entries.size, entries.display_end_index); ++i)
	{
		bool on_cursor = (i == entries.index);
		e = &(entries.list[i]);

		if(on_cursor)
			attron(A_REVERSE);

		if(e->is_selected)
			attron(A_UNDERLINE);

		printw("%4i", i);

		if(e->is_dir)
			printw("%4s", " [D]");
		else
			printw("    ");

		printw("%s\n", e->name);

		if(e->is_selected)
			attroff(A_UNDERLINE);

		if(on_cursor)
			attroff(A_REVERSE);
	}
}

void draw_command(const char* prompt, char* buffer)
{
	if(buffer == NULL)
		return;

	mvprintw(0, 0, "%s: %s\n", prompt, buffer);
}

enum ACTIONS get_next_action(void)
{
	char input_key;
	input_key = getch();

	switch(input_key)
	{
		case 'q':
			return QUIT; break;
		case 'h':
			return MOVE_LEFT; break;
		case 'j':
			return MOVE_DOWN; break;
		case 'k':
			return MOVE_UP; break;
		case 'l':
			return MOVE_RIGHT; break;
		case ' ':
			return TOGGLE_SELECTION; break;
		case '.':
			return TOGGLE_HIDDEN; break;
		case 'r':
			return RELOAD_REDRAW; break;
		case 0x1b:	/* ESC KEY */
			return QUIT_NO_CD; break;
		case 'G':
			return JUMP_TO_FIRST; break;
		case 'g':
			return JUMP_TO_LAST; break;
		case 'n':
			return NEW_FILE; break; /* new file or directory */
		case 'd':
			return DELETE_FILE; break; /* delete file or directory */
		/*
		 *	c - copy
		 *	m - move
		 *	p - paste
		 *	: - command
		 *	i - toggle verbose
		 *	v - toggle verbose
		 */
		default:
			return ACTION_UNKNOWN; break;
	}
}

char* read_command_input(const char* prompt)
{
	char* buffer = NULL;
	size_t buffer_size = sizeof(char) * PATH_MAX;
	size_t index_buffer = 0;
	int input_key;
	bool expect_more_input = true;
	bool cancel_operation = false;

	if((buffer = malloc(buffer_size)) == NULL)
		return NULL;
	memset(buffer, 0x0, buffer_size);

	timeout(-1);

	draw_command(prompt, buffer);

	while(expect_more_input)
	{
		input_key = getch();

		switch(input_key)
		{
			case KEY_ENTER:
			case '\n':
			case '\r':
				expect_more_input = false;
				break;
			case 0x1b: /* ESC */
				expect_more_input = false;
				cancel_operation = true;
				break;
			case 0x7f: /* fallthrough */
			case 0x8: /* backspace */
				if(index_buffer > 0)
					buffer[--index_buffer] = 0x0;
				draw_command(prompt, buffer);
				break;
			case '\t':
				break;
			default:
				if(index_buffer < buffer_size)
					buffer[index_buffer++] = input_key;
				draw_command(prompt, buffer);
		}
	}

	timeout(1000);

	if(cancel_operation)
		return NULL;

	return buffer;
}

void create_file(const char* name, const bool is_dir)
{
	DIR* dir_ptr = opendir(cwd);
	int cwd_fd = dirfd(dir_ptr);
	mode_t mode;
	
	if(is_dir)
		mkdirat(cwd_fd, name, 0755);
	else
	{
		int fd = openat(cwd_fd, name, O_RDWR|O_CREAT|O_CLOEXEC, 0644);
		close(fd);
	}

	closedir(dir_ptr);
}

void delete_all_selected_files(void)
{
	timeout(-1);

	for(size_t it = 0; it < entries.size; ++it)
	{
		struct entry* e = NULL;

		e = &(entries.list[it]);

		if(e->is_selected)
		{
			char* path = path_concat(cwd, e->name);
			if(path == NULL)
				continue;

			remove(path);
		}
	}

	timeout(1000);
}

void get_parent_dir(void)
{
	char* ptr;
	ptr = strrchr(cwd, '/');
	if(ptr != NULL)
		*ptr = '\0';
}

void get_child_path(void)
{
	struct entry* e = NULL;
	char* file = NULL;

	e = &(entries.list[entries.index]);
	file = e->name;

	char* new_cwd = path_concat(cwd, file);
	if(new_cwd == NULL)
		return;

	free(cwd);

	cwd = new_cwd;
}

bool path_is_dir(const char* path)
{
	if(path == NULL)
		return false;

	if(path[strlen(path)-1] == '/')
		return true;

	return false;
}

char* path_concat(char* base, char* appended)
{
	if(base == NULL || appended == NULL)
		return NULL;

	size_t length = strlen(base) + sizeof(char) + strlen(appended) + 1;
	char* new_path = NULL;

	if((new_path = malloc(sizeof(char) * length)) == NULL)
		return NULL;
	memset(new_path, 0x0, sizeof(char) * length);

	strncat(new_path, base, length - 1);
	strncat(new_path, "/", length - 1);
	strncat(new_path, appended, length - 1);

	return new_path;
}

