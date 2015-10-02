#include "medusa_pd.h"

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND AUDIO CONNECT
------------------------------------------------------------------------------*/
void medusa_pd_send_audio_connect(
         t_medusa_pd_send_audio *x,
         t_symbol * protocol,
         t_floatarg port){

   medusa_sender_start_network(
            x->sender,
            medusa_util_get_protocol(protocol->s_name),
            (int)port);
   post("Connected: Protocol %s Port %d", 
         medusa_util_get_protocol_name(
               medusa_sender_get_network_config(x->sender)->protocol),
         medusa_network_config_get_port(
               medusa_sender_get_network_config(x->sender))
               );
}

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND AUDIO PERFORM
------------------------------------------------------------------------------*/
static t_int * medusa_pd_send_audio_perform(t_int *w) {
   t_medusa_pd_send_audio * x = (t_medusa_pd_send_audio *) (w[1]);

   if(x->number_of_channels < 1 
      || medusa_sender_get_status(x->sender) == MEDUSA_DISCONNECTED)
      return (w + 3);

   int data_size = w[2];
   int i = 0;
   for(i = 0; i < x->number_of_channels; i++){
      medusa_sender_prepare_data(
         x->sender,
         i,
         (void *) x->invec[i],
         (int) (data_size * sizeof(t_float)));
   }
   medusa_sender_send_data(x->sender);
   return (w + 3);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND AUDIO DSP
------------------------------------------------------------------------------*/
static void medusa_pd_send_audio_dsp(t_medusa_pd_send_audio *x, t_signal **sp) {
   if(x->number_of_channels < 1) return;
   int i = 0;
   for(; i < x->number_of_channels ; i++){
      x->invec[i] = malloc(sys_getblksize() * sizeof(t_sample));
      x->invec[i] = sp[i]->s_vec;
   }
   dsp_add(medusa_pd_send_audio_perform, 2, x, sp[0]->s_n);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND AUDIO NEW
------------------------------------------------------------------------------*/
void * medusa_pd_send_audio_new(t_floatarg number_of_channels) {
   t_medusa_pd_send_audio *x = (t_medusa_pd_send_audio *) pd_new(medusa_pd_send_audio_class);
   if((int)(number_of_channels) < 1){
     post("Number of channels is a mandatory argument");
     return NULL;
   }
   x->number_of_channels = (number_of_channels > 0)?number_of_channels:1;

   x->invec = (t_sample **) malloc(sizeof(t_sample) * x->number_of_channels);
   x->control = medusa_control_create("");
   x->sender = medusa_sender_create(x->control, MEDUSA_AUDIO, "PD_send");

   medusa_sender_create_audio_resource(
              x->sender,
              (int)number_of_channels,
              sys_getsr(),
              MEDUSA_32_BITS,
              sys_getblksize());

   short i;
   for (i = 0; i < x->number_of_channels - 1; i++) {
      inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal); // signal inlets
   }

   return (void *) x;
}

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND AUDIO FREE
------------------------------------------------------------------------------*/
static void medusa_pd_send_audio_free(t_medusa_pd_send_audio * x){
   x->number_of_channels = 0;
   medusa_control_free(x->control);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD SEND AUDIO SETUP
------------------------------------------------------------------------------*/
void medusa_send_tilde_setup(void){
   medusa_pd_send_audio_class = class_new(gensym("medusa_send~"),
         (t_newmethod) medusa_pd_send_audio_new, // Constructor
         (t_method) medusa_pd_send_audio_free, //destructor
         sizeof (t_medusa_pd_send_audio),
         CLASS_DEFAULT,
         A_DEFFLOAT, // Number of channels
         0);

   CLASS_MAINSIGNALIN(
         medusa_pd_send_audio_class,
         t_medusa_pd_send_audio, x_f);
   class_addmethod(
         medusa_pd_send_audio_class,
         (t_method) medusa_pd_send_audio_dsp,
         gensym("dsp"),
         0);
   class_addmethod(
         medusa_pd_send_audio_class,
         (t_method) medusa_pd_send_audio_connect,
         gensym("connect"),
         A_DEFSYMBOL,
         A_DEFFLOAT, 
         0);
}

/* ---------------------------------------------------------------------------*/
