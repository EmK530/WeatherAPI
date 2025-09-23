#ifndef UI_CONSOLE_H
#define UI_CONSOLE_H

#include "app_state.h"

void print_available_locations(location_list *);

void clear_buffer_stdin();

int prompt_user_for_current_selection(app_state *);

void print_current_location(location *);

#endif /* UI_CONSOLE_H */