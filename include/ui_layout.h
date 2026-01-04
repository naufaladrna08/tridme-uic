#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include "ui_core.h"

/*
 * Vertical Box Layout, a Layout that manage widgets by placing them in a columns.
 * 
 * Example of usage:
 *   ui_begin_vbox(ui); <- This will create new vbox layout. You can configure the
 *                         parameters like how you want the widgets to expand, and 
 *                         etc. The default height of a Vbox is adjusted by the max
 *                         height of it's widgets. You can override the height and
 *                         Force the widget to follow the layout's height. 
 *   ... Render some widgets. 
 *   ui_end_vbox();
 */

void ui_begin_vbox(UIContext* ctx);
void ui_end_vbox(UIContext* ctx);



#endif