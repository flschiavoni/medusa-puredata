#include "medusa_pd.h"

/* -----------------------------------------------------------------------------
   MEDUSA PD RECEIVE AUDIO CONNECT
------------------------------------------------------------------------------*/
void medusa_pd_receive_audio_list_nodes(t_medusa_pd_receive_audio *x){
   t_medusa_list * list = medusa_control_get_nodes(x->control);
   char msg[1024];
   while(list){
      t_medusa_node * node = (t_medusa_node *) list->data;
      t_medusa_list * resource = medusa_node_get_audio_resources(node);
      while(resource){
         t_medusa_node_audio_resource * audio_resource
               = (t_medusa_node_audio_resource *) resource->data;
         sprintf(msg, "%s@%s", audio_resource->name, medusa_node_get_name(node));
         post(msg);
         resource = resource->next;
      }
      resource = medusa_node_get_midi_resources(node);
      while(resource){
         t_medusa_node_midi_resource * midi_resource
               = (t_medusa_node_midi_resource *) resource->data;
         sprintf(msg, "%s@%s", midi_resource->name, medusa_node_get_name(node));
         post(msg);
         resource = resource->next;
      }
      list = list->next;
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA PD RECEIVE AUDIO CONNECT
------------------------------------------------------------------------------*/
void medusa_pd_receive_audio_connect(t_medusa_pd_receive_audio * x,
         t_symbol * node){
   if(medusa_receiver_connect_resource(x->receiver, node->s_name) == 0){
      post("Resource or node not found or with a wrong resource type.");
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA PD RECEIVE AUDIO PERFORM
------------------------------------------------------------------------------*/
static t_int * medusa_pd_receive_audio_perform(t_int *w) {
    t_medusa_pd_receive_audio * x = (t_medusa_pd_receive_audio *) (w[1]);

    if(x->number_of_channels < 1 
         || medusa_receiver_get_status(x->receiver) == MEDUSA_DISCONNECTED)
       return (w + 3);

    int i, data_size = w[2];

    for(i = 0; i < x->number_of_channels ; i++){
      medusa_receiver_read_data(
               x->receiver,
               i,
               (void *) x->outvec[i],
               (int) (data_size * sizeof(t_float)));
    }

    return (w + 3);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD RECEIVE AUDIO DSP
------------------------------------------------------------------------------*/
static void medusa_pd_receive_audio_dsp(t_medusa_pd_receive_audio *x, t_signal **sp) {
   if(x->number_of_channels < 1) return;
   int i = 0;
   for(; i < x->number_of_channels ; i++){
      x->outvec[i] = malloc(sys_getblksize() * sizeof(t_sample));
      x->outvec[i] = sp[i]->s_vec;
   }
   dsp_add(medusa_pd_receive_audio_perform, 2, x, sp[0]->s_n);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD RECEIVE AUDIO NEW
------------------------------------------------------------------------------*/
void * medusa_pd_receive_audio_new(t_floatarg number_of_channels) {
    t_medusa_pd_receive_audio *x 
         = (t_medusa_pd_receive_audio *) pd_new(medusa_pd_receive_audio_class);

   x->control = medusa_control_create("");

   x->number_of_channels = (number_of_channels > 0)?number_of_channels:1;

   x->receiver = medusa_receiver_create(x->control, MEDUSA_AUDIO, "PD_rcv");
   medusa_receiver_create_audio_resource(
                  x->receiver,
                  x->number_of_channels,
                  sys_getsr(),
                  sizeof(t_sample),
                  sys_getblksize());

   short i;
   for (i = 0; i < x->number_of_channels; i++) {
      outlet_new(&x->x_obj, &s_signal);
   }

   x->outvec = (t_sample **) malloc(sizeof(t_sample) * x->number_of_channels);

    return (void *) x;
}

/* -----------------------------------------------------------------------------
   MEDUSA PD RECEIVE AUDIO FREE
------------------------------------------------------------------------------*/
static void medusa_pd_receive_audio_free(t_medusa_pd_receive_audio *x){
   x->number_of_channels = 0;
   medusa_control_free(x->control);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD RECEIVE AUDIO TILDE SETUP
------------------------------------------------------------------------------*/
void medusa_receive_tilde_setup(void) {
    medusa_pd_receive_audio_class = class_new(gensym("medusa_receive~"),
            (t_newmethod) medusa_pd_receive_audio_new, // Constructor
            (t_method) medusa_pd_receive_audio_free,
            sizeof (t_medusa_pd_receive_audio),
            CLASS_DEFAULT,
            A_DEFFLOAT, // Resource name
            0);

   class_addmethod(medusa_pd_receive_audio_class,
            (t_method) medusa_pd_receive_audio_dsp,
            gensym("dsp"),
            (t_atomtype) 0);
   class_addmethod(
            medusa_pd_receive_audio_class,
            (t_method) medusa_pd_receive_audio_connect,
            gensym("connect"),
            A_DEFSYMBOL,
            0);

   class_addmethod(
            medusa_pd_receive_audio_class,
            (t_method) medusa_pd_receive_audio_list_nodes,
            gensym("list_node"),
            0);

}

/*----------------------------------------------------------------------------*/
