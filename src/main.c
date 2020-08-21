#include "cpuUsage.h"
#include <gtk/gtk.h>
#include "listOfProcess.h"
typedef struct 
{
  GtkListStore * str;
  GtkTreeIter iter;
}tree_widgets;  //stuct that mantains info of the list while running

gboolean calculatetemp(tree_widgets * ptr);

int main (int argc, char *argv[]){
    GtkWidget *main_window;
    GtkBuilder *builder;
    GtkTreeView *tv1;
    GtkTreeSelection *sel;
    tree_widgets * widgets = g_slice_new(tree_widgets); //allocating memory

    gtk_init(&argc,&argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder,"glade/ui.glade",NULL);   //accessing the glade file to build the ui

    main_window = GTK_WIDGET(gtk_builder_get_object(builder,"main_window"));
    tv1 = GTK_TREE_VIEW(gtk_builder_get_object(builder,"tv"));
    widgets->str = GTK_LIST_STORE(gtk_builder_get_object(builder,"str"));
    sel = gtk_tree_view_get_selection(tv1);

    gtk_builder_connect_signals(builder,widgets);
    g_timeout_add_seconds(1,(GSourceFunc)calculatetemp,widgets);    //calling this func every second

    g_object_unref(builder);
    gtk_widget_show(main_window);
    gtk_main();
    g_slice_free(tree_widgets,widgets);
    return EXIT_SUCCESS;
}

gboolean calculatetemp(tree_widgets * ptr){
    gtk_list_store_move_after(ptr->str,&ptr->iter,NULL);
    gtk_list_store_clear(ptr->str);
    calculate();
    Node * temp = getHead();
    while (temp!=NULL)
    {
        gtk_list_store_append(ptr->str,&ptr->iter);
        gtk_list_store_set(ptr->str,&ptr->iter,0,temp->value.comm,1,temp->value.pid,2,temp->value.perc);
        temp = temp->next;
    }
    return TRUE;
}