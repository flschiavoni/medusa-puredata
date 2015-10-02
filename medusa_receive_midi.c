#include "medusa_pd.h"

/* -----------------------------------------------------------------------------
   MEDUSA PD RECEIVE MIDI NEW
------------------------------------------------------------------------------*/
void * medusa_pd_receive_midi_new(t_symbol * node_name) {
    t_medusa_pd_receive_midi *x 
            = (t_medusa_pd_receive_midi *) pd_new(medusa_pd_receive_midi_class);

   x->control = medusa_control_create("");


   if(strcmp(node_name->s_name, "") == 0){
      post("Please, inform sender name");
      return 0;
   }

   t_medusa_node_midi_resource * midi_resource
         = medusa_control_get_midi_resource_by_name(x->control,
               node_name->s_name);

   if(midi_resource == NULL){
      post("Resource not found");
      return NULL;
   }

   x->receiver = medusa_receiver_create(x->control, MEDUSA_MIDI, "PD");
   medusa_receiver_create_midi_resource(
                  x->receiver,
                  midi_resource->channels);

   medusa_receiver_connect_resource(x->receiver, node_name->s_name);
   return (void *) x;
}

/* -----------------------------------------------------------------------------
   MEDUSA PD RECEIVE MIDI FREE
------------------------------------------------------------------------------*/
static void medusa_pd_receive_midi_free(t_medusa_pd_receive_midi *x){
   x->number_of_channels = 0;
   medusa_control_free(x->control);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD RECEIVE MIDI SETUP
------------------------------------------------------------------------------*/
void medusa_receive_midi_setup(void) {
    medusa_pd_receive_midi_class = class_new(gensym("medusa_receive_midi"),
            (t_newmethod) medusa_pd_receive_midi_new, // Constructor
            (t_method) medusa_pd_receive_midi_free,
            sizeof (t_medusa_pd_receive_midi),
            CLASS_DEFAULT,
            A_DEFSYM, // Number of channels
            0);
}
/*----------------------------------------------------------------------------*/
