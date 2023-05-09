#ifndef CALLBACK_H
#define CALLBACK_H

#include <iostream>
#include "portaudio.h"

//Datos para guardar guardar el audio.
struct DATA {
	DATA(unsigned int N_samples = 0);
	void create_array(unsigned int N_samples); //crea el arreglo de datos.
	~DATA();

	unsigned int item;  //para navegar en el arreglo de datos (rec_samp).
	unsigned int N_samples; //cantidad de datos (samples).
	float *samples; //recorded samples.
};

//CALLBACKS.
//Aclaración: el input es el sonido que escucha el programa, el output es el sonido que reproduce el programa.
int record_cb(const void *input, void *output, unsigned long N_frames, const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data);
int playback_cb(const void *input, void *output, unsigned long N_frames, const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data);

#endif //CALLBACK_H