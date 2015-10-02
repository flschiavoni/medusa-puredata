#include "medusa_pd.h"

static t_class *medusa_control_class;

typedef struct _medusa_control {
   t_object x_obj;
   t_outlet *node_list_symbol_outlet;
   t_medusa_control * control;
} t_medusa_pd_control;

void medusa_control_list_nodes(t_medusa_pd_control *x){
   t_medusa_list * list = medusa_control_get_nodes(x->control);
   char msg[1024];
   while(list){
      t_medusa_node * node = (t_medusa_node *) list->data;
      t_medusa_list * resource = medusa_node_get_audio_resources(node);
      while(resource){
         t_medusa_node_audio_resource * audio_resource
               = (t_medusa_node_audio_resource *) resource->data;
         sprintf(msg, "%s@%s", audio_resource->name, medusa_node_get_name(node));
         outlet_symbol(x->node_list_symbol_outlet, gensym(msg));
         resource = resource->next;
      }
      resource = medusa_node_get_midi_resources(node);
      while(resource){
         t_medusa_node_midi_resource * midi_resource
               = (t_medusa_node_midi_resource *) resource->data;
         sprintf(msg, "%s@%s", midi_resource->name, medusa_node_get_name(node));
         outlet_symbol(x->node_list_symbol_outlet, gensym(msg));
         resource = resource->next;
      }
      list = list->next;
   }
}

void medusa_control_add_node(t_medusa_node * node, void * args){
   (void) node;
   t_medusa_pd_control *x = (t_medusa_pd_control *) args;
   medusa_control_list_nodes(x);
}

void medusa_control_remove_node(t_medusa_node * node, void * args){
   (void) node;
   t_medusa_pd_control *x = (t_medusa_pd_control *) args;
   medusa_control_list_nodes(x);
}

void medusa_control_add_node_audio_resource(
      t_medusa_node * node,
      t_medusa_node_audio_resource * audio_resource,
      void * args
      ){
   (void) node;
   (void) audio_resource;
   t_medusa_pd_control *x = (t_medusa_pd_control *) args;
   medusa_control_list_nodes(x);
}

void medusa_control_add_node_midi_resource(
      t_medusa_node * node,
      t_medusa_node_midi_resource * midi_resource,
      void * args
      ){
   (void) node;
   (void) midi_resource;
   t_medusa_pd_control *x = (t_medusa_pd_control *) args;
   medusa_control_list_nodes(x);
}

void medusa_control_remove_node_audio_resource(
      t_medusa_node * node,
      t_medusa_node_audio_resource * audio_resource,
      void * args
      ){
   (void) node;
   (void) audio_resource;
   t_medusa_pd_control *x = (t_medusa_pd_control *) args;
   medusa_control_list_nodes(x);
}

void medusa_control_remove_node_midi_resource(
      t_medusa_node * node,
      t_medusa_node_midi_resource * midi_resource,
      void * args
      ){
   (void) node;
   (void) midi_resource;
   t_medusa_pd_control *x = (t_medusa_pd_control *) args;
   medusa_control_list_nodes(x);
}

// Constructor of the class
void * medusa_control_new(t_symbol * name) {
   t_medusa_pd_control *x = (t_medusa_pd_control *) pd_new(medusa_control_class);

   if(strcmp(name->s_name, "") == 0)
      x->control = medusa_control_create("");
   else
      x->control = medusa_control_create(name->s_name);

   medusa_control_set_add_node_callback(
         x->control,
         medusa_control_add_node,
         x);

   medusa_control_set_remove_node_callback(
         x->control,
         medusa_control_remove_node,
         x);

   medusa_control_set_add_node_audio_resource_callback(
         x->control,
         medusa_control_add_node_audio_resource,
         x);

   medusa_control_set_remove_node_audio_resource_callback(
         x->control,
         medusa_control_remove_node_audio_resource,
         x);

   medusa_control_set_add_node_midi_resource_callback(
         x->control,
         medusa_control_add_node_midi_resource,
         x);

   medusa_control_set_remove_node_midi_resource_callback(
         x->control,
         medusa_control_remove_node_midi_resource,
         x);

   x->node_list_symbol_outlet = outlet_new(&x->x_obj, gensym("symbol"));

   return (void *) x;
}

// Destroy the class
void medusa_control_destroy(t_medusa_pd_control *x) {
   medusa_control_free(x->control);
}

void medusa_control_setup(void) {
   medusa_control_class = class_new(gensym("medusa_control"),
      (t_newmethod) medusa_control_new, // Constructor
      (t_method) medusa_control_destroy, // Destructor
      sizeof (t_medusa_pd_control),
      CLASS_DEFAULT,
      A_DEFSYM, // Number of channels
      0);//Must always ends with a zero

   class_sethelpsymbol(medusa_control_class,gensym ("medusa_control_help"));
   class_addmethod(
            medusa_control_class,
            (t_method) medusa_control_list_nodes,
            gensym("list_node"),
            0);
}

