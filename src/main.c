#include <gtk/gtk.h>
#include "listOfProcess.h"
#include <sys/sysinfo.h>
#include <cairo/cairo.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH   640
#define HEIGHT  480

#define ZOOM_X  10.0
#define ZOOM_Y  10.0

static int cpus;
typedef struct 
{
  GtkListStore * str;
  GtkTreeIter iter;
  GtkDrawingArea * da;
}tree_widgets;  //stuct that mantains info of the list while running

typedef struct graph_node{
    float x, y;
    double r,g,b;
    struct graph_node * next;
}GraphNode;

GraphNode **percs; //array of pointers containing all the values in the graph // graph related to cpus values

void addGraphNode(GraphNode ** head, float y){
    GraphNode * add = (GraphNode *) malloc(sizeof(GraphNode));
    GraphNode * temp;
    GraphNode * a;
    add->next = NULL;
    add->y=y;
    if(*head == NULL)
        *head = add;
    else if((*head)->x == 40.0){     //clear the memory of the points over the graph
        a = (*head)->next;
        free(*head);
        *head = a;
    }else{
        temp = *head;
        while(temp->next!=NULL){
            temp->x++;
            temp = temp->next;
        }
        temp->x++;
        temp->next = add;
    }
    add->x = 1;
}

static gboolean
on_draw (GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    GdkRectangle da;            /* GtkDrawingArea size */
    gdouble dx, dy; /* Pixels between each point */
    gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
    GdkWindow *window = gtk_widget_get_window(widget);
    /* Determine GtkDrawingArea dimensions */
    gdk_window_get_geometry (window,
            &da.x,
            &da.y,
            &da.width,
            &da.height);

    dx = da.width/20;   //there will be 20 points in the x axis
    dy = da.height/100; //and 100 points in the y axys
    /* Change the transformation matrix */
    cairo_translate (cr, 0, da.height);
    cairo_scale (cr, ZOOM_X, -ZOOM_Y);

    /* Determine the data points to calculate (ie. those in the clipping zone */
    cairo_device_to_user_distance (cr, &dx, &dy);
    cairo_clip_extents (cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width (cr, 0.5);

    /* Draws x and y axis */
    cairo_set_source_rgb (cr, 0.0, 1.0, 1.0);
    cairo_move_to (cr, clip_x1, 0.0);
    cairo_line_to (cr, clip_x2, 0.0);
    cairo_move_to (cr, 0.0, clip_y1);
    cairo_line_to (cr, 0.0, clip_y2);
    cairo_stroke (cr);
    if( percs[0] == NULL)
      cairo_move_to(cr,0,0);
    else{
      int procs = get_nprocs();
      GraphNode * temp;
      for(int i =0; i < procs+1; i++){  //draws the lists of
        temp = percs[i];
        cairo_move_to(cr,clip_x2-temp->x*dx,temp->y*100*dy*-1);     //multiplying for dx and dy to mantain proportion in maximized window
        while(temp!=NULL){
          cairo_line_to(cr,clip_x2-temp->x*dx,temp->y*100*dy*-1);   //-1 because dy is a negative number
          temp = temp->next;
        }
        cairo_set_source_rgba (cr, percs[i]->r, percs[i]->g, percs[i]->b, 0.8);
        cairo_stroke (cr);
      }
    }
    return FALSE;
}

gboolean calculatetemp(tree_widgets * ptr);

void on_main_window_destroy(){
    gtk_main_quit();
}

void on_select_changed(){

}
void on_tv_start_interactive_search(){
    
}

int main (int argc, char *argv[]){
    GtkWidget *main_window;
    GtkBuilder *builder;
    GtkTreeView *tv1;
    GtkTreeSelection *sel;
    GtkTreeViewColumn * cl2;
    tree_widgets * widgets = g_slice_new(tree_widgets); //allocating memory
    GtkBox * box;
    cpus = get_nprocs();
    percs = (GraphNode **) malloc(sizeof(GraphNode **)*(cpus+1));

    for(int i =0; i < cpus+1; i++)
        percs[i] = NULL;

    gtk_init(&argc,&argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder,"glade/ui.glade",NULL);   //accessing the glade file to build the ui

    main_window = GTK_WIDGET(gtk_builder_get_object(builder,"main_window"));
    tv1 = GTK_TREE_VIEW(gtk_builder_get_object(builder,"tv"));
    widgets->str = GTK_LIST_STORE(gtk_builder_get_object(builder,"str"));
    cl2 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder,"cl_perc"));
    sel = gtk_tree_view_get_selection(tv1);
    widgets->da = GTK_DRAWING_AREA(gtk_builder_get_object(builder,"drw_ar"));
    box = GTK_BOX(gtk_builder_get_object(builder,"g_box2"));

    gtk_builder_connect_signals(builder,widgets);
    g_signal_connect(G_OBJECT(widgets->da),
        "draw",
        G_CALLBACK(on_draw),
        NULL);
    g_timeout_add_seconds(2,(GSourceFunc)calculatetemp,widgets);    //calling this func every second

    gtk_list_store_append(widgets->str,&widgets->iter);
    gtk_list_store_set(widgets->str,&widgets->iter,0,"asad",1,"3213",2,0.5,-1);
    gtk_list_store_append(widgets->str,&widgets->iter);
    gtk_list_store_set(widgets->str,&widgets->iter,0,"asad",-1);
    gtk_tree_view_column_clicked(cl2);
    gtk_tree_view_column_clicked(cl2);

    g_object_unref(builder);
    gtk_widget_show(main_window);
    gtk_main();
    g_slice_free(tree_widgets,widgets);
    return EXIT_SUCCESS;
}

gboolean calculatetemp(tree_widgets * ptr){
    int i = 0;
    float cpu_calc[cpus+1];
    gtk_list_store_move_after(ptr->str,&ptr->iter,NULL);
    gtk_list_store_clear(ptr->str);
    calculate(cpu_calc);
    Node * temp = *getHead();
    while (temp!=NULL)
    {
        gtk_list_store_append(ptr->str,&ptr->iter);
        gtk_list_store_set(ptr->str,&ptr->iter,0,temp->value.comm,1,temp->value.pid,2,temp->value.perc*100,-1);
        temp = temp->next;
    }
    for(;i < cpus +1; i++){
        addGraphNode(&percs[i],cpu_calc[i]);
        if(percs[i]->next==NULL){
            percs[i]->r = (float) (rand()%101)/100;
            percs[i]->g = (float) (rand()%101)/100;
            percs[i]->b = (float) (rand()%101)/100;
        }else{
            GraphNode * temp = percs[i];
            while(temp->next!=NULL)
                temp = temp->next;
            temp->r = percs[i]->r;
            temp->g = percs[i]->g;
            temp->b = percs[i]->b;
        }
    }
    gtk_widget_queue_draw(ptr->da);
    freeAll();
    return TRUE;
}