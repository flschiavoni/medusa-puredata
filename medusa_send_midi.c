#include "medusa_pd.h"

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND MIDI NEW
------------------------------------------------------------------------------*/
void * medusa_pd_send_midi_new(t_symbol *name_arg){
    t_medusa_pd_send_midi *x 
                     = (t_medusa_pd_send_midi *)
                     pd_new(medusa_pd_send_midi_class);

   x->control = medusa_control_create("");
   x->sender = medusa_sender_create(x->control, MEDUSA_MIDI, name_arg->s_name);

   medusa_sender_create_midi_resource(
                  x->sender,
                  1);

   x->my_outlet = outlet_new(&x->x_obj, gensym("symbol"));

    return (void *) x;
}

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND MIDI CONNECT
------------------------------------------------------------------------------*/
void medusa_pd_send_midi_send_list_method(
      t_medusa_pd_send_midi *x,
      t_symbol *s,
      int argc,
      t_atom *argv){
   (void) s;
   uint8_t values[argc];
   post("%d", argc);
   int i = 0;
   for(i = 0; i < argc; i++)
      values[i] = (uint8_t)(float)atom_getfloat(argv + i);
   medusa_sender_prepare_data(x->sender, 0, values, argc);
   medusa_sender_send_data(x->sender);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND MIDI CONNECT
------------------------------------------------------------------------------*/
void medusa_pd_send_midi_connect(
      t_medusa_pd_send_midi *x,
      t_symbol * protocol,
      t_floatarg port){

   medusa_sender_start_network(
            x->sender,
            medusa_util_get_protocol(protocol->s_name),
            (int)port);

   outlet_symbol(x->my_outlet, gensym("Connected"));

}

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND MIDI INFO
------------------------------------------------------------------------------*/
void medusa_pd_send_midi_info(t_medusa_pd_send_midi *x){
   t_medusa_list * list = medusa_control_get_nodes(x->control);
   char msg[1024];
   while(list){
      t_medusa_node * node = (t_medusa_node *) list->data;
      t_medusa_list * resource = medusa_node_get_audio_resources(node);
      while(resource){
         t_medusa_node_audio_resource * audio_resource
               = (t_medusa_node_audio_resource *) resource->data;
         sprintf(msg, "%s@%s", audio_resource->name, medusa_node_get_name(node));
         outlet_symbol(x->my_outlet, gensym(msg));
         post("AUDIO RESOURCE");
         resource = resource->next;
      }
      resource = medusa_node_get_midi_resources(node);
      while(resource){
         t_medusa_node_midi_resource * midi_resource
               = (t_medusa_node_midi_resource *) resource->data;
         sprintf(msg, "%s@%s", midi_resource->name, medusa_node_get_name(node));
         outlet_symbol(x->my_outlet, gensym(msg));
         post("MIDI RESOURCE");
         resource = resource->next;
      }
      list = list->next;
   }

}

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND MIDI FREE
------------------------------------------------------------------------------*/
static void medusa_pd_send_midi_free(t_medusa_pd_send_midi * x){
   medusa_control_free(x->control);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND MIDI SETUP
------------------------------------------------------------------------------*/
void medusa_send_midi_setup(void){
    medusa_pd_send_midi_class = class_new(gensym("medusa_send_midi"),
            (t_newmethod) medusa_pd_send_midi_new, // Constructor
            (t_method) medusa_pd_send_midi_free, //destructor
            sizeof (t_medusa_pd_send_midi),
            CLASS_DEFAULT,
            A_DEFSYMBOL, // Name
            0);

   class_addmethod(medusa_pd_send_midi_class,
         (t_method) medusa_pd_send_midi_connect,
         gensym("set"),
         A_DEFSYMBOL,
         A_DEFFLOAT,
         0);

   class_addmethod(medusa_pd_send_midi_class,
         (t_method) medusa_pd_send_midi_info,
         gensym("info"),
         0);

   class_addlist(
         medusa_pd_send_midi_class,
         medusa_pd_send_midi_send_list_method);
}

/*----------------------------------------------------------------------------*/
