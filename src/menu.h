#ifndef _MENU_H
#define _MENU_H

#define MENU_ITEMS_PER_PAGE 8

int select_menu(const char *title, const void *data, unsigned int nmemb, unsigned int size, const char *gettext(const void *));
int areyousure_menu(const char *title);
void edit_int(int *val, const char *title);

#endif